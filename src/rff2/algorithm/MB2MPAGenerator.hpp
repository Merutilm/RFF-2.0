//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>

#include "../algorithm/MPAIndexMapper.hpp"
#include "../algorithm/MPAUtils.hpp"
#include "../app/FnListeners.hpp"
#include "../constants/Constants.hpp"
#include "../data/ApproxTableBuffer.h"
#include "../data/MPAData.hpp"
#include "../data/ReferenceData.hpp"
#include "../parallel/ParallelRenderState.h"
#include "../util/ArrayUtils.hpp"
#include "HNREMPAView.hpp"
#include "MPAIndexMapperUtils.hpp"
#include "MPAInfo.hpp"
#include "PAGenerator.h"
#include "RFFResult.hpp"
#include "srpc_module/SRPCUnit.hpp"
#include "srpc_module/SRPCUtils.hpp"

#include "vulkan-helper/base/logger.hpp"

namespace merutilm::rff2 {

    template<Number Num>
    struct UProcessor {

        std::vector<itt::ref> itCount;
        std::vector<itt::ref> itCountLim;
        std::vector<PAGenerator<Num>> currentPA;
        std::vector<uint64_t> currentPASkips;
        std::vector<bool> generationAvailable;
        std::vector<bool> isPartial;
        size_t flattenTableIndex = 0;
        itt::ref iteration = 0;
        std::vector<std::pair<std::optional<PAGenerator<Num>>, std::optional<PAGenerator<Num>>>> partialPA;
    };

    template<Number Num>
    struct CProcessor {

        std::vector<itt::ref> itCount;
        std::vector<itt::ref> itCountLim;
        std::vector<PAGenerator<Num>> currentPA;
        std::vector<uint64_t> currentPASkips;
        std::vector<bool> generationAvailable;
        itt::ref pulledTableIndex = 0;
        size_t flattenTableIndex = 0;
        itt::ref iteration = 0;
    };


    // MP-Approximation table.
    // prefix c: For compressed table.
    // prefix u: For uncompressed table. Arithmetic overflowing is not permitted.
    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    class MB2MPAGenerator {

        // input value
        const ParallelRenderState &state;
        const ReferenceData<Num> &reference;
        ApproxTableBuffer &buffer;

        const FrtGeneralSettings &generalSettings;
        const FrtMPASettings &mpaSettings;
        const bool computeShaderUsed;
        Num dcMax;
        FnCreatingTable fnCreatingTable;


        // result
        std::unique_ptr<MPAData<Num>> mpaData;

    public:
        explicit MB2MPAGenerator(const ParallelRenderState &state, const ReferenceData<Num> &reference,
                                 ApproxTableBuffer &buffer, const FrtGeneralSettings &generalSettings,
                                 const FrtMPASettings &mpaSettings, bool computeShaderUsed, Num dcMax,
                                 FnCreatingTable &&fnCreatingTable) :
            state(state), reference(reference), buffer(buffer), generalSettings(generalSettings),
            mpaSettings(mpaSettings), computeShaderUsed(computeShaderUsed), dcMax(dcMax),
            fnCreatingTable(std::move(fnCreatingTable)) {}

        RFFResult generate();

        std::unique_ptr<MPAData<Num>> extract() &&;

    private:
        void calcBufferSize(HNREMPAData<Num> &data, size_t &tableLen, size_t &mapperLen) const;

        void fitBufferSize() const;

        std::span<PA<Num>> getMutableMPAFromMapper(HNREMPAData<Num> &data, MPAIndexMapper flattenIndexMapper);

        RFFResult generateTable(HNREMPAData<Num> &data);

        [[nodiscard]] UProcessor<Num> uInitProcessor(HNREMPAData<Num> &data, uint64_t startIteration,
                                                     double epsilon) const;

        void uVerifyPA(HNREMPAData<Num> &data, UProcessor<Num> &up);

        void uRefreshCounter(HNREMPAData<Num> &data, UProcessor<Num> &up);

        void uStepOnce(HNREMPAData<Num> &data, UProcessor<Num> &up);

        void uConfigurePartialPA(HNREMPAData<Num> &data, UProcessor<Num> &up);

        void uGluePartialPA(HNREMPAData<Num> &data, std::vector<UProcessor<Num>> &ups);

        RFFResult uGenerateTable(HNREMPAData<Num> &data);

        [[nodiscard]] CProcessor<Num> cInitProcessor(HNREMPAData<Num> &data, double epsilon);

        bool cTryJumpTableGeneration(HNREMPAData<Num> &data, CProcessor<Num> &cp);

        void cVerifyPA(HNREMPAData<Num> &data, CProcessor<Num> &cp);

        void cRefreshCounter(HNREMPAData<Num> &data, CProcessor<Num> &cp);

        void cStepOnce(HNREMPAData<Num> &data, CProcessor<Num> &cp);

        RFFResult cGenerateTable(HNREMPAData<Num> &data);

#ifndef NDEBUG
        void checkZero(HNREMPAData<Num> &data);
#endif
    };

