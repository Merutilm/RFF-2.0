//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>

#include "../data/ApproxTableCache.h"
#include "../mb/MB2Reference.h"
#include "../parallel/ParallelRenderState.h"
#include "ArrayCompressionTool.h"
#include "ArrayCompressor.h"
#include "MPAIndexMapper.hpp"
#include "MPAIndexMapperUtils.hpp"
#include "MPAPeriod.h"
#include "PAGenerator.h"

#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {


    template<Number Num>
    struct MPATable {

        static constexpr int PERTURBATION_REQ = 2;
        // table caches
        ApproxTableCache<Num> *tableCache = nullptr;


        const FrtGeneralSettings generalSettings;
        const FrtMPASettings mpaSettings;

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        std::vector<ArrayCompressionTool> pulledMPACompressor = std::vector<ArrayCompressionTool>();

        // important data to generate
        std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;


        explicit MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                          std::unique_ptr<ApproxTableCacheBase> &tableCache, const FrtGeneralSettings &generalSettings,
                          const FrtMPASettings &mpaSettings, Num dcMax,
                          const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);


    protected:
        [[nodiscard]] bool tryInit(const MB2Reference<Num> &reference,
                                   std::unique_ptr<ApproxTableCacheBase> &tableCache);

        [[nodiscard]] std::vector<ArrayCompressionTool>
        generatePulledMPACompressor(const std::vector<ArrayCompressionTool> &referenceCompressor) const;

        [[nodiscard]] std::span<PA<Num>> getMPAFromMapper(MPAIndexMapper flattenIndexMapper);

        [[nodiscard]] std::span<const PA<Num>> getMPAFromMapper(MPAIndexMapper flattenIndexMapper) const;

        static void debugCheckMPAFromMapper(size_t totalSize, size_t mapped, size_t levels, size_t generatedLevels);

        static uint64_t binarySearch(const std::vector<uint64_t> &arr, uint64_t key);

        void fitBufferSize();


        bool tryJumpTableGeneration(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                    std::vector<PAGenerator<Num>> &currentPA, std::vector<bool> &generationAvailable,
                                    uint64_t &pulledTableIndex, uint64_t &flattenTableIndex, uint64_t &iteration);
        void verifyPAUncompressed(
                const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
                std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                std::vector<uint64_t> &currentPASkips, std::vector<bool> &isPartial,
                std::vector<std::pair<std::optional<PAGenerator<Num>>, std::optional<PAGenerator<Num>>>> &partialPA,
                uint64_t iteration);

        void verifyPACompressed(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
                                std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                                uint64_t iteration);

        void refreshCounterCompressed(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                      const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                                      std::vector<PAGenerator<Num>> &currentPA, uint64_t iteration);
        void refreshCounterUncompressed(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                        const std::vector<uint64_t> &tablePeriod,
                                        std::vector<bool> &generationAvailable,
                                        std::vector<PAGenerator<Num>> &currentPA, std::vector<uint64_t> &currentPASkips,
                                        std::vector<bool> &isPartial, uint64_t iteration);
        void uncompressedStepOnce(std::vector<uint64_t> &itCount, const std::vector<uint64_t> &itCountLim,
                                  const std::vector<uint64_t> &tablePeriod, std::vector<PAGenerator<Num>> &currentPA,
                                  std::vector<uint64_t> &currentPASkips, uint64_t &flattenTableIndex,
                                  uint64_t &iteration);
        void compressedStepOnce(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                                std::vector<PAGenerator<Num>> &currentPA, uint64_t &pulledTableIndex,
                                uint64_t &flattenTableIndex, uint64_t &iteration);


        void generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                           const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);
        void generateIterationCountVec(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                       std::vector<bool> &generationAvailable, std::vector<bool> &isPartial,
                                       uint64_t iteration) const;
        void generateCompressedTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                                     const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);
        void generateUncompressedTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                                       const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);
#ifndef NDEBUG
        void checkZero();
