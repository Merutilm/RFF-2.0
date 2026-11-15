//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>

#include "../data/ApproxTableBuffer.h"
#include "../mb/MB2Reference.h"
#include "../parallel/ParallelRenderState.h"
#include "../util/ArrayUtils.hpp"
#include "MPAData.hpp"
#include "MPAIndexMapper.hpp"
#include "MPAIndexMapperUtils.hpp"
#include "MPAInfo.hpp"
#include "MPAUtils.hpp"
#include "MPAView.hpp"
#include "PAGenerator.h"
#include "SRPCUnit.hpp"
#include "SRPCUtils.hpp"

#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {

    // MP-Approximation table.
    // prefix c: For compressed table.
    // prefix u: For uncompressed table. Arithmetic overflowing is not permitted.
    template<Number Num>
    struct MPATable {

        using PartialPA = std::vector<std::pair<std::optional<PAGenerator<Num>>, std::optional<PAGenerator<Num>>>>;

        // table caches
        MPAData data;

        const FrtGeneralSettings generalSettings;
        const bool computeShaderUsed;

        template<FnListeners::FnCreatingTable FnCreatingTable>
        explicit MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                          ApproxTableBuffer &buffer, const FrtGeneralSettings &generalSettings,
                          const FrtMPASettings &mpaSettings, bool computeShaderUsed, Num dcMax,
                          FnCreatingTable &&fnCreatingTable);


    private:
        [[nodiscard]] bool tryInit(ApproxTableBuffer &buffer, const MB2Reference<Num> &reference,
                                   const FrtMPASettings &mpaSettings);

        void fitBufferSize() const;

        std::span<PA<Num>> getMutableMPAFromMapper(const MPAIndexMapper flattenIndexMapper);


        bool cTryJumpTableGeneration(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                     std::vector<PAGenerator<Num>> &currentPA, std::vector<bool> &generationAvailable,
                                     itt::ref &pulledTableIndex, size_t &flattenTableIndex, itt::ref &iteration);

        void uVerifyPA(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriods,
                       std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                       std::vector<uint64_t> &currentPASkips, std::vector<bool> *isPartial, PartialPA *partialPA,
                       uint64_t iteration);

        void cVerifyPA(const std::vector<itt::ref> &itCountLim, const std::vector<itt::ref> &tablePeriods,
                       std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                       itt::ref iteration);

        void cRefreshCounter(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                             const std::vector<itt::ref> &tablePeriods, std::vector<bool> &generationAvailable,
                             std::vector<PAGenerator<Num>> &currentPA, itt::ref iteration);

        void uRefreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                             const std::vector<uint64_t> &tablePeriods, std::vector<bool> &generationAvailable,
                             std::vector<PAGenerator<Num>> &currentPA, std::vector<uint64_t> &currentPASkips,
                             std::vector<bool> *isPartial, uint64_t iteration);

        void uStepOnce(std::vector<uint64_t> &itCount, const std::vector<uint64_t> &itCountLim,
                       const std::vector<uint64_t> &tablePeriods, std::vector<PAGenerator<Num>> &currentPA,
                       std::vector<uint64_t> &currentPASkips, uint64_t &flattenTableIndex, uint64_t &iteration);

        void cStepOnce(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                       const std::vector<itt::ref> &tablePeriods, std::vector<bool> &generationAvailable,
                       std::vector<PAGenerator<Num>> &currentPA, itt::ref &pulledTableIndex, size_t &flattenTableIndex,
                       itt::ref &iteration);

        template<FnListeners::FnCreatingTable FnCreatingTable>
        void generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                           FnCreatingTable &&fnCreatingTable);

        void cInitIterationCountVec(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                    std::vector<bool> &generationAvailable) const;

        void uGenerateIterationCountVec(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                        std::vector<bool> &generationAvailable, std::vector<bool> &isPartial,
                                        uint64_t iteration) const;

        template<FnListeners::FnCreatingTable FnCreatingTable>
        void cGenerateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                            FnCreatingTable &&fnCreatingTable);


        void uConfigurePartialPA(const std::vector<itt::ref> &tablePeriods, const std::vector<uint64_t> &itCountLim,
                                 std::vector<PAGenerator<Num>> &currentPA, std::vector<bool> &isPartial,
                                 PartialPA &partialPA);

        void uGluePartialPA(const std::vector<itt::ref> &tablePeriod, uint32_t threadCount,
                            std::vector<PartialPA> &partialPAs);

        template<FnListeners::FnCreatingTable FnCreatingTable>
        void uGenerateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                            FnCreatingTable &&fnCreatingTable);