    // DEFINITION OF MPA TABLE


    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    RFFResult MB2MPAGenerator<Num, FnCreatingTable>::generate() {
        mpaData = std::make_unique<MPAData<Num>>();
        mpaData->mpaSettings = mpaSettings;
        for (const HNREReferenceData<Num> &hnreData: reference.hnreList) {
            const auto &referencePeriod = hnreData.generatedPeriods;
            const itt::ref longestPeriod = hnreData.longestPeriod();

            HNREMPAData<Num> data;
            data.targetHNRE = &hnreData;

            if (const int minSkip = mpaSettings.minSkipReference; longestPeriod <= minSkip) {
                mpaData->hnreList.emplace_back(std::move(data));
            } else {
                data.buffer = &buffer;
                data.info = MPAInfo(referencePeriod, hnreData.getCompressor(), mpaSettings);
                mpaData->hnreList.emplace_back(std::move(data));
            }
        }

        fitBufferSize();

        for (auto &hnreData: mpaData->hnreList) {
            if (RFFResult result; hnreData.buffer && (result = generateTable(hnreData)) != RFFResult::SUCCESS)
                return result;
        }
        return RFFResult::SUCCESS;
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    std::unique_ptr<MPAData<Num>> MB2MPAGenerator<Num, FnCreatingTable>::extract() && {
        return std::move(mpaData);
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::calcBufferSize(HNREMPAData<Num> &data, size_t &tableLen,
                                                               size_t &mapperLen) const {
        if (!data.buffer) return;

        const itt::ref longestPeriod = data.info.tablePeriods.back();

        tableLen = data.info.tableElementCounts.back();
        mapperLen = 0;

        if (data.info.pulledMPACompressor.empty()) {
            mapperLen = longestPeriod + 1;
        } else {
            const auto pulledTableIndex =
                    MPAIndexMapperUtils::iterationToPulledTableIndex(data.info, longestPeriod + 1);
            const auto compressedIndex = SRPCUtils::compress(data.info.pulledMPACompressor, pulledTableIndex);
            mapperLen = compressedIndex;

            for (const auto &compressor: data.info.pulledMPACompressor) {
                const itt::ref i = ArrayUtils::binarySearch(data.info.skippableIterationCounts, compressor.range() + 1);
                RFF_ASSERT(i != UINT64_MAX);
                tableLen -= data.info.tableElementCounts[i] - (i + 1);
            }
        }

        if (static_cast<itt::ref>(static_cast<size_t>(tableLen)) != tableLen) {
            throw vkh::exception_invalid_state("table buffer overflow");
        }

        if (static_cast<itt::ref>(static_cast<size_t>(mapperLen)) != mapperLen) {
            throw vkh::exception_invalid_state("mapper buffer overflow");
        }
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::fitBufferSize() const {
        size_t totalTableLen = 0;
        size_t totalMapperLen = 0;
        for (HNREMPAData<Num> &data: mpaData->hnreList) {
            size_t tableLen = 0;
            size_t mapperLen = 0;
            calcBufferSize(data, tableLen, mapperLen);
            data.tableOffset = totalTableLen;
            data.mapperOffset = totalMapperLen;
            data.tableSize = tableLen;
            data.mapperSize = mapperLen;
            totalTableLen += tableLen;
            totalMapperLen += mapperLen;
        }

        buffer.resize<PA<Num>>(static_cast<size_t>(totalTableLen), static_cast<size_t>(totalMapperLen),
                               computeShaderUsed);
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    std::span<PA<Num>>
    MB2MPAGenerator<Num, FnCreatingTable>::getMutableMPAFromMapper(HNREMPAData<Num> &data,
                                                                   const MPAIndexMapper flattenIndexMapper) {
        const size_t levels = data.info.tablePeriods.size();
        size_t mapped = flattenIndexMapper.mapped;
        size_t size = flattenIndexMapper.generatedLevels;
        MPAUtils::debugCheckMPAFromMapper(data.tableSize, mapped, levels, size);
        return data.interpretPA(mapped, size);
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    bool MB2MPAGenerator<Num, FnCreatingTable>::cTryJumpTableGeneration(HNREMPAData<Num> &data, CProcessor<Num> &cp) {

        if (data.info.pulledMPACompressor.empty())
            return false;

        const PulledMPACompressionUnit *containedTool =
                SRPCUtils::find(data.info.pulledMPACompressor, cp.pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != cp.pulledTableIndex + 1) {
            return false;
        }
        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();
        const auto &skippableIterationsCount = data.info.skippableIterationCounts;
        const size_t level =
                ArrayUtils::binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const size_t generatedLevels = MPAIndexMapperUtils::iterationToGeneratedLevels(data.info, cp.iteration);
        const auto mainReferenceMPA = getMutableMPAFromMapper(data, {0, generatedLevels});

        if (level >= mainReferenceMPA.size() || level + 1 > generatedLevels) {
            throw vkh::exception_invalid_state(
                    "Invalid level detected. it might be a bug! Please contact the developer and "
                    "attach the current location file (.rfl)");
        }

        const PA<Num> &mainReferencePA = mainReferenceMPA[level];
        const itt::ref skip = mainReferencePA.skip;

        auto mpa = getMutableMPAFromMapper(data, {.mapped = cp.flattenTableIndex, .generatedLevels = generatedLevels});


        for (uint32_t i = level + 1; i < levels; ++i) {
            if (i <= level && cp.itCount[i] != 0) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} is not zero.",
                                 cp.itCount[i]);
                return false;
            }
            if (cp.itCount[i] + skip > tablePeriods[i] - MPAUtils::PERTURBATION_REQ) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} + "
                                 "skip {} exceeds its period {}.",
                                 cp.itCount[i], skip, tablePeriods[i]);
                return false;
            }
        }