#endif

        [[nodiscard]] MPAIndexMapper getFlattenIndexMapper(uint64_t iteration) const;
        [[nodiscard]] MPAIndexMapper getCompFlattenIndexMapper(uint64_t iteration) const;


    public:
        [[nodiscard]] const PA<Num> *lookup(uint64_t refIteration, complex<Num> dz) const;

        [[nodiscard]] size_t getLength() const;
    };

    // DEFINITION OF MPA TABLE


    template<Number Num>
    MPATable<Num>::MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                            std::unique_ptr<ApproxTableCacheBase> &tableCache,
                            const FrtGeneralSettings &generalSettings, const FrtMPASettings &mpaSettings, Num dcMax,
                            const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration) :
        generalSettings(generalSettings), mpaSettings(mpaSettings) {

        if (tryInit(reference, tableCache)) {
            generateTable(state, reference, dcMax, actionPerCreatingTableIteration);
        }
    }


    //[re] init mpa periods and compressors
    template<Number Num>
    bool MPATable<Num>::tryInit(const MB2Reference<Num> &reference, std::unique_ptr<ApproxTableCacheBase> &tableCache) {
        const auto &referencePeriod = reference.period;
        const uint64_t longestPeriod = reference.longestPeriod();

        if (const int minSkip = mpaSettings.minSkipReference; longestPeriod < minSkip) {
            this->mpaPeriod = nullptr;
            this->pulledMPACompressor = std::vector<ArrayCompressionTool>();
            return false;
        }

        if (!dynamic_cast<ApproxTableCache<Num> *>(tableCache.get()))
            tableCache = std::make_unique<ApproxTableCache<Num>>();
        this->tableCache = static_cast<ApproxTableCache<Num> *>(tableCache.get());

        this->mpaPeriod = MPAPeriod::generate(referencePeriod, mpaSettings);
        this->pulledMPACompressor = mpaSettings.useCompress ? generatePulledMPACompressor(reference.compressor)
                                                            : std::vector<ArrayCompressionTool>();
        return true;
    }

    template<Number Num>
    std::vector<ArrayCompressionTool>
    MPATable<Num>::generatePulledMPACompressor(const std::vector<ArrayCompressionTool> &referenceCompressor) const {
        std::vector<ArrayCompressionTool> mpaTools;
        auto &tablePeriod = mpaPeriod->tablePeriods;
        auto &skippableIterationCounts = mpaPeriod->skippableIterationCounts;
        auto &isArtificial = mpaPeriod->isArtificial;

        for (ArrayCompressionTool tool: referenceCompressor) {
            const uint64_t start = tool.start;
            const uint64_t length = tool.range();
            const uint64_t index = binarySearch(tablePeriod, length + 1);

            // Check if the reference compressor is same as period.
            // However, The Computer doesn't know whether the compressor's length came from skipping to the periodic
            // point, or being cut off in the middle. So, Do check tableIndex too.

            if (const auto [pulledIndex, _] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(*mpaPeriod, start);
                index != UINT64_MAX && pulledIndex != UINT64_MAX && !isArtificial[index]) {
                const uint64_t skippableIterationCount = skippableIterationCounts[index];
                mpaTools.emplace_back(1, pulledIndex + 1, pulledIndex + skippableIterationCount - 1);
            }
        }
        return mpaTools;
    }

    template<Number Num>
    uint64_t MPATable<Num>::binarySearch(const std::vector<uint64_t> &arr, const uint64_t key) {
        if (arr.empty()) {
            return UINT64_MAX;
        }

        uint64_t low = 0;
        uint64_t high = arr.size() - 1;

        while (low <= high) {
            const uint64_t mid = (low + high) >> 1;
            if (const uint64_t value = arr[mid]; value < key) {
                low = mid + 1;
            } else if (value > key) {
                if (mid == 0) {
                    return UINT64_MAX;
                }
                high = mid - 1;
            } else
                return mid;
        }
        return UINT64_MAX;
    }


    template<Number Num>
    void MPATable<Num>::fitBufferSize() {


        const uint64_t longestPeriod = mpaPeriod->tablePeriods.back();

        uint64_t tableLen = mpaPeriod->tableElementCounts.back();
        uint64_t mapperLen = 0;

        if (mpaSettings.useCompress) {
            const auto pulledTableIndex =
                    MPAIndexMapperUtils::iterationToPulledTableIndex(*mpaPeriod, longestPeriod + 1);
            const auto compressedIndex = ArrayCompressor::compress(pulledMPACompressor, pulledTableIndex);
            mapperLen = compressedIndex;

            for (const auto &compressor: pulledMPACompressor) {
                const uint64_t i = binarySearch(mpaPeriod->skippableIterationCounts, compressor.range() + 1);
                assert(i != UINT64_MAX);
                tableLen -= mpaPeriod->tableElementCounts[i] - (i + 1);
            }

        } else {
            mapperLen = longestPeriod + 1;
        }

        tableCache->resize(tableLen, mapperLen);
    }

    template<Number Num>
    bool MPATable<Num>::tryJumpTableGeneration(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                               std::vector<PAGenerator<Num>> &currentPA,
                                               std::vector<bool> &generationAvailable, uint64_t &pulledTableIndex,
                                               uint64_t &flattenTableIndex, uint64_t &iteration) {

        if (pulledMPACompressor.empty())
            return false;

        const ArrayCompressionTool *containedTool = ArrayCompressor::find(pulledMPACompressor, pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != pulledTableIndex + 1) {
            return false;
        }
        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const uint64_t levels = tablePeriod.size();
        const auto &skippableIterationsCount = mpaPeriod->skippableIterationCounts;
        const uint64_t level = binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const auto generatedLevels = MPAIndexMapperUtils::iterationToGeneratedLevels(*mpaPeriod, iteration);
        const auto mainReferenceMPA = getMPAFromMapper({0, generatedLevels});

        if (level >= mainReferenceMPA.size() || level + 1 > generatedLevels) {
            throw std::logic_error("Invalid level detected. it might be a bug! Please contact the developer and "
                                   "attach the current location file (.rfl)");
        }

        const PA<Num> &mainReferencePA = mainReferenceMPA[level];
        const uint64_t skip = mainReferencePA.skip;

        auto mpa = getMPAFromMapper({flattenTableIndex, generatedLevels});


        for (uint64_t i = level + 1; i < levels; ++i) {
            if (i <= level && itCount[i] != 0) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} is not zero.",
                                 itCount[i]);
                return false;
            }
            if (itCount[i] + skip > tablePeriod[i] - PERTURBATION_REQ) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} + "
                                 "skip {} exceeds its period {}.",
                                 itCount[i], skip, tablePeriod[i]);
                return false;
            }
        }

        pulledTableIndex += skippableIterationsCount[level];
        iteration += skip;
        flattenTableIndex += generatedLevels;

        for (uint64_t i = 0; i < level + 1; ++i) {
            mpa[i] = mainReferenceMPA[i];
            currentPA[i].reuse(iteration);
            itCount[i] = 0;
            itCountLim[i] = PERTURBATION_REQ;
            generationAvailable[i] = false;
        }

        if (level + 1 < levels) {
            itCount[level + 1] += skip;
            currentPA[level + 1].merge(mainReferencePA);
        }
        return true;
    }

    template<Number Num>
    void MPATable<Num>::verifyPAUncompressed(
            const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
            std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
            std::vector<uint64_t> &currentPASkips, std::vector<bool> &isPartial,
            std::vector<std::pair<std::optional<PAGenerator<Num>>, std::optional<PAGenerator<Num>>>> &partialPA,
            uint64_t iteration) {

        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        uint64_t level = 0;
        const uint64_t levels = tablePeriod.size();

#ifndef NDEBUG
        if (currentPASkips[level] > tablePeriod[level] - PERTURBATION_REQ) {
            throw std::logic_error("skip count is exceeded");
        }
#endif

        while (level < levels && (currentPASkips[level] == tablePeriod[level] - PERTURBATION_REQ ||
                                  itCountLim[level] != tablePeriod[level] || !generationAvailable[level])) {

            if (itCountLim[level] == tablePeriod[level] && generationAvailable[level]) {


                if (isPartial[level]) {
                    partialPA[level].first.emplace(currentPA[level]);
                    isPartial[level] = false;
                } else {
                    const MPAIndexMapper flattenIndexMapper = tableCache->flattenIndexMapper[currentPA[level].start];
#ifndef NDEBUG
                    if (level >= flattenIndexMapper.generatedLevels) {
                        throw std::invalid_argument("invalid level provided");
                    }
#endif
                    auto pa = getMPAFromMapper(flattenIndexMapper);
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
    void MPATable<Num>::verifyPACompressed(const std::vector<uint64_t> &itCountLim,
                                           const std::vector<uint64_t> &tablePeriod,
                                           std::vector<bool> &generationAvailable,
                                           std::vector<PAGenerator<Num>> &currentPA, uint64_t iteration) {
        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        uint64_t level = 0;
        const uint64_t levels = tablePeriod.size();

        while (level < levels && (currentPA[level].skip == tablePeriod[level] - PERTURBATION_REQ ||
                                  itCountLim[level] != tablePeriod[level] || !generationAvailable[level])) {

            if (itCountLim[level] == tablePeriod[level] && generationAvailable[level]) {
                const MPAIndexMapper flattenIndexMapper = getCompFlattenIndexMapper(currentPA[level].start);


                auto pa = getMPAFromMapper(flattenIndexMapper);
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
    std::span<PA<Num>> MPATable<Num>::getMPAFromMapper(const MPAIndexMapper flattenIndexMapper) {
        const size_t levels = mpaPeriod->tablePeriods.size();
        size_t size = flattenIndexMapper.generatedLevels;
        debugCheckMPAFromMapper(tableCache->tableSizeUsed, flattenIndexMapper.mapped, levels, size);
#ifndef NDEBUG
        PA<Num> *start = tableCache->mpaTable.data() + flattenIndexMapper.mapped;
#else
        PA<Num> *start = tableCache->mpaTable + flattenIndexMapper.mapped;
#endif
        return std::span<PA<Num>>(start, size);
    }

    template<Number Num>
    std::span<const PA<Num>> MPATable<Num>::getMPAFromMapper(const MPAIndexMapper flattenIndexMapper) const {

        const size_t levels = mpaPeriod->tablePeriods.size();
        size_t size = flattenIndexMapper.generatedLevels;
        debugCheckMPAFromMapper(tableCache->tableSizeUsed, flattenIndexMapper.mapped, levels, size);
#ifndef NDEBUG
        const PA<Num> *start = tableCache->mpaTable.data() + flattenIndexMapper.mapped;
#else
        PA<Num> *start = tableCache->mpaTable + flattenIndexMapper.mapped;
#endif

        return std::span<const PA<Num>>(start, size);
    }

    template<Number Num>
    void MPATable<Num>::debugCheckMPAFromMapper(const size_t totalSize, const size_t mapped, const size_t levels,
                                                const size_t generatedLevels) {
#ifndef NDEBUG
        if (levels == 0) {
            throw std::invalid_argument("levels is zero");
        }
        if (totalSize < mapped + generatedLevels) {
            throw std::invalid_argument("generatedLevels out of range");
        }
        if (levels < generatedLevels) {
            throw std::invalid_argument("levels out of range");
        }
#endif
    }

    template<Number Num>
    void MPATable<Num>::refreshCounterCompressed(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                                 const std::vector<uint64_t> &tablePeriod,
                                                 std::vector<bool> &generationAvailable,
                                                 std::vector<PAGenerator<Num>> &currentPA, uint64_t iteration) {


        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        uint64_t level = 0;

        while (level < tablePeriod.size() - 1 && itCount[level] == itCountLim[level]) {
            itCount[level + 1] += itCount[level];
            currentPA[level + 1].merge(currentPA[level]);
            currentPA[level].reuse(iteration);
            ++level;
        }

        while (level > 0) {
            --level;
            itCountLim[level] = std::min(tablePeriod[level], itCountLim[level + 1] - itCount[level + 1]);
            itCount[level] = 0;
            generationAvailable[level] = itCountLim[level] == tablePeriod[level];
        }
    }


    template<Number Num>
    void MPATable<Num>::refreshCounterUncompressed(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                                   const std::vector<uint64_t> &tablePeriod,
                                                   std::vector<bool> &generationAvailable,
                                                   std::vector<PAGenerator<Num>> &currentPA,
                                                   std::vector<uint64_t> &currentPASkips, std::vector<bool> &isPartial,
                                                   uint64_t iteration) {


        // reset current and lower level count when it reached limit
        // Amortized O(1)

        uint64_t level = 0;

        while (level < tablePeriod.size() - 1 && itCount[level] == itCountLim[level]) {
            itCount[level + 1] += itCount[level];
            currentPASkips[level + 1] += currentPASkips[level];
            currentPA[level + 1].merge(currentPA[level]);

            isPartial[level] = false;
            currentPASkips[level] = 0;
            currentPA[level].reuse(iteration);
            ++level;
        }

        while (level > 0) {
            --level;
            itCountLim[level] = std::min(tablePeriod[level], itCountLim[level + 1] - itCount[level + 1]);
            itCount[level] = 0;
            generationAvailable[level] = itCountLim[level] == tablePeriod[level];
        }
    }

    template<Number Num>
    void MPATable<Num>::uncompressedStepOnce(std::vector<uint64_t> &itCount, const std::vector<uint64_t> &itCountLim,
                                             const std::vector<uint64_t> &tablePeriod,
                                             std::vector<PAGenerator<Num>> &currentPA,
                                             std::vector<uint64_t> &currentPASkips, uint64_t &flattenTableIndex,
                                             uint64_t &iteration) {
        uint64_t levels = 0;
        while (levels < tablePeriod.size() && itCount[levels] == 0 && itCountLim[levels] == tablePeriod[levels]) {
            ++levels;
        }


        if (levels > 0) {
            debugCheckMPAFromMapper(tableCache->tableSizeUsed, flattenTableIndex, tablePeriod.size(), levels);
            assert(tableCache->flattenIndexMapper.size() > iteration);
            tableCache->flattenIndexMapper[iteration] = {flattenTableIndex, levels};
            flattenTableIndex += levels;
        } else {
            assert(tableCache->flattenIndexMapper.size() > iteration);
            tableCache->flattenIndexMapper[iteration] = {UINT64_MAX, 0};
        }
        currentPA[0].step();
        ++currentPASkips[0];
        ++itCount[0];
        ++iteration;
    }


    template<Number Num>
    void MPATable<Num>::compressedStepOnce(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                           const std::vector<uint64_t> &tablePeriod,
                                           std::vector<bool> &generationAvailable,
                                           std::vector<PAGenerator<Num>> &currentPA, uint64_t &pulledTableIndex,
                                           uint64_t &flattenTableIndex, uint64_t &iteration) {
        uint64_t levels = 0;
        while (levels < tablePeriod.size() && itCount[levels] == 0 && itCountLim[levels] == tablePeriod[levels]) {
            ++levels;
        }

        bool jumped = false;
        if (levels > 0) {

            uint64_t compIndex = ArrayCompressor::compress(pulledMPACompressor, pulledTableIndex);
            assert(tableCache->flattenIndexMapper.size() > compIndex);
            tableCache->flattenIndexMapper[compIndex] = {flattenTableIndex, levels};

            jumped = tryJumpTableGeneration(itCount, itCountLim, currentPA, generationAvailable, pulledTableIndex,
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
    void MPATable<Num>::generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                                      const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration) {


        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const uint64_t longestPeriod = tablePeriod.back();

        if (longestPeriod < mpaSettings.minSkipReference)
            return;

        fitBufferSize();
        if (mpaSettings.useCompress) {
            generateCompressedTable(state, reference, dcMax, actionPerCreatingTableIteration);
        } else {
            generateUncompressedTable(state, reference, dcMax, actionPerCreatingTableIteration);
        }
    }


    template<Number Num>
    void MPATable<Num>::generateIterationCountVec(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                                  std::vector<bool> &generationAvailable, std::vector<bool> &isPartial,
                                                  const uint64_t iteration) const {

        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const size_t levels = tablePeriod.size();
        itCount.resize(levels, 0);
        itCountLim.resize(levels);
        generationAvailable.resize(levels);
        isPartial.resize(levels);

        uint64_t remainder = iteration - 1;
        uint64_t lim = UINT64_MAX;

        for (uint64_t i = levels; i > 0; --i) {
            const uint64_t level = i - 1;
            const uint64_t p = tablePeriod[level];
            const uint64_t quotient = remainder / p;

            lim = std::min(tablePeriod[level], lim - quotient * p);
            remainder -= quotient * p;

            itCount[level] = remainder;
            itCountLim[level] = lim;
            generationAvailable[level] = lim == p && itCount[level] < lim - PERTURBATION_REQ;
            isPartial[level] = remainder > 0;
        }

        for (uint64_t level = levels - 1; level > 0; --level) {
            itCount[level] -= itCount[level - 1];
        }
    }

    template<Number Num>
    void MPATable<Num>::generateCompressedTable(
            const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
            const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration) {


        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const uint64_t longestPeriod = tablePeriod.back();
        const size_t levels = tablePeriod.size();
        const auto epsilonPower = mpaSettings.epsilonPower;
        const double epsilon = pow(10, epsilonPower);
        uint64_t iteration = 1;


        std::vector<uint64_t> itCount;
        std::vector<uint64_t> itCountLim;
        std::vector<bool> generationAvailable;
        std::vector<bool> isPartial;
        std::vector<PAGenerator<Num>> currentPA(levels, PAGenerator<Num>(reference, epsilon, dcMax, 1));

        generateIterationCountVec(itCount, itCountLim, generationAvailable, isPartial, 1);

        uint64_t pulledTableIndex = 0;
        uint64_t flattenTableIndex = 0;

        while (iteration <= longestPeriod) {
            if (iteration % Constants::Fractal::PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL == 0 &&
                state.interruptRequested())
                return;

            actionPerCreatingTableIteration(iteration,
                                            static_cast<double>(iteration) / static_cast<double>(longestPeriod));

            compressedStepOnce(itCount, itCountLim, tablePeriod, generationAvailable, currentPA, pulledTableIndex,
                               flattenTableIndex, iteration);
            verifyPACompressed(itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
            refreshCounterCompressed(itCount, itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
        }
#ifndef NDEBUG
        checkZero();
#endif
    }


    template<Number Num>
    void MPATable<Num>::generateUncompressedTable(
            const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
            const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration) {

        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const uint64_t longestPeriod = tablePeriod.back();
        const size_t levels = tablePeriod.size();
        const auto epsilonPower = mpaSettings.epsilonPower;
        const double epsilon = pow(10, epsilonPower);
        const uint32_t threadCount = generalSettings.threads;
        const uint64_t itInterval = std::max(tablePeriod[0], longestPeriod / threadCount + 1);
        std::vector<std::vector<std::pair<std::optional<PAGenerator<Num>>, std::optional<PAGenerator<Num>>>>>
                partialPAs(threadCount);
        if (mpaSettings.useParallelization) {
            std::vector<std::unique_ptr<std::jthread>> threads(threadCount);

            for (auto &v: partialPAs) {
                v.resize(levels);
            }


            for (uint64_t i = 0; i < threadCount; ++i) {


                threads[i] = std::make_unique<std::jthread>([this, &reference, &state, &actionPerCreatingTableIteration,
                                                             dcMax, i, itInterval, &tablePeriod, longestPeriod, epsilon,
                                                             levels, &partialPAs, threadCount] {
                    const uint64_t startIteration = itInterval * i + 1;
                    if (startIteration > longestPeriod)
                        return;

                    std::vector<uint64_t> itCount;
                    std::vector<uint64_t> itCountLim;
                    std::vector<bool> generationAvailable;
                    std::vector<bool> isPartial;
                    generateIterationCountVec(itCount, itCountLim, generationAvailable, isPartial, startIteration);

                    std::vector<uint64_t> currentPASkips = itCount;
                    for (uint64_t j = 0; j < levels - 1; ++j) {
                        if (currentPASkips[j] >= tablePeriod[j] - PERTURBATION_REQ) {
                            currentPASkips[j + 1] += currentPASkips[j];
                            currentPASkips[j] = 0;
                        }
                    }

                    MPAIndexMapper mapper = MPAIndexMapperUtils::invalidValueSentinel<
                            MPAIndexMapperUtils::IndexMappingMode::FLATTEN_LEVELS, MPAIndexMapper>();

                    uint64_t startIterationDecrement = 0;
                    for (; mapper.mapped == UINT64_MAX; ++startIterationDecrement) {
                        mapper = MPAIndexMapperUtils::iterationToFlattenTableIndexMapper(
                                *mpaPeriod, startIteration - startIterationDecrement);
                        // TODO can this be optimized?
                        // Even though the number of loops is less than 2 * MinSkipReference, optimization may still be
                        // possible.
                    }

                    uint64_t flattenTableIndex = itCount[0] > 0 || startIterationDecrement > 1
                                                         ? mapper.mapped + mapper.generatedLevels
                                                         : mapper.mapped;


                    std::vector<PAGenerator<Num>> currentPA(
                            levels, PAGenerator<Num>(reference, epsilon, dcMax, startIteration));


                    uint64_t iteration = startIteration;


                    while (iteration <= std::min(startIteration + itInterval - 1, longestPeriod)) {
                        if (iteration % Constants::Fractal::PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL == 0) {
                            if (state.interruptRequested())
                                return;
                            if (i == 0) {
                                actionPerCreatingTableIteration(
                                        std::min(longestPeriod, iteration * threadCount),
                                        std::min(1.0, static_cast<double>(iteration) * threadCount /
                                                              static_cast<double>(longestPeriod)));
                            }
                        }

                        uncompressedStepOnce(itCount, itCountLim, tablePeriod, currentPA, currentPASkips,
                                             flattenTableIndex, iteration);
                        verifyPAUncompressed(itCountLim, tablePeriod, generationAvailable, currentPA, currentPASkips,
                                             isPartial, partialPAs[i], iteration);
                        refreshCounterUncompressed(itCount, itCountLim, tablePeriod, generationAvailable, currentPA,
                                                   currentPASkips, isPartial, iteration);
                    }


                    for (uint64_t j = 1; j < levels; ++j) {
                        currentPA[j].merge(currentPA[j - 1]);
                    }


                    for (uint64_t j = 0; j < levels; ++j) {
                        auto &pp = partialPAs[i][j];

                        if (isPartial[j]) {
                            pp.first.emplace(currentPA[j]);
                        }
                        if (tablePeriod[j] == itCountLim[j]) {
                            pp.second.emplace(currentPA[j]);
                        }
                    }


                    for (uint64_t j = 0; j < levels; ++j) {
                        auto &pp = partialPAs[i][j];
                        if (pp.first.has_value() && pp.first->skip == 0)
                            pp.first.reset();
                        if (pp.second.has_value() && pp.second->skip == 0)
                            pp.second.reset();
                    }
                });
#ifndef NDEBUG
                if (threads[i]->joinable())
                    threads[i]->join();
#endif
            }

            for (const auto &thread: threads) {
                if (thread->joinable())
                    thread->join();
            }

            if (state.interruptRequested())
                return;

            for (uint64_t i = 0; i < levels; ++i) {

                std::optional<PAGenerator<Num>> preservingPA = std::nullopt;

                for (uint64_t j = 1; j < threadCount; ++j) {

                    auto &referencePA = partialPAs[j - 1][i].second;

                    if (referencePA.has_value()) {

                        if (!preservingPA.has_value())
                            preservingPA.emplace(*referencePA);

#ifndef NDEBUG
                        if (!partialPAs[j][i].first.has_value())
                            throw std::logic_error("that is a bug");
#endif
                        preservingPA->merge(*partialPAs[j][i].first);

                        if (preservingPA->skip == tablePeriod[i] - PERTURBATION_REQ) {
                            PA<Num> pa = preservingPA->build();
                            const uint64_t flattenIndex =
                                    MPAIndexMapperUtils::iterationToFlattenTableIndex(*mpaPeriod, preservingPA->start) +
                                    i;
#ifndef NDEBUG
                            if (flattenIndex == UINT64_MAX || tableCache->mpaTable[flattenIndex].skip != 0)
                                throw std::logic_error("already assigned or flatten index cannot be found");
#endif
                            preservingPA.reset();
                            tableCache->mpaTable[flattenIndex] = pa;
                        }
                    }
                }
            }
        } else {
            uint64_t flattenTableIndex = 0;
            std::vector<uint64_t> itCount;
            std::vector<uint64_t> itCountLim;
            std::vector<bool> generationAvailable;
            std::vector<bool> isPartial;
            uint64_t iteration = 1;
            generateIterationCountVec(itCount, itCountLim, generationAvailable, isPartial, 1);
            std::vector<PAGenerator<Num>> currentPA(levels, PAGenerator<Num>(reference, epsilon, dcMax, 1));
            std::vector<uint64_t> currentPASkips(levels, 0);
            std::vector<std::pair<std::optional<PAGenerator<Num>>, std::optional<PAGenerator<Num>>>> partialPA;

            while (iteration <= longestPeriod) {

                if (iteration % Constants::Fractal::PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL == 0) {
                    if (state.interruptRequested()) {
                        return;
                    }
                    actionPerCreatingTableIteration(iteration, static_cast<double>(iteration) /
                                                                       static_cast<double>(longestPeriod));
                }


                uncompressedStepOnce(itCount, itCountLim, tablePeriod, currentPA, currentPASkips, flattenTableIndex,
                                     iteration);
                verifyPAUncompressed(itCountLim, tablePeriod, generationAvailable, currentPA, currentPASkips, isPartial,
                                     partialPA, iteration);
                refreshCounterUncompressed(itCount, itCountLim, tablePeriod, generationAvailable, currentPA,
                                           currentPASkips, isPartial, iteration);
            }
        }

#ifndef NDEBUG
        checkZero();
#endif
    }

#ifndef NDEBUG
    template<Number Num>
    void MPATable<Num>::checkZero() {
        for (size_t i = 0; i < tableCache->mpaTable.size(); ++i) {
            auto &pa = tableCache->mpaTable[i];
            if (pa.skip == 0) {
                throw std::logic_error("zero skips detected at index " + std::to_string(i));
            }
        }
    }
#endif


    template<Number Num>
    MPAIndexMapper MPATable<Num>::getCompFlattenIndexMapper(const uint64_t iteration) const {
        const auto [pulled, levels] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(*mpaPeriod, iteration);
        if (pulled == UINT64_MAX) {
            return MPAIndexMapper{UINT64_MAX, 0};
        }

        const uint64_t comp = ArrayCompressor::compress(pulledMPACompressor, pulled);
        return MPAIndexMapper{tableCache->flattenIndexMapper[comp].mapped, levels};
    }

    template<Number Num>
    MPAIndexMapper MPATable<Num>::getFlattenIndexMapper(const uint64_t iteration) const {
        if (mpaSettings.useCompress) {
            return getCompFlattenIndexMapper(iteration);
        }
        return tableCache->flattenIndexMapper[iteration];
    }


    template<Number Num>
    const PA<Num> *MPATable<Num>::lookup(const uint64_t refIteration, const complex<Num> dz) const {

        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }

        const MPAIndexMapper mapper = getFlattenIndexMapper(refIteration);

        if (mapper.mapped == UINT64_MAX) {
            return nullptr;
        }


        debugCheckMPAFromMapper(tableCache->tableSizeUsed, mapper.mapped, mpaPeriod->tablePeriods.size(),
                                mapper.generatedLevels);

        const auto table = getMPAFromMapper(mapper);
        const Num r = dz.norm_approx();

        switch (mpaSettings.mpaSelectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                const PA<Num> *pa = nullptr;

                for (const PA<Num> &test: table) {

                    if (test.isValid(r)) {
                        pa = &test;
                    } else
                        return pa;
                }
                return pa;
            }
            case HIGHEST: {
                const PA<Num> &pa = table.front();
                // This table cannot be empty because the pre-processing is done.

                if (!pa.isValid(r)) {
                    return nullptr;
                }

                for (uint64_t j = table.size(); j > 0; --j) {
                    const PA<Num> &test = table[j - 1];

                    if (test.isValid(r)) {
                        return &test;
                    }
                }

                return &pa;
            }
            default:
                return nullptr;
        }
    }

    template<Number Num>
    size_t MPATable<Num>::getLength() const {
        return tableCache ? tableCache->tableSizeUsed : 0;
    }

    using LightMPATable = MPATable<double>;
    using DeepMPATable = MPATable<dex>;
} // namespace merutilm::rff2
