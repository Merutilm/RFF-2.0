//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>

#include "../data/ApproxTableCache.h"
#include "../mb/MB2Reference.h"
#include "../parallel/ParallelRenderState.h"
#include "MPAIndexMapper.hpp"
#include "MPAIndexMapperUtils.hpp"
#include "MPAPeriod.hpp"
#include "PAGenerator.h"
#include "ReferenceCompressUnit.hpp"
#include "ReferenceCompressor.hpp"

#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {

    // MP-Approximation table.
    // prefix c: For compressed table.
    // prefix u: For uncompressed table. Arithmetic overflowing is not permitted.
    template<Number Num>
    struct MPATable {

        using PartialPA = std::vector<std::pair<std::optional<PAGenerator<Num>>, std::optional<PAGenerator<Num>>>>;

        static constexpr int PERTURBATION_REQ = 2;
        // table caches
        ApproxTableCache &tableCache;


        const FrtGeneralSettings generalSettings;
        const FrtMPASettings mpaSettings;
        const bool computeShaderUsed;

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        std::vector<ReferenceCompressUnit> pulledMPACompressor = std::vector<ReferenceCompressUnit>();

        // important data to generate
        std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;

        template<FnListeners::FnCreatingTable FnCreatingTable>
        explicit MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                          ApproxTableCache &tableCache, const FrtGeneralSettings &generalSettings,
                          const FrtMPASettings &mpaSettings, bool computeShaderUsed, Num dcMax,
                          FnCreatingTable &&fnCreatingTable);


    private:
        [[nodiscard]] bool tryInit(const MB2Reference<Num> &reference);

        [[nodiscard]] std::vector<ReferenceCompressUnit>
        generatePulledMPACompressor(const std::vector<ReferenceCompressUnit> &referenceCompressor) const;

        [[nodiscard]] std::span<PA<Num>> getMPAFromMapper(MPAIndexMapper flattenIndexMapper);

        [[nodiscard]] std::span<const PA<Num>> getMPAFromMapper(MPAIndexMapper flattenIndexMapper) const;

        [[nodiscard]] static size_t binarySearch(const std::vector<itt::ref> &arr, itt::ref key);

        [[nodiscard]] static std::vector<uint64_t> generateUncompressedCurrentPASkips(const std::vector<itt::ref> &tablePeriod,
                                                                          const std::vector<uint64_t> &itCount);

        static void debugCheckMPAFromMapper(size_t totalSize, size_t mapped, size_t levels, size_t generatedLevels);

        void fitBufferSize() const;


        bool cTryJumpTableGeneration(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                    std::vector<PAGenerator<Num>> &currentPA, std::vector<bool> &generationAvailable,
                                    itt::ref &pulledTableIndex, size_t &flattenTableIndex, itt::ref &iteration);

        void uVerifyPA(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
                                  std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                                  std::vector<uint64_t> &currentPASkips, std::vector<bool> *isPartial,
                                  PartialPA *partialPA, uint64_t iteration);

        void cVerifyPA(const std::vector<itt::ref> &itCountLim, const std::vector<itt::ref> &tablePeriod,
                                std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                                itt::ref iteration);

        void cRefreshCounter(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                      const std::vector<itt::ref> &tablePeriod, std::vector<bool> &generationAvailable,
                                      std::vector<PAGenerator<Num>> &currentPA, itt::ref iteration);

        void uRefreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                        const std::vector<uint64_t> &tablePeriod,
                                        std::vector<bool> &generationAvailable,
                                        std::vector<PAGenerator<Num>> &currentPA, std::vector<uint64_t> &currentPASkips,
                                        std::vector<bool> *isPartial, uint64_t iteration);

        void uStepOnce(std::vector<uint64_t> &itCount, const std::vector<uint64_t> &itCountLim,
                                  const std::vector<uint64_t> &tablePeriod, std::vector<PAGenerator<Num>> &currentPA,
                                  std::vector<uint64_t> &currentPASkips, uint64_t &flattenTableIndex,
                                  uint64_t &iteration);

        void cStepOnce(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                const std::vector<itt::ref> &tablePeriod, std::vector<bool> &generationAvailable,
                                std::vector<PAGenerator<Num>> &currentPA, itt::ref &pulledTableIndex,
                                size_t &flattenTableIndex, itt::ref &iteration);

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


        void uConfigurePartialPA(const std::vector<itt::ref> &tablePeriod, const std::vector<uint64_t> &itCountLim,
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

        [[nodiscard]] MPAIndexMapper getFlattenIndexMapper(itt::ref iteration) const;
        [[nodiscard]] MPAIndexMapper getCompFlattenIndexMapper(itt::ref iteration) const;


    public:
        [[nodiscard]] const PA<Num> *lookup(itt::ref refIteration, const complex<Num> &dz) const noexcept;

        void lookupBatch(std::array<PA<Num>, RFF_SIMD_CHUNK_SIZE> &results,
                         const std::array<itt::ref, RFF_SIMD_CHUNK_SIZE> &refIterations,
                         const std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> &dzs,
                         const std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &finishes) const noexcept;
    };

    // DEFINITION OF MPA TABLE


    template<Number Num>
    template<FnListeners::FnCreatingTable FnCreatingTable>
    MPATable<Num>::MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                            ApproxTableCache &tableCache, const FrtGeneralSettings &generalSettings,
                            const FrtMPASettings &mpaSettings, const bool computeShaderUsed, Num dcMax,
                            FnCreatingTable &&fnCreatingTable) :
        tableCache(tableCache), generalSettings(generalSettings), mpaSettings(mpaSettings),
        computeShaderUsed(computeShaderUsed) {

        if (tryInit(reference)) {
            generateTable(state, reference, dcMax, fnCreatingTable);
        }
    }


    //[re] init mpa periods and compressors
    template<Number Num>
    bool MPATable<Num>::tryInit(const MB2Reference<Num> &reference) {
        const auto &referencePeriod = reference.period;
        const itt::ref longestPeriod = reference.longestPeriod();

        if (const int minSkip = mpaSettings.minSkipReference; longestPeriod < minSkip) {
            this->mpaPeriod = nullptr;
            this->pulledMPACompressor = std::vector<ReferenceCompressUnit>();
            return false;
        }

        this->mpaPeriod = MPAPeriod::generate(referencePeriod, mpaSettings);
        this->pulledMPACompressor = mpaSettings.useCompress ? generatePulledMPACompressor(reference.compressor)
                                                            : std::vector<ReferenceCompressUnit>();
        return true;
    }

    template<Number Num>
    std::vector<ReferenceCompressUnit>
    MPATable<Num>::generatePulledMPACompressor(const std::vector<ReferenceCompressUnit> &referenceCompressor) const {
        std::vector<ReferenceCompressUnit> mpaTools;
        auto &tablePeriod = mpaPeriod->tablePeriods;
        auto &skippableIterationCounts = mpaPeriod->skippableIterationCounts;
        auto &isArtificial = mpaPeriod->isArtificial;

        for (ReferenceCompressUnit tool: referenceCompressor) {
            const itt::ref start = tool.start;
            const itt::ref length = tool.range();
            const size_t index = binarySearch(tablePeriod, length + 1);

            // Check if the reference compressor is same as period.
            // However, The Computer doesn't know whether the compressor's length came from skipping to the periodic
            // point, or being cut off in the middle. So, Do check tableIndex too.

            if (const auto [pulledIndex, _] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(*mpaPeriod, start);
                index != UINT64_MAX && pulledIndex != UINT64_MAX && !isArtificial[index]) {
                const itt::ref skippableIterationCount = skippableIterationCounts[index];
                mpaTools.emplace_back(1, pulledIndex + 1, pulledIndex + skippableIterationCount - 1);
            }
        }
        return mpaTools;
    }

    template<Number Num>
    size_t MPATable<Num>::binarySearch(const std::vector<itt::ref> &arr, const itt::ref key) {
        if (arr.empty()) {
            return UINT64_MAX;
        }

        size_t low = 0;
        size_t high = arr.size() - 1;

        while (low <= high) {
            const size_t mid = (low + high) >> 1;
            if (const itt::ref value = arr[mid]; value < key) {
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
    void MPATable<Num>::fitBufferSize() const {


        const itt::ref longestPeriod = mpaPeriod->tablePeriods.back();

        itt::ref tableLen = mpaPeriod->tableElementCounts.back();
        itt::ref mapperLen = 0;

        if (mpaSettings.useCompress) {
            const auto pulledTableIndex =
                    MPAIndexMapperUtils::iterationToPulledTableIndex(*mpaPeriod, longestPeriod + 1);
            const auto compressedIndex = ReferenceCompressor::compress(pulledMPACompressor, pulledTableIndex);
            mapperLen = compressedIndex;

            for (const auto &compressor: pulledMPACompressor) {
                const itt::ref i = binarySearch(mpaPeriod->skippableIterationCounts, compressor.range() + 1);
                RFF_ASSERT(i != UINT64_MAX);
                tableLen -= mpaPeriod->tableElementCounts[i] - (i + 1);
            }

        } else {
            mapperLen = longestPeriod + 1;
        }

        if (static_cast<itt::ref>(static_cast<size_t>(tableLen)) != tableLen) {
            throw vkh::exception_invalid_state("table buffer overflow");
        }

        if (static_cast<itt::ref>(static_cast<size_t>(mapperLen)) != mapperLen) {
            throw vkh::exception_invalid_state("mapper buffer overflow");
        }

        tableCache.resize<PA<Num>>(static_cast<size_t>(tableLen), static_cast<size_t>(mapperLen), computeShaderUsed);
    }

    template<Number Num>
    bool MPATable<Num>::cTryJumpTableGeneration(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                               std::vector<PAGenerator<Num>> &currentPA,
                                               std::vector<bool> &generationAvailable, itt::ref &pulledTableIndex,
                                               size_t &flattenTableIndex, itt::ref &iteration) {

        if (pulledMPACompressor.empty())
            return false;

        const ReferenceCompressUnit *containedTool =
                ReferenceCompressor::find(pulledMPACompressor, pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != pulledTableIndex + 1) {
            return false;
        }
        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const size_t levels = tablePeriod.size();
        const auto &skippableIterationsCount = mpaPeriod->skippableIterationCounts;
        const size_t level = binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const size_t generatedLevels = MPAIndexMapperUtils::iterationToGeneratedLevels(*mpaPeriod, iteration);
        const auto mainReferenceMPA = getMPAFromMapper({0, generatedLevels});

        if (level >= mainReferenceMPA.size() || level + 1 > generatedLevels) {
            throw vkh::exception_invalid_state(
                    "Invalid level detected. it might be a bug! Please contact the developer and "
                    "attach the current location file (.rfl)");
        }

        const PA<Num> &mainReferencePA = mainReferenceMPA[level];
        const itt::ref skip = mainReferencePA.skip;

        auto mpa = getMPAFromMapper({.mapped = flattenTableIndex, .generatedLevels = generatedLevels});


        for (uint32_t i = level + 1; i < levels; ++i) {
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

        for (size_t i = 0; i < level + 1; ++i) {
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
    void MPATable<Num>::uVerifyPA(const std::vector<uint64_t> &itCountLim,
                                             const std::vector<uint64_t> &tablePeriod,
                                             std::vector<bool> &generationAvailable,
                                             std::vector<PAGenerator<Num>> &currentPA,
                                             std::vector<uint64_t> &currentPASkips, std::vector<bool> *isPartial,
                                             PartialPA *partialPA, uint64_t iteration) {

        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        size_t level = 0;
        const size_t levels = tablePeriod.size();

#ifndef NDEBUG
        if (currentPASkips[level] > tablePeriod[level] - PERTURBATION_REQ) {
            throw vkh::exception_invalid_state("skip count is exceeded");
        }
#endif

        while (level < levels && (currentPASkips[level] == tablePeriod[level] - PERTURBATION_REQ ||
                                  itCountLim[level] != tablePeriod[level] || !generationAvailable[level])) {

            if (itCountLim[level] == tablePeriod[level] && generationAvailable[level]) {


                if (isPartial && partialPA && (*isPartial)[level]) {
                    (*partialPA)[level].first.emplace(currentPA[level]);
                    (*isPartial)[level] = false;
                } else {
                    const MPAIndexMapper flattenIndexMapper =
                            tableCache.flattenIndexMapper.interpret<MPAIndexMapper>()[currentPA[level].start];
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
    void MPATable<Num>::cVerifyPA(const std::vector<itt::ref> &itCountLim,
                                           const std::vector<itt::ref> &tablePeriod,
                                           std::vector<bool> &generationAvailable,
                                           std::vector<PAGenerator<Num>> &currentPA, itt::ref iteration) {
        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        itt::ref level = 0;
        const itt::ref levels = tablePeriod.size();

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
        debugCheckMPAFromMapper(tableCache.mpaTable.sizeUsed, flattenIndexMapper.mapped, levels, size);
        PA<Num> *start = tableCache.mpaTable.interpret<PA<Num>>() + flattenIndexMapper.mapped;
        return std::span<PA<Num>>(start, size);
    }

    template<Number Num>
    std::span<const PA<Num>> MPATable<Num>::getMPAFromMapper(const MPAIndexMapper flattenIndexMapper) const {

        const size_t levels = mpaPeriod->tablePeriods.size();
        size_t size = flattenIndexMapper.generatedLevels;
        debugCheckMPAFromMapper(tableCache.mpaTable.sizeUsed, flattenIndexMapper.mapped, levels, size);
        PA<Num> *start = tableCache.mpaTable.interpret<PA<Num>>() + flattenIndexMapper.mapped;
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
    void MPATable<Num>::cRefreshCounter(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                                 const std::vector<itt::ref> &tablePeriod,
                                                 std::vector<bool> &generationAvailable,
                                                 std::vector<PAGenerator<Num>> &currentPA, itt::ref iteration) {


        // reset current and lower level count when it reached limit
        // table period is exponential
        // Amortized O(1)

        size_t level = 0;

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
    void MPATable<Num>::uRefreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                                   const std::vector<uint64_t> &tablePeriod,
                                                   std::vector<bool> &generationAvailable,
                                                   std::vector<PAGenerator<Num>> &currentPA,
                                                   std::vector<uint64_t> &currentPASkips, std::vector<bool> *isPartial,
                                                   uint64_t iteration) {


        // reset current and lower level count when it reached limit
        // Amortized O(1)

        size_t level = 0;

        while (level < tablePeriod.size() - 1 && itCount[level] == itCountLim[level]) {
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
            itCountLim[level] = std::min(tablePeriod[level], itCountLim[level + 1] - itCount[level + 1]);
            itCount[level] = 0;
            generationAvailable[level] = itCountLim[level] == tablePeriod[level];
        }
    }

    template<Number Num>
    void MPATable<Num>::uStepOnce(std::vector<uint64_t> &itCount, const std::vector<uint64_t> &itCountLim,
                                             const std::vector<uint64_t> &tablePeriod,
                                             std::vector<PAGenerator<Num>> &currentPA,
                                             std::vector<uint64_t> &currentPASkips, uint64_t &flattenTableIndex,
                                             uint64_t &iteration) {
        uint64_t levels = 0;
        while (levels < tablePeriod.size() && itCount[levels] == 0 && itCountLim[levels] == tablePeriod[levels]) {
            ++levels;
        }


        if (levels > 0) {
            debugCheckMPAFromMapper(tableCache.mpaTable.sizeUsed, flattenTableIndex, tablePeriod.size(), levels);
            RFF_ASSERT(tableCache.flattenIndexMapper.sizeUsed > iteration);
            tableCache.flattenIndexMapper.interpret<MPAIndexMapper>()[iteration] = {flattenTableIndex, levels};
            flattenTableIndex += levels;
        } else {
            RFF_ASSERT(tableCache.flattenIndexMapper.sizeUsed > iteration);
            tableCache.flattenIndexMapper.interpret<MPAIndexMapper>()[iteration] = {UINT64_MAX, 0};
        }
        currentPA[0].step();
        ++currentPASkips[0];
        ++itCount[0];
        ++iteration;
    }


    template<Number Num>
    void MPATable<Num>::cStepOnce(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                           const std::vector<itt::ref> &tablePeriod,
                                           std::vector<bool> &generationAvailable,
                                           std::vector<PAGenerator<Num>> &currentPA, itt::ref &pulledTableIndex,
                                           size_t &flattenTableIndex, itt::ref &iteration) {
        size_t levels = 0;
        while (levels < tablePeriod.size() && itCount[levels] == 0 && itCountLim[levels] == tablePeriod[levels]) {
            ++levels;
        }

        bool jumped = false;
        if (levels > 0) {

            const size_t compIndex = ReferenceCompressor::compress(pulledMPACompressor, pulledTableIndex);
            RFF_ASSERT(tableCache.flattenIndexMapper.sizeUsed > compIndex);
            tableCache.flattenIndexMapper.interpret<MPAIndexMapper>()[compIndex] = {.mapped = flattenTableIndex, .generatedLevels = levels};

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


        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const itt::ref longestPeriod = tablePeriod.back();

        if (longestPeriod < mpaSettings.minSkipReference)
            return;

        fitBufferSize();

        // iteration starts from 1, the first value is garbage
        tableCache.flattenIndexMapper.interpret<MPAIndexMapper>()[0] = {};

        if (mpaSettings.useCompress) {
            cGenerateTable(state, reference, dcMax, std::forward<FnCreatingTable>(fnCreatingTable));
        } else {
            uGenerateTable(state, reference, dcMax, std::forward<FnCreatingTable>(fnCreatingTable));
        }
    }

    template<Number Num>
    void MPATable<Num>::cInitIterationCountVec(std::vector<itt::ref> &itCount, std::vector<itt::ref> &itCountLim,
                                                  std::vector<bool> &generationAvailable) const {
        //input vector must be empty.
        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const size_t levels = tablePeriod.size();
        itCount.resize(levels, 0);
        itCountLim = tablePeriod;
        generationAvailable.resize(levels, true);
    }


    template<Number Num>
    void MPATable<Num>::uGenerateIterationCountVec(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                                  std::vector<bool> &generationAvailable, std::vector<bool> &isPartial,
                                                  const uint64_t iteration) const {

        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const size_t levels = tablePeriod.size();
        itCount.resize(levels);
        itCountLim.resize(levels);
        generationAvailable.resize(levels);
        isPartial.resize(levels);

        uint64_t remainder = iteration - 1;
        uint64_t lim = UINT64_MAX;

        for (uint64_t i = levels; i > 0; --i) {
            const uint64_t level = i - 1;
            const uint64_t p = tablePeriod[level];
            const uint64_t quotient = remainder / p;

            lim = std::min(static_cast<uint64_t>(tablePeriod[level]), lim - quotient * p);
            remainder -= quotient * p;

            itCount[level] = remainder;
            itCountLim[level] = lim;
            generationAvailable[level] = lim == p && itCount[level] < lim - PERTURBATION_REQ;
            isPartial[level] = remainder > 0;
        }

        for (size_t level = levels - 1; level > 0; --level) {
            itCount[level] -= itCount[level - 1];
        }
    }

    template<Number Num>
    template<FnListeners::FnCreatingTable FnCreatingTable>
    void MPATable<Num>::cGenerateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                                                Num dcMax, FnCreatingTable &&fnCreatingTable) {


        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const itt::ref longestPeriod = tablePeriod.back();
        const size_t levels = tablePeriod.size();
        const auto epsilonPower = mpaSettings.epsilonPower;
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

            cStepOnce(itCount, itCountLim, tablePeriod, generationAvailable, currentPA, pulledTableIndex,
                               flattenTableIndex, iteration);
            cVerifyPA(itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
            cRefreshCounter(itCount, itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
        }
#ifndef NDEBUG
        checkZero(state);
#endif
    }

    template<Number Num>
    std::vector<uint64_t> MPATable<Num>::generateUncompressedCurrentPASkips(const std::vector<itt::ref> &tablePeriod,
                                                                const std::vector<uint64_t> &itCount) {
        const size_t levels = tablePeriod.size();
        std::vector<uint64_t> result = itCount;
        for (size_t j = 0; j < levels - 1; ++j) {
            if (result[j] >= static_cast<uint64_t>(tablePeriod[j]) - PERTURBATION_REQ) {
                result[j + 1] += result[j];
                result[j] = 0;
            }
        }
        return result;
    }

    template<Number Num>
    void MPATable<Num>::uConfigurePartialPA(const std::vector<itt::ref> &tablePeriod,
                                           const std::vector<uint64_t> &itCountLim,
                                           std::vector<PAGenerator<Num>> &currentPA, std::vector<bool> &isPartial,
                                           PartialPA &partialPA) {
        const size_t levels = tablePeriod.size();

        for (size_t j = 1; j < levels; ++j) {
            currentPA[j].merge(currentPA[j - 1]);
        }


        for (size_t j = 0; j < levels; ++j) {
            auto &pp = partialPA[j];

            if (isPartial[j]) {
                pp.first.emplace(currentPA[j]);
            }
            if (tablePeriod[j] == itCountLim[j]) {
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

                    if (preservingPA->skip == tablePeriod[i] - PERTURBATION_REQ) {
                        PA<Num> pa = preservingPA->build();
                        const size_t flattenIndex =
                                MPAIndexMapperUtils::iterationToFlattenTableIndex(*mpaPeriod, preservingPA->start) + i;
#ifndef NDEBUG
                        if (flattenIndex == UINT64_MAX ||
                            tableCache.mpaTable.interpret<PA<Num>>()[flattenIndex].skip != 0)
                            throw vkh::exception_invalid_state("already assigned or flatten index cannot be found");
#endif
                        preservingPA.reset();
                        tableCache.mpaTable.interpret<PA<Num>>()[flattenIndex] = pa;
                    }
                }
            }
        }
    }
    template<Number Num>
    template<FnListeners::FnCreatingTable FnCreatingTable>
    void MPATable<Num>::uGenerateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                                                  Num dcMax, FnCreatingTable &&fnCreatingTable) {

        const auto &tablePeriod = mpaPeriod->tablePeriods;
        const uint64_t longestPeriod = static_cast<uint64_t>(tablePeriod.back());
        const size_t levels = tablePeriod.size();
        const auto epsilonPower = mpaSettings.epsilonPower;
        const float epsilon = std::pow(10.f, epsilonPower);
        const uint32_t threadCount = generalSettings.threads;
        if (mpaSettings.useParallelization) {

            const size_t itInterval = std::max(static_cast<size_t>(tablePeriod[0]), longestPeriod / threadCount + 1);
            std::vector<PartialPA> partialPAs(threadCount);
            std::vector<std::unique_ptr<std::jthread>> threads(threadCount);

            for (auto &v: partialPAs) {
                v.resize(levels);
            }


            for (size_t i = 0; i < threadCount; ++i) {


                threads[i] = std::make_unique<std::jthread>([this, &reference, &state, &fnCreatingTable, dcMax, i,
                                                             itInterval, &tablePeriod, longestPeriod, epsilon, levels,
                                                             &partialPAs, threadCount] {
                    const uint64_t startIteration = itInterval * i + 1;
                    if (startIteration > longestPeriod || state.interruptRequested()) {
                        return;
                    }

                    std::vector<uint64_t> itCount;
                    std::vector<uint64_t> itCountLim;
                    std::vector<bool> generationAvailable;
                    std::vector<bool> isPartial;
                    uGenerateIterationCountVec(itCount, itCountLim, generationAvailable, isPartial, startIteration);
                    std::vector<uint64_t> currentPASkips = generateUncompressedCurrentPASkips(tablePeriod, itCount);

                    uint64_t flattenTableIndex =
                            MPAIndexMapperUtils::iterationToNearestFlattenTableIndex(*mpaPeriod, startIteration);
                    std::vector<PAGenerator<Num>> currentPA(
                            levels, PAGenerator<Num>(reference, epsilon, dcMax, startIteration));

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

                        uStepOnce(itCount, itCountLim, tablePeriod, currentPA, currentPASkips,
                                             flattenTableIndex, iteration);
                        uVerifyPA(itCountLim, tablePeriod, generationAvailable, currentPA, currentPASkips,
                                             &isPartial, &partialPAs[i], iteration);
                        uRefreshCounter(itCount, itCountLim, tablePeriod, generationAvailable, currentPA,
                                                   currentPASkips, &isPartial, iteration);
                    }

                    uConfigurePartialPA(tablePeriod, itCountLim, currentPA, isPartial, partialPAs[i]);
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

            uGluePartialPA(tablePeriod, threadCount, partialPAs);

#ifndef NDEBUG
            checkZero(state);
#endif
        } else {
            uint64_t flattenTableIndex = 0;
            std::vector<uint64_t> itCount;
            std::vector<uint64_t> itCountLim;
            std::vector<bool> generationAvailable;
            std::vector<bool> isPartial;
            uint64_t iteration = 1;
            uGenerateIterationCountVec(itCount, itCountLim, generationAvailable, isPartial, 1);
            std::vector<PAGenerator<Num>> currentPA(levels, PAGenerator<Num>(reference, epsilon, dcMax, 1));
            std::vector<uint64_t> currentPASkips(levels, 0);

            while (iteration <= longestPeriod) {

                if (iteration % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0) {
                    if (state.interruptRequested())
                        return;
                    fnCreatingTable(static_cast<double>(iteration) / static_cast<double>(longestPeriod));
                }


                uStepOnce(itCount, itCountLim, tablePeriod, currentPA, currentPASkips, flattenTableIndex,
                                     iteration);
                uVerifyPA(itCountLim, tablePeriod, generationAvailable, currentPA, currentPASkips, nullptr,
                                     nullptr, iteration);
                uRefreshCounter(itCount, itCountLim, tablePeriod, generationAvailable, currentPA,
                                           currentPASkips, nullptr, iteration);
            }

#ifndef NDEBUG
            checkZero(state);
#endif
        }
    }

#ifndef NDEBUG
    template<Number Num>
    void MPATable<Num>::checkZero(const ParallelRenderState &state) {
        for (size_t i = 0; i < tableCache.mpaTable.sizeUsed; ++i) {
            auto &pa = tableCache.mpaTable.interpret<PA<Num>>()[i];
            if (state.interruptRequested())
                return;
            if (pa.skip == 0) {
                throw vkh::exception_invalid_state(std::format("zero skips detected at index {}", std::to_string(i)));
            }
        }
    }
#endif


    template<Number Num>
    MPAIndexMapper MPATable<Num>::getCompFlattenIndexMapper(const itt::ref iteration) const {
        const auto [pulled, levels] = MPAIndexMapperUtils::iterationToPulledTableIndexMapper(*mpaPeriod, iteration);
        if (pulled == UINT64_MAX) {
            return MPAIndexMapper{.mapped = UINT64_MAX, .generatedLevels = 0};
        }

        const size_t comp = ReferenceCompressor::compress(pulledMPACompressor, pulled);
        return MPAIndexMapper{.mapped = tableCache.flattenIndexMapper.interpret<MPAIndexMapper>()[comp].mapped,
                              .generatedLevels = levels};
    }

    template<Number Num>
    MPAIndexMapper MPATable<Num>::getFlattenIndexMapper(const itt::ref iteration) const {
        if (mpaSettings.useCompress) {
            return getCompFlattenIndexMapper(iteration);
        }
        return tableCache.flattenIndexMapper.interpret<MPAIndexMapper>()[static_cast<size_t>(iteration)];
    }

    template<Number Num>
    const PA<Num> *MPATable<Num>::lookup(const itt::ref refIteration, const complex<Num> &dz) const noexcept {

        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }

        const MPAIndexMapper mapper = getFlattenIndexMapper(refIteration);

        if (mapper.mapped == UINT64_MAX) {
            return nullptr;
        }


        debugCheckMPAFromMapper(tableCache.mpaTable.sizeUsed, mapper.mapped, mpaPeriod->tablePeriods.size(),
                                mapper.generatedLevels);

        const auto table = getMPAFromMapper(mapper);
        const Num r = dz.max();

        switch (mpaSettings.selectionMethod) {
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

                for (size_t j = table.size(); j > 1; --j) {
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
    void MPATable<Num>::lookupBatch(std::array<PA<Num>, RFF_SIMD_CHUNK_SIZE> &results,
                                    const std::array<itt::ref, RFF_SIMD_CHUNK_SIZE> &refIterations,
                                    const std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> &dzs,
                                    const std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &finishes) const noexcept {

        if (mpaPeriod == nullptr) {
            std::ranges::fill(results, PA<Num>::IDENTITY);
            return;
        }

        std::array<MPAIndexMapper, RFF_SIMD_CHUNK_SIZE> mapperTemp;
        std::array<Num, RFF_SIMD_CHUNK_SIZE> radiusTemp;
        std::array<std::span<const PA<Num>>, RFF_SIMD_CHUNK_SIZE> tableTemp;

        for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            mapperTemp[i] = getFlattenIndexMapper(refIterations[i]);
            if (mapperTemp[i].mapped == UINT64_MAX) {
                mapperTemp[i].mapped = 0;
            } else
                debugCheckMPAFromMapper(tableCache.mpaTable.sizeUsed, mapperTemp[i].mapped,
                                        mpaPeriod->tablePeriods.size(), mapperTemp[i].generatedLevels);
            tableTemp[i] = getMPAFromMapper(mapperTemp[i]);
            radiusTemp[i] = dzs[i].max();
        }

        switch (mpaSettings.selectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    const PA<Num> *pa = &PA<Num>::IDENTITY;
                    if (!finishes[i]) {
                        for (const PA<Num> &test: tableTemp[i]) {
                            if (test.isValid(radiusTemp[i]))
                                pa = &test;
                            else
                                break;
                        }
                    }
                    results[i] = *pa;
                }
                break;
            }
            case HIGHEST: {
                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    const PA<Num> *pa = &PA<Num>::IDENTITY;
                    if (!finishes[i] && !tableTemp[i].empty() && tableTemp[i].front().isValid(radiusTemp[i])) {
                        for (size_t j = tableTemp[i].size(); j > 0; --j) {
                            const PA<Num> &test = tableTemp[i][j - 1];
                            if (test.isValid(radiusTemp[i])) {
                                pa = &test;
                                break;
                            }
                        }
                    }
                    results[i] = *pa;
                }
                break;
            }
            default:
                break;
        }
    }

} // namespace merutilm::rff2