        cp.pulledTableIndex += skippableIterationsCount[level];
        cp.iteration += skip;
        cp.flattenTableIndex += generatedLevels;

        for (size_t i = 0; i < level + 1; ++i) {
            mpa[i] = mainReferenceMPA[i];
            cp.currentPA[i].reuse(cp.iteration);
            cp.itCount[i] = 0;
            cp.itCountLim[i] = MPAUtils::PERTURBATION_REQ;
            cp.generationAvailable[i] = false;
        }

        if (level + 1 < levels) {
            cp.itCount[level + 1] += skip;
            cp.currentPA[level + 1].merge(mainReferencePA);
        }
        return true;
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::uVerifyPA(HNREMPAData<Num> &data, UProcessor<Num> &up) {

        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        size_t level = 0;
        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();

#ifndef NDEBUG
        if (up.currentPASkips[level] > tablePeriods[level] - MPAUtils::PERTURBATION_REQ) {
            throw vkh::exception_invalid_state("skip count is exceeded");
        }
#endif

        while (level < levels && (up.currentPASkips[level] == tablePeriods[level] - MPAUtils::PERTURBATION_REQ ||
                                  up.itCountLim[level] != tablePeriods[level] || !up.generationAvailable[level])) {

            if (up.itCountLim[level] == tablePeriods[level] && up.generationAvailable[level]) {


                if (up.isPartial[level]) {
                    up.partialPA[level].first.emplace(up.currentPA[level]);
                    up.isPartial[level] = false;
                } else {
                    const MPAIndexMapper flattenIndexMapper = data.interpretMapper(up.currentPA[level].start);
#ifndef NDEBUG
                    if (level >= flattenIndexMapper.generatedLevels) {
                        throw std::invalid_argument("invalid level provided");
                    }
#endif
                    auto pa = getMutableMPAFromMapper(data, flattenIndexMapper);
                    pa[level] = up.currentPA[level].build();
                }

                up.generationAvailable[level] = false;
            }

            if (level < levels - 1) {
                up.currentPASkips[level + 1] += up.currentPASkips[level];
                up.currentPA[level + 1].merge(up.currentPA[level]);
            }
            up.currentPASkips[level] = 0;
            up.currentPA[level].reuse(up.iteration);
            ++level;
        }
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::cVerifyPA(HNREMPAData<Num> &data, CProcessor<Num> &cp) {

        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        const auto &tablePeriods = data.info.tablePeriods;
        itt::ref level = 0;
        const itt::ref levels = tablePeriods.size();

        while (level < levels && (cp.currentPA[level].skip == tablePeriods[level] - MPAUtils::PERTURBATION_REQ ||
                                  cp.itCountLim[level] != tablePeriods[level] || !cp.generationAvailable[level])) {

            if (cp.itCountLim[level] == tablePeriods[level] && cp.generationAvailable[level]) {
                const MPAIndexMapper flattenIndexMapper =
                        MPAUtils::cGetFlattenIndexMapper(data, cp.currentPA[level].start);

                auto pa = getMutableMPAFromMapper(data, flattenIndexMapper);
#ifndef NDEBUG
                if (level >= flattenIndexMapper.generatedLevels) {
                    throw std::invalid_argument("invalid level provided");
                }
#endif

                pa[level] = cp.currentPA[level].build();
                cp.generationAvailable[level] = false;
            }

            if (level < levels - 1) {
                cp.currentPA[level + 1].merge(cp.currentPA[level]);
            }
            cp.currentPA[level].reuse(cp.iteration);
            ++level;
        }
    }


    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::cRefreshCounter(HNREMPAData<Num> &data, CProcessor<Num> &cp) {


        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        size_t level = 0;
        const auto &tablePeriods = data.info.tablePeriods;
        while (level < tablePeriods.size() - 1 && cp.itCount[level] == cp.itCountLim[level]) {
            cp.itCount[level + 1] += cp.itCount[level];
            cp.currentPA[level + 1].merge(cp.currentPA[level]);
            cp.currentPA[level].reuse(cp.iteration);
            ++level;
        }

        while (level > 0) {
            --level;
            cp.itCountLim[level] = std::min(tablePeriods[level], cp.itCountLim[level + 1] - cp.itCount[level + 1]);
            cp.itCount[level] = 0;
            cp.generationAvailable[level] = cp.itCountLim[level] == tablePeriods[level];
        }
    }


    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::uRefreshCounter(HNREMPAData<Num> &data, UProcessor<Num> &up) {


        // reset current and lower level count when it reached limit
        // Amortized O(1)

        size_t level = 0;

        const auto &tablePeriods = data.info.tablePeriods;
        while (level < tablePeriods.size() - 1 && up.itCount[level] == up.itCountLim[level]) {
            up.itCount[level + 1] += up.itCount[level];
            up.currentPASkips[level + 1] += up.currentPASkips[level];
            up.currentPA[level + 1].merge(up.currentPA[level]);
            up.isPartial[level] = false;
            up.currentPASkips[level] = 0;
            up.currentPA[level].reuse(up.iteration);
            ++level;
        }

        while (level > 0) {
            --level;
            up.itCountLim[level] = std::min(tablePeriods[level], up.itCountLim[level + 1] - up.itCount[level + 1]);
            up.itCount[level] = 0;
            up.generationAvailable[level] = up.itCountLim[level] == tablePeriods[level];
        }
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::uStepOnce(HNREMPAData<Num> &data, UProcessor<Num> &up) {
        uint64_t levels = 0;
        const auto &tablePeriods = data.info.tablePeriods;
        while (levels < tablePeriods.size() && up.itCount[levels] == 0 &&
               up.itCountLim[levels] == tablePeriods[levels]) {
            ++levels;
        }


        if (levels > 0) {
            MPAUtils::debugCheckMPAFromMapper(buffer.mpaTable.sizeUsed, up.flattenTableIndex, tablePeriods.size(),
                                              levels);
            RFF_ASSERT(data.mapperSize > up.iteration);
            data.interpretMapper(up.iteration) = {up.flattenTableIndex, levels};
            up.flattenTableIndex += levels;
        } else {
            RFF_ASSERT(data.mapperSize > up.iteration);
            data.interpretMapper(up.iteration) = {UINT64_MAX, 0};
        }
        up.currentPA[0].step();
        ++up.currentPASkips[0];
        ++up.itCount[0];
        ++up.iteration;
    }


    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::cStepOnce(HNREMPAData<Num> &data, CProcessor<Num> &cp) {
        size_t levels = 0;
        const auto &tablePeriods = data.info.tablePeriods;
        while (levels < tablePeriods.size() && cp.itCount[levels] == 0 &&
               cp.itCountLim[levels] == tablePeriods[levels]) {
            ++levels;
        }

        bool jumped = false;
        if (levels > 0) {

            const size_t compIndex = SRPCUtils::compress(data.info.pulledMPACompressor, cp.pulledTableIndex);
            RFF_ASSERT(buffer.flattenIndexMapper.sizeUsed > compIndex);
            data.interpretMapper(compIndex) = {.mapped = cp.flattenTableIndex, .generatedLevels = levels};

            jumped = cTryJumpTableGeneration(data, cp);
        }

        if (!jumped) {
            if (levels > 0) {
                ++cp.pulledTableIndex;
                cp.flattenTableIndex += levels;
            }

            cp.currentPA[0].step();
            ++cp.itCount[0];
            ++cp.iteration;
        }
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    RFFResult MB2MPAGenerator<Num, FnCreatingTable>::generateTable(HNREMPAData<Num> &data) {

        // iteration starts from 1, the first value is garbage
        data.interpretMapper(0) = {};

        if (data.info.pulledMPACompressor.empty()) {
            return uGenerateTable(data);
        } else {
            return cGenerateTable(data);
        }
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    CProcessor<Num> MB2MPAGenerator<Num, FnCreatingTable>::cInitProcessor(HNREMPAData<Num> &data, double epsilon) {
        CProcessor<Num> cp;
        // input vector must be empty.
        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();
        cp.itCount.resize(levels, 0);
        cp.itCountLim = tablePeriods;
        cp.generationAvailable.resize(levels, true);

        cp.currentPA = std::vector(levels, PAGenerator<Num>(*data.targetHNRE, epsilon, dcMax, 1));

        cp.iteration = 1;
        cp.pulledTableIndex = 0;
        cp.flattenTableIndex = 0;

        return cp;
    }


    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    UProcessor<Num> MB2MPAGenerator<Num, FnCreatingTable>::uInitProcessor(HNREMPAData<Num> &data,
                                                                          const uint64_t startIteration,
                                                                          double epsilon) const {
        UProcessor<Num> up;

        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();

        up.itCount.resize(levels);
        up.itCountLim.resize(levels);
        up.generationAvailable.resize(levels);
        up.isPartial.resize(levels);
        up.partialPA.resize(levels);

        uint64_t remainder = startIteration - 1;
        uint64_t lim = UINT64_MAX;

        for (uint64_t i = levels; i > 0; --i) {
            const uint64_t level = i - 1;
            const uint64_t p = tablePeriods[level];
            const uint64_t quotient = remainder / p;

            lim = std::min(static_cast<uint64_t>(tablePeriods[level]), lim - quotient * p);
            remainder -= quotient * p;

            up.itCount[level] = remainder;
            up.itCountLim[level] = lim;
            up.generationAvailable[level] = lim == p && up.itCount[level] < lim - MPAUtils::PERTURBATION_REQ;
            up.isPartial[level] = remainder > 0;
        }

        for (size_t level = levels - 1; level > 0; --level) {
            up.itCount[level] -= up.itCount[level - 1];
        }

        up.currentPASkips = up.itCount;
        for (size_t j = 0; j < levels - 1; ++j) {
            if (up.currentPASkips[j] >= static_cast<uint64_t>(tablePeriods[j]) - MPAUtils::PERTURBATION_REQ) {
                up.currentPASkips[j + 1] += up.currentPASkips[j];
                up.currentPASkips[j] = 0;
            }
        }

        up.flattenTableIndex = MPAIndexMapperUtils::iterationToNearestFlattenTableIndex(data.info, startIteration);
        up.currentPA = std::vector(levels, PAGenerator<Num>(*data.targetHNRE, epsilon, dcMax, startIteration));
        up.iteration = startIteration;

        return up;
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    RFFResult MB2MPAGenerator<Num, FnCreatingTable>::cGenerateTable(HNREMPAData<Num> &data) {

        const auto &tablePeriods = data.info.tablePeriods;
        const itt::ref longestPeriod = tablePeriods.back();
        const auto epsilonPower = mpaSettings.epsilonPower;
        const float epsilon = std::pow(10.f, epsilonPower);
        CProcessor<Num> cp = cInitProcessor(data, epsilon);

        while (cp.iteration <= longestPeriod) {
            if (cp.iteration % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0 &&
                state.interruptRequested()) {
                return RFFResult::TERMINATED;
            }

            fnCreatingTable(static_cast<double>(cp.iteration) / static_cast<double>(longestPeriod));
            cStepOnce(data, cp);
            cVerifyPA(data, cp);
            cRefreshCounter(data, cp);
        }
#ifndef NDEBUG
        checkZero(data);
#endif
        return RFFResult::SUCCESS;
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::uConfigurePartialPA(HNREMPAData<Num> &data, UProcessor<Num> &up) {
        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();

        for (size_t j = 1; j < levels; ++j) {
            up.currentPA[j].merge(up.currentPA[j - 1]);
        }


        for (size_t j = 0; j < levels; ++j) {
            auto &pp = up.partialPA[j];

            if (up.isPartial[j]) {
                pp.first.emplace(up.currentPA[j]);
            }
            if (tablePeriods[j] == up.itCountLim[j]) {
                pp.second.emplace(up.currentPA[j]);
            }
        }


        for (size_t j = 0; j < levels; ++j) {
            auto &pp = up.partialPA[j];
            if (pp.first.has_value() && pp.first->skip == 0)
                pp.first.reset();
            if (pp.second.has_value() && pp.second->skip == 0)
                pp.second.reset();
        }
    }

    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::uGluePartialPA(HNREMPAData<Num> &data,
                                                               std::vector<UProcessor<Num>> &ups) {

        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();

        for (size_t i = 0; i < levels; ++i) {

            std::optional<PAGenerator<Num>> preservingPA = std::nullopt;

            for (size_t j = 1; j < ups.size(); ++j) {
                assert(i < ups[j - 1].partialPA.size());
                auto &referencePA = ups[j - 1].partialPA[i].second;

                if (referencePA.has_value()) {

                    if (!preservingPA.has_value())
                        preservingPA.emplace(*referencePA);

#ifndef NDEBUG
                    if (!ups[j].partialPA[i].first.has_value())
                        throw vkh::exception_invalid_state("that is a bug");
#endif
                    preservingPA->merge(*ups[j].partialPA[i].first);

                    if (preservingPA->skip == tablePeriods[i] - MPAUtils::PERTURBATION_REQ) {
                        PA<Num> pa = preservingPA->build();
                        const size_t flattenIndex =
                                MPAIndexMapperUtils::iterationToFlattenTableIndex(data.info, preservingPA->start) + i;
#ifndef NDEBUG
                        if (flattenIndex == UINT64_MAX || data.interpretPA(flattenIndex).skip != 0)
                            throw vkh::exception_invalid_state("already assigned or flatten index cannot be found");
#endif
                        preservingPA.reset();
                        data.interpretPA(flattenIndex) = pa;
                    }
                }
            }
        }
    }
    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    RFFResult MB2MPAGenerator<Num, FnCreatingTable>::uGenerateTable(HNREMPAData<Num> &data) {

        const auto &tablePeriods = data.info.tablePeriods;
        const uint64_t longestPeriod = static_cast<uint64_t>(tablePeriods.back());
        const auto epsilonPower = mpaSettings.epsilonPower;
        const float epsilon = std::pow(10.f, epsilonPower);
        const uint32_t threadCount = generalSettings.threads;


        const size_t itInterval = std::max(static_cast<size_t>(tablePeriods[0]), longestPeriod / threadCount + 1);
        std::vector<UProcessor<Num>> ups(threadCount);
        std::vector<std::unique_ptr<std::jthread>> threads(threadCount);


        for (size_t i = 0; i < threadCount; ++i) {
            threads[i] = std::make_unique<std::jthread>(
                    [this, &data, i, itInterval, longestPeriod, epsilon, &ups, threadCount] {
                        const itt::ref startIteration = itInterval * i + 1;
                        ups[i] = uInitProcessor(data, startIteration, epsilon);

                        if (startIteration > longestPeriod || state.interruptRequested()) {
                            return;
                        }

                        UProcessor<Num> &up = ups[i];

                        while (up.iteration <= std::min(startIteration + itInterval - 1, longestPeriod)) {
                            if (up.iteration % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0) {
                                if (state.interruptRequested())
                                    return;

                                if (i == 0) {
                                    fnCreatingTable(std::min(1.0, static_cast<double>(up.iteration) * threadCount /
                                                                          static_cast<double>(longestPeriod)));
                                }
                            }

                            uStepOnce(data, up);
                            uVerifyPA(data, up);
                            uRefreshCounter(data, up);
                        }

                        uConfigurePartialPA(data, up);
                    });
        }

        for (const auto &thread: threads) {
            if (thread->joinable())
                thread->join();
        }

        if (state.interruptRequested())
            return RFFResult::TERMINATED;

        uGluePartialPA(data, ups);

#ifndef NDEBUG
        checkZero(data);
#endif
        return RFFResult::SUCCESS;
    }

#ifndef NDEBUG
    template<Number Num, FnListeners::FnCreatingTable FnCreatingTable>
    void MB2MPAGenerator<Num, FnCreatingTable>::checkZero(HNREMPAData<Num> &data) {
        for (size_t i = 0; i < data.tableSize; ++i) {
            auto &pa = data.interpretPA(i);
            if (state.interruptRequested())
                return;
            if (pa.skip == 0) {
                throw vkh::exception_invalid_state(std::format("zero skips detected at index {}", std::to_string(i)));
            }
        }
    }
#endif

} // namespace merutilm::rff2