#ifndef NDEBUG
        void checkZero(const ParallelRenderState &state);
#endif


    public:
        [[nodiscard]] MPAView<Num> makeMPAView() const;
    };

    // DEFINITION OF MPA TABLE


    template<Number Num>
    template<FnListeners::FnCreatingTable FnCreatingTable>
    MPATable<Num>::MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                            ApproxTableBuffer &buffer, const FrtGeneralSettings &generalSettings,
                            const FrtMPASettings &mpaSettings, const bool computeShaderUsed, Num dcMax,
                            FnCreatingTable &&fnCreatingTable) : generalSettings(generalSettings),
        computeShaderUsed(computeShaderUsed) {
        if (tryInit(buffer, reference, mpaSettings)) {
            generateTable(state, reference, dcMax, fnCreatingTable);
        }
    }


    //[re] init mpa periods and compressors
    template<Number Num>
    bool MPATable<Num>::tryInit(ApproxTableBuffer &buffer, const MB2Reference<Num> &reference, const FrtMPASettings &mpaSettings) {
        const auto &referencePeriod = reference.period;
        const itt::ref longestPeriod = reference.longestPeriod();

        if (const int minSkip = mpaSettings.minSkipReference; longestPeriod < minSkip) {
            return false;
        }
        data.buffer = &buffer;
        data.mpaSettings = mpaSettings;
        data.info = MPAInfo(referencePeriod, mpaSettings);
        data.compressor = MPAUtils::generatePulledMPACompressor(data.info, reference.getCompressor());
        return true;
    }



    template<Number Num>
    void MPATable<Num>::fitBufferSize() const {


        const itt::ref longestPeriod = data.info.tablePeriods.back();

        itt::ref tableLen = data.info.tableElementCounts.back();
        itt::ref mapperLen = 0;

        if (data.compressor.empty()) {
            mapperLen = longestPeriod + 1;
        } else {
            const auto pulledTableIndex =
                    MPAIndexMapperUtils::iterationToPulledTableIndex(data.info, longestPeriod + 1);
            const auto compressedIndex = SRPCUtils::compress(data.compressor, pulledTableIndex);
            mapperLen = compressedIndex;

            for (const auto &compressor: data.compressor) {
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

        data.buffer->resize<PA<Num>>(static_cast<size_t>(tableLen), static_cast<size_t>(mapperLen), computeShaderUsed);
    }

    template<Number Num>
    std::span<PA<Num>> MPATable<Num>::getMutableMPAFromMapper(const MPAIndexMapper flattenIndexMapper) {
        const size_t levels = data.info.tablePeriods.size();
        size_t size = flattenIndexMapper.generatedLevels;
        MPAUtils::debugCheckMPAFromMapper(data.buffer->mpaTable.sizeUsed, flattenIndexMapper.mapped, levels, size);
        PA<Num> *start = data.buffer->mpaTable.interpret<PA<Num>>() + flattenIndexMapper.mapped;
        return std::span<PA<Num>>(start, size);
    }

    template<Number Num>
    bool MPATable<Num>::cTryJumpTableGeneration(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                                std::vector<PAGenerator<Num>> &currentPA,
                                                std::vector<bool> &generationAvailable, itt::ref &pulledTableIndex,
                                                size_t &flattenTableIndex, itt::ref &iteration) {

        if (data.compressor.empty())
            return false;

        const PulledMPACompressionUnit *containedTool = SRPCUtils::find(data.compressor, pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != pulledTableIndex + 1) {
            return false;
        }
        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();
        const auto &skippableIterationsCount = data.info.skippableIterationCounts;
        const size_t level = ArrayUtils::binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const size_t generatedLevels = MPAIndexMapperUtils::iterationToGeneratedLevels(data.info, iteration);
        const auto mainReferenceMPA = getMutableMPAFromMapper({0, generatedLevels});

        if (level >= mainReferenceMPA.size() || level + 1 > generatedLevels) {
            throw vkh::exception_invalid_state(
                    "Invalid level detected. it might be a bug! Please contact the developer and "
                    "attach the current location file (.rfl)");
        }

        const PA<Num> &mainReferencePA = mainReferenceMPA[level];
        const itt::ref skip = mainReferencePA.skip;

        auto mpa = getMutableMPAFromMapper({.mapped = flattenTableIndex, .generatedLevels = generatedLevels});


        for (uint32_t i = level + 1; i < levels; ++i) {
            if (i <= level && itCount[i] != 0) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} is not zero.",
                                 itCount[i]);
                return false;
            }
            if (itCount[i] + skip > tablePeriods[i] - MPAUtils::PERTURBATION_REQ) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} + "
                                 "skip {} exceeds its period {}.",
                                 itCount[i], skip, tablePeriods[i]);
                return false;
            }
        }

        pulledTableIndex += skippableIterationsCount[level];
        iteration += skip;
        flattenTableIndex += generatedLevels;

        for (size_t i = 0; i < level + 1; ++i) {
            mpa[i] = mainReferenceMPA[i];
            currentPA[i].reuse(iteration);
            itCount[i] = 0;
            itCountLim[i] = MPAUtils::PERTURBATION_REQ;
            generationAvailable[i] = false;
        }

        if (level + 1 < levels) {
            itCount[level + 1] += skip;
            currentPA[level + 1].merge(mainReferencePA);
        }
        return true;
    }

    template<Number Num>
    void MPATable<Num>::uVerifyPA(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriods,
                                  std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                                  std::vector<uint64_t> &currentPASkips, std::vector<bool> *isPartial,
                                  PartialPA *partialPA, uint64_t iteration) {

        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        size_t level = 0;
        const size_t levels = tablePeriods.size();

#ifndef NDEBUG
        if (currentPASkips[level] > tablePeriods[level] - MPAUtils::PERTURBATION_REQ) {
            throw vkh::exception_invalid_state("skip count is exceeded");
        }
#endif

        while (level < levels && (currentPASkips[level] == tablePeriods[level] - MPAUtils::PERTURBATION_REQ ||
                                  itCountLim[level] != tablePeriods[level] || !generationAvailable[level])) {

            if (itCountLim[level] == tablePeriods[level] && generationAvailable[level]) {


                if (isPartial && partialPA && (*isPartial)[level]) {
                    (*partialPA)[level].first.emplace(currentPA[level]);
                    (*isPartial)[level] = false;
                } else {
                    const MPAIndexMapper flattenIndexMapper =
                            data.buffer->flattenIndexMapper.interpret<MPAIndexMapper>()[currentPA[level].start];
#ifndef NDEBUG
                    if (level >= flattenIndexMapper.generatedLevels) {
                        throw std::invalid_argument("invalid level provided");
                    }
#endif
                    auto pa = getMutableMPAFromMapper(flattenIndexMapper);
                    pa[level] = currentPA[level].build();
                }

                generationAvailable[level] = false;
            }

            if (level < levels - 1) {
                currentPASkips[level + 1] += currentPASkips[level];
                currentPA[level + 1].merge(currentPA[level]);
            }
            currentPASkips[level] = 0;
            currentPA[level].reuse(iteration);
            ++level;
        }
    }

    template<Number Num>
    void MPATable<Num>::cVerifyPA(const std::vector<itt::ref> &itCountLim, const std::vector<itt::ref> &tablePeriods,
                                  std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                                  itt::ref iteration) {
        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        itt::ref level = 0;
        const itt::ref levels = tablePeriods.size();

        while (level < levels && (currentPA[level].skip == tablePeriods[level] - MPAUtils::PERTURBATION_REQ ||
                                  itCountLim[level] != tablePeriods[level] || !generationAvailable[level])) {

            if (itCountLim[level] == tablePeriods[level] && generationAvailable[level]) {
                const MPAIndexMapper flattenIndexMapper = MPAUtils::cGetFlattenIndexMapper(data, currentPA[level].start);

                auto pa = getMutableMPAFromMapper(flattenIndexMapper);
#ifndef NDEBUG
                if (level >= flattenIndexMapper.generatedLevels) {
                    throw std::invalid_argument("invalid level provided");
                }
#endif

                pa[level] = currentPA[level].build();
                generationAvailable[level] = false;
            }

            if (level < levels - 1) {
                currentPA[level + 1].merge(currentPA[level]);
            }
            currentPA[level].reuse(iteration);
            ++level;
        }
    }


    template<Number Num>
    void MPATable<Num>::cRefreshCounter(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                        const std::vector<itt::ref> &tablePeriods,
                                        std::vector<bool> &generationAvailable,
                                        std::vector<PAGenerator<Num>> &currentPA, itt::ref iteration) {


        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        size_t level = 0;

        while (level < tablePeriods.size() - 1 && itCount[level] == itCountLim[level]) {
            itCount[level + 1] += itCount[level];
            currentPA[level + 1].merge(currentPA[level]);
            currentPA[level].reuse(iteration);
            ++level;
        }

        while (level > 0) {
            --level;
            itCountLim[level] = std::min(tablePeriods[level], itCountLim[level + 1] - itCount[level + 1]);
            itCount[level] = 0;
            generationAvailable[level] = itCountLim[level] == tablePeriods[level];
        }
    }


    template<Number Num>
    void MPATable<Num>::uRefreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                        const std::vector<uint64_t> &tablePeriods,
                                        std::vector<bool> &generationAvailable,
                                        std::vector<PAGenerator<Num>> &currentPA, std::vector<uint64_t> &currentPASkips,
                                        std::vector<bool> *isPartial, uint64_t iteration) {


        // reset current and lower level count when it reached limit
        // Amortized O(1)

        size_t level = 0;

        while (level < tablePeriods.size() - 1 && itCount[level] == itCountLim[level]) {
            itCount[level + 1] += itCount[level];
            currentPASkips[level + 1] += currentPASkips[level];
            currentPA[level + 1].merge(currentPA[level]);

            if (isPartial)
                (*isPartial)[level] = false;
            currentPASkips[level] = 0;
            currentPA[level].reuse(iteration);
            ++level;
        }

        while (level > 0) {
            --level;
            itCountLim[level] = std::min(tablePeriods[level], itCountLim[level + 1] - itCount[level + 1]);
            itCount[level] = 0;
            generationAvailable[level] = itCountLim[level] == tablePeriods[level];
        }
    }

    template<Number Num>
    void MPATable<Num>::uStepOnce(std::vector<uint64_t> &itCount, const std::vector<uint64_t> &itCountLim,
                                  const std::vector<uint64_t> &tablePeriods, std::vector<PAGenerator<Num>> &currentPA,
                                  std::vector<uint64_t> &currentPASkips, uint64_t &flattenTableIndex,
                                  uint64_t &iteration) {
        uint64_t levels = 0;
        while (levels < tablePeriods.size() && itCount[levels] == 0 && itCountLim[levels] == tablePeriods[levels]) {
            ++levels;
        }


        if (levels > 0) {
            MPAUtils::debugCheckMPAFromMapper(data.buffer->mpaTable.sizeUsed, flattenTableIndex, tablePeriods.size(), levels);
            RFF_ASSERT(data.buffer->flattenIndexMapper.sizeUsed > iteration);
            data.buffer->flattenIndexMapper.interpret<MPAIndexMapper>()[iteration] = {flattenTableIndex, levels};
            flattenTableIndex += levels;
        } else {
            RFF_ASSERT(data.buffer->flattenIndexMapper.sizeUsed > iteration);
            data.buffer->flattenIndexMapper.interpret<MPAIndexMapper>()[iteration] = {UINT64_MAX, 0};
        }
        currentPA[0].step();
        ++currentPASkips[0];
        ++itCount[0];
        ++iteration;
    }


    template<Number Num>
    void MPATable<Num>::cStepOnce(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                  const std::vector<itt::ref> &tablePeriods, std::vector<bool> &generationAvailable,
                                  std::vector<PAGenerator<Num>> &currentPA, itt::ref &pulledTableIndex,
                                  size_t &flattenTableIndex, itt::ref &iteration) {
        size_t levels = 0;
        while (levels < tablePeriods.size() && itCount[levels] == 0 && itCountLim[levels] == tablePeriods[levels]) {
            ++levels;
        }

        bool jumped = false;
        if (levels > 0) {

            const size_t compIndex = SRPCUtils::compress(data.compressor, pulledTableIndex);
            RFF_ASSERT(data.buffer->flattenIndexMapper.sizeUsed > compIndex);
            data.buffer->flattenIndexMapper.interpret<MPAIndexMapper>()[compIndex] = {.mapped = flattenTableIndex,
                                                                                    .generatedLevels = levels};

            jumped = cTryJumpTableGeneration(itCount, itCountLim, currentPA, generationAvailable, pulledTableIndex,
                                             flattenTableIndex, iteration);
        }

        if (!jumped) {
            if (levels > 0) {
                ++pulledTableIndex;
                flattenTableIndex += levels;
            }

            currentPA[0].step();
            ++itCount[0];
            ++iteration;
        }
    }

    template<Number Num>
    template<FnListeners::FnCreatingTable FnCreatingTable>
    void MPATable<Num>::generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                                      FnCreatingTable &&fnCreatingTable) {


        const auto &tablePeriods = data.info.tablePeriods;
        const itt::ref longestPeriod = tablePeriods.back();

        if (longestPeriod < data.mpaSettings.minSkipReference)
            return;

        fitBufferSize();

        // iteration starts from 1, the first value is garbage
        data.buffer->flattenIndexMapper.interpret<MPAIndexMapper>()[0] = {};

        if (data.compressor.empty()) {
            uGenerateTable(state, reference, dcMax, std::forward<FnCreatingTable>(fnCreatingTable));
        } else {
            cGenerateTable(state, reference, dcMax, std::forward<FnCreatingTable>(fnCreatingTable));
        }
    }

    template<Number Num>
    void MPATable<Num>::cInitIterationCountVec(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                               std::vector<bool> &generationAvailable) const {
        // input vector must be empty.
        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();
        itCount.resize(levels, 0);
        itCountLim = tablePeriods;
        generationAvailable.resize(levels, true);
    }


    template<Number Num>
    void MPATable<Num>::uGenerateIterationCountVec(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                                   std::vector<bool> &generationAvailable, std::vector<bool> &isPartial,
                                                   const uint64_t iteration) const {

        const auto &tablePeriods = data.info.tablePeriods;
        const size_t levels = tablePeriods.size();
        itCount.resize(levels);
        itCountLim.resize(levels);
        generationAvailable.resize(levels);
        isPartial.resize(levels);

        uint64_t remainder = iteration - 1;
        uint64_t lim = UINT64_MAX;

        for (uint64_t i = levels; i > 0; --i) {
            const uint64_t level = i - 1;
            const uint64_t p = tablePeriods[level];
            const uint64_t quotient = remainder / p;

            lim = std::min(static_cast<uint64_t>(tablePeriods[level]), lim - quotient * p);
            remainder -= quotient * p;

            itCount[level] = remainder;
            itCountLim[level] = lim;
            generationAvailable[level] = lim == p && itCount[level] < lim - MPAUtils::PERTURBATION_REQ;
            isPartial[level] = remainder > 0;
        }

        for (size_t level = levels - 1; level > 0; --level) {
            itCount[level] -= itCount[level - 1];
        }
    }

    template<Number Num>
    template<FnListeners::FnCreatingTable FnCreatingTable>
    void MPATable<Num>::cGenerateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                                       FnCreatingTable &&fnCreatingTable) {


        const auto &tablePeriods = data.info.tablePeriods;
        const itt::ref longestPeriod = tablePeriods.back();
        const size_t levels = tablePeriods.size();
        const auto epsilonPower = data.mpaSettings.epsilonPower;
        const float epsilon = std::pow(10.f, epsilonPower);
        itt::ref iteration = 1;


        std::vector<itt::ref> itCount;
        std::vector<itt::ref> itCountLim;
        std::vector<bool> generationAvailable;
        std::vector<PAGenerator<Num>> currentPA(levels, PAGenerator<Num>(reference, epsilon, dcMax, 1));

        cInitIterationCountVec(itCount, itCountLim, generationAvailable);

        itt::ref pulledTableIndex = 0;
        size_t flattenTableIndex = 0;

        while (iteration <= longestPeriod) {
            if (iteration % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return;
            }

            fnCreatingTable(static_cast<double>(iteration) / static_cast<double>(longestPeriod));

            cStepOnce(itCount, itCountLim, tablePeriods, generationAvailable, currentPA, pulledTableIndex,
                      flattenTableIndex, iteration);
            cVerifyPA(itCountLim, tablePeriods, generationAvailable, currentPA, iteration);
            cRefreshCounter(itCount, itCountLim, tablePeriods, generationAvailable, currentPA, iteration);
        }
#ifndef NDEBUG
        checkZero(state);
#endif
    }

    template<Number Num>
    void MPATable<Num>::uConfigurePartialPA(const std::vector<itt::ref> &tablePeriods,
                                            const std::vector<uint64_t> &itCountLim,
                                            std::vector<PAGenerator<Num>> &currentPA, std::vector<bool> &isPartial,
                                            PartialPA &partialPA) {
        const size_t levels = tablePeriods.size();

        for (size_t j = 1; j < levels; ++j) {
            currentPA[j].merge(currentPA[j - 1]);
        }


        for (size_t j = 0; j < levels; ++j) {
            auto &pp = partialPA[j];

            if (isPartial[j]) {
                pp.first.emplace(currentPA[j]);
            }
            if (tablePeriods[j] == itCountLim[j]) {
                pp.second.emplace(currentPA[j]);
            }
        }


        for (size_t j = 0; j < levels; ++j) {
            auto &pp = partialPA[j];
            if (pp.first.has_value() && pp.first->skip == 0)
                pp.first.reset();
            if (pp.second.has_value() && pp.second->skip == 0)
                pp.second.reset();
        }
    }

    template<Number Num>
    void MPATable<Num>::uGluePartialPA(const std::vector<itt::ref> &tablePeriod, const uint32_t threadCount,
                                       std::vector<PartialPA> &partialPAs) {

        const size_t levels = tablePeriod.size();

        for (size_t i = 0; i < levels; ++i) {

            std::optional<PAGenerator<Num>> preservingPA = std::nullopt;

            for (size_t j = 1; j < threadCount; ++j) {

                auto &referencePA = partialPAs[j - 1][i].second;

                if (referencePA.has_value()) {

                    if (!preservingPA.has_value())
                        preservingPA.emplace(*referencePA);

#ifndef NDEBUG
                    if (!partialPAs[j][i].first.has_value())
                        throw vkh::exception_invalid_state("that is a bug");
#endif
                    preservingPA->merge(*partialPAs[j][i].first);

                    if (preservingPA->skip == tablePeriod[i] - MPAUtils::PERTURBATION_REQ) {
                        PA<Num> pa = preservingPA->build();
                        const size_t flattenIndex =
                                MPAIndexMapperUtils::iterationToFlattenTableIndex(data.info, preservingPA->start) + i;
#ifndef NDEBUG
                        if (flattenIndex == UINT64_MAX ||
                            data.buffer->mpaTable.interpret<PA<Num>>()[flattenIndex].skip != 0)
                            throw vkh::exception_invalid_state("already assigned or flatten index cannot be found");
#endif
                        preservingPA.reset();
                        data.buffer->mpaTable.interpret<PA<Num>>()[flattenIndex] = pa;
                    }
                }
            }
        }
    }
    template<Number Num>
    template<FnListeners::FnCreatingTable FnCreatingTable>
    void MPATable<Num>::uGenerateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                                       FnCreatingTable &&fnCreatingTable) {

        const auto &tablePeriods = data.info.tablePeriods;
        const uint64_t longestPeriod = static_cast<uint64_t>(tablePeriods.back());
        const size_t levels = tablePeriods.size();
        const auto epsilonPower = data.mpaSettings.epsilonPower;
        const float epsilon = std::pow(10.f, epsilonPower);
        const uint32_t threadCount = generalSettings.threads;


        const size_t itInterval = std::max(static_cast<size_t>(tablePeriods[0]), longestPeriod / threadCount + 1);
        std::vector<PartialPA> partialPAs(threadCount);
        std::vector<std::unique_ptr<std::jthread>> threads(threadCount);

        for (auto &v: partialPAs) {
            v.resize(levels);
        }


        for (size_t i = 0; i < threadCount; ++i) {


            threads[i] = std::make_unique<std::jthread>([this, &reference, &state, &fnCreatingTable, dcMax, i,
                                                         itInterval, &tablePeriods, longestPeriod, epsilon, levels,
                                                         &partialPAs, threadCount] {
                const itt::ref startIteration = itInterval * i + 1;
                if (startIteration > longestPeriod || state.interruptRequested()) {
                    return;
                }

                std::vector<uint64_t> itCount;
                std::vector<uint64_t> itCountLim;
                std::vector<bool> generationAvailable;
                std::vector<bool> isPartial;
                uGenerateIterationCountVec(itCount, itCountLim, generationAvailable, isPartial, startIteration);
                std::vector<uint64_t> currentPASkips = MPAUtils::uGenerateCurrentPASkips(tablePeriods, itCount);

                uint64_t flattenTableIndex =
                        MPAIndexMapperUtils::iterationToNearestFlattenTableIndex(data.info, startIteration);
                std::vector<PAGenerator<Num>> currentPA(levels,
                                                        PAGenerator<Num>(reference, epsilon, dcMax, startIteration));

                uint64_t iteration = startIteration;


                while (iteration <= std::min(startIteration + itInterval - 1, longestPeriod)) {
                    if (iteration % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0) {
                        if (state.interruptRequested())
                            return;

                        // #ifndef NDEBUG
                        //                             fnCreatingTable(
                        //                                     std::min(longestPeriod, iteration),
                        //                                     std::min(1.0, static_cast<double>(iteration) /
                        //                                     static_cast<double>(longestPeriod)));
                        // #else
                        if (i == 0) {
                            fnCreatingTable(std::min(1.0, static_cast<double>(iteration) * threadCount /
                                                                  static_cast<double>(longestPeriod)));
                        }
                        // #endif
                    }

                    uStepOnce(itCount, itCountLim, tablePeriods, currentPA, currentPASkips, flattenTableIndex,
                              iteration);
                    uVerifyPA(itCountLim, tablePeriods, generationAvailable, currentPA, currentPASkips, &isPartial,
                              &partialPAs[i], iteration);
                    uRefreshCounter(itCount, itCountLim, tablePeriods, generationAvailable, currentPA, currentPASkips,
                                    &isPartial, iteration);
                }

                uConfigurePartialPA(tablePeriods, itCountLim, currentPA, isPartial, partialPAs[i]);
            });
            // #ifndef NDEBUG
            //                 if (threads[i]->joinable())
            //                     threads[i]->join();
            // #endif
        }

        for (const auto &thread: threads) {
            if (thread->joinable())
                thread->join();
        }

        if (state.interruptRequested())
            return;

        uGluePartialPA(tablePeriods, threadCount, partialPAs);

#ifndef NDEBUG
        checkZero(state);
#endif
    }

#ifndef NDEBUG
    template<Number Num>
    void MPATable<Num>::checkZero(const ParallelRenderState &state) {
        for (size_t i = 0; i < data.buffer->mpaTable.sizeUsed; ++i) {
            auto &pa = data.buffer->mpaTable.interpret<PA<Num>>()[i];
            if (state.interruptRequested())
                return;
            if (pa.skip == 0) {
                throw vkh::exception_invalid_state(std::format("zero skips detected at index {}", std::to_string(i)));
            }
        }
    }
#endif



    template<Number Num>
    MPAView<Num> MPATable<Num>::makeMPAView() const {
        return MPAView<Num>(data);
    }

} // namespace merutilm::rff2
