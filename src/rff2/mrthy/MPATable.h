//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>

#include "../data/ApproxTableCache.h"
#include "../formula/MB2Reference.h"
#include "../parallel/ParallelRenderState.h"
#include "ArrayCompressionTool.h"
#include "ArrayCompressor.h"
#include "MPAIndexMapper.hpp"
#include "MPAPeriod.h"
#include "PAGenerator.h"

#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {


    template<Number Num>
    struct MPATable {

        static constexpr int PERTURBATION_REQ = 2;
        // table caches
        ApproxTableCache<Num> *tableCache = nullptr;

        const FrtMPASettings mpaSettings;

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        std::vector<ArrayCompressionTool> pulledMPACompressor = std::vector<ArrayCompressionTool>();

        // important data to generate
        std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;


        explicit MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                          std::unique_ptr<ApproxTableCacheBase> &tableCache, const FrtMPASettings *mpaSettings,
                          Num dcMax, const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);


    protected:
        [[nodiscard]] bool tryInit(const MB2Reference<Num> &reference,
                                   std::unique_ptr<ApproxTableCacheBase> &tableCache);

        [[nodiscard]] std::vector<ArrayCompressionTool>
        generatePulledMPACompressor(const std::vector<ArrayCompressionTool> &referenceCompressor) const;

        [[nodiscard]] std::span<PA<Num>> getMPAFromMapper(MPAIndexMapper mapper);

        [[nodiscard]] std::span<const PA<Num>> getMPAFromMapper(MPAIndexMapper mapper) const;

        static void debugCheckMPAFromMapper(size_t totalSize, size_t pulledTableIndex, size_t levels,
                                       size_t generatedLevels);

        static uint64_t binarySearch(const std::vector<uint64_t> &arr, uint64_t key);

        void fitBufferSize();


        bool tryJumpTableGeneration(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                    std::vector<PAGenerator<Num>> &currentPA, std::vector<bool> &generationAvailable,
                                    uint64_t &iteration);
        void verifyPA(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
                      std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                      uint64_t iteration);
        void refreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                            const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                            std::vector<PAGenerator<Num>> &currentPA, uint64_t iteration);
        void stepOnce(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                      const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                      std::vector<PAGenerator<Num>> &currentPA, uint64_t &pulledTableIndex, uint64_t &iteration);


        void generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                           const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);

        /**
         * Gets the pulled table index of MPA Table.
         * @param mpaPeriod The generated MPA Period
         * @param iteration The iteration to pull
         * @return The pulled index. if not found, returns @code UINT64_MAX@endcode
         */
        static MPAIndexMapper iterationToPulledTableIndexMapper(const MPAPeriod &mpaPeriod, uint64_t iteration);

        /**
         * Gets the finally compressed table index of MPA Table.
         * @param useCompress The compress flag
         * @param mpaPeriod The generated MPA Period
         * @param pulledMPACompressor The compressor of pulled MPA table
         * @param iteration The iteration to pull
         * @return The finally compressed index. if not found, returns @code UINT64_MAX@endcode
         */
        [[nodiscard]] MPAIndexMapper
        iterationToCompTableIndexMapper(bool useCompress, const MPAPeriod &mpaPeriod,
                                        const std::vector<ArrayCompressionTool> &pulledMPACompressor,
                                        uint64_t iteration) const;


    public:
        [[nodiscard]] const PA<Num> *lookup(uint64_t refIteration, complex<Num> dz) const;

        [[nodiscard]] size_t getLength() const;
    };

    // DEFINITION OF MPA TABLE


    template<Number Num>
    MPATable<Num>::MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                            std::unique_ptr<ApproxTableCacheBase> &tableCache, const FrtMPASettings *mpaSettings,
                            Num dcMax, const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration) :
        mpaSettings(*mpaSettings) {

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
        auto &tablePeriod = mpaPeriod->tablePeriod;
        auto &tablePeriodElements = mpaPeriod->skippableIterationsCount;
        auto &isArtificial = mpaPeriod->isArtificial;

        for (ArrayCompressionTool tool: referenceCompressor) {
            const uint64_t start = tool.start;
            const uint64_t length = tool.range();
            const uint64_t index = binarySearch(tablePeriod, length + 1);

            // Check if the reference compressor is same as period.
            // However, The Computer doesn't know whether the compressor's length came from skipping to the periodic
            // point, or being cut off in the middle. So, Do check tableIndex too.

            if (const auto [tableIndex, _] = iterationToPulledTableIndexMapper(*mpaPeriod, start);
                index != UINT64_MAX && tableIndex != UINT64_MAX && !isArtificial[index]) {
                const uint64_t periodElements = tablePeriodElements[index];
                mpaTools.emplace_back(1, tableIndex + 1, tableIndex + periodElements - 1);
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


        // no compression : lastCompIndex > skippableIterationsCount.back()
        // compressed : lastCompIndex < skippableIterationsCount.back()

        const uint64_t longestPeriod = mpaPeriod->tablePeriod.back();
        const uint64_t levels = mpaPeriod->tablePeriod.size();
        const uint64_t skippableIterationsCount = mpaPeriod->skippableIterationsCount.back();
        const auto [lastCompIndex, _] =
                iterationToCompTableIndexMapper(true, *mpaPeriod, pulledMPACompressor, longestPeriod + 1);
        const uint64_t tableLen = levels * std::min(lastCompIndex, skippableIterationsCount);
        const uint64_t mapperLen = mpaSettings.useCompress ? 0 : longestPeriod + 1;
        tableCache->resize(tableLen, mapperLen);
    }

    template<Number Num>
    bool MPATable<Num>::tryJumpTableGeneration(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                               std::vector<PAGenerator<Num>> &currentPA,
                                               std::vector<bool> &generationAvailable, uint64_t &iteration) {

        if (pulledMPACompressor.empty())
            return false;

        const auto [pulledTableIndex, generatedLevels] = iterationToPulledTableIndexMapper(*mpaPeriod, iteration);
        const ArrayCompressionTool *containedTool = ArrayCompressor::find(pulledMPACompressor, pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != pulledTableIndex + 1) {
            return false;
        }
        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t levels = tablePeriod.size();
        const auto &skippableIterationsCount = mpaPeriod->skippableIterationsCount;
        const uint64_t level = binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const auto mainReferenceMPA = getMPAFromMapper({0, generatedLevels});

        if (level >= mainReferenceMPA.size() || level + 1 > generatedLevels) {
            throw std::invalid_argument("Invalid level detected. it might be a bug! Please contact the developer and "
                                        "attach the current location file (.rfl)");
        }

        const PA<Num> &mainReferencePA = mainReferenceMPA[level];
        const uint64_t skip = mainReferencePA.skip;

        const auto compTableIndex = ArrayCompressor::compress(pulledMPACompressor, pulledTableIndex);

        auto mpa = getMPAFromMapper({compTableIndex, generatedLevels});


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

        iteration += skip;

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
    void MPATable<Num>::verifyPA(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
                                 std::vector<bool> &generationAvailable, std::vector<PAGenerator<Num>> &currentPA,
                                 uint64_t iteration) {

        // reset current and lower level count when it reached limit
        // Amortized O(1)

        uint64_t level = 0;
        const uint64_t levels = tablePeriod.size();
        auto &table = tableCache->mpaTable;

        while (level < levels && (currentPA[level].skip == tablePeriod[level] - PERTURBATION_REQ ||
                                  itCountLim[level] != tablePeriod[level] || !generationAvailable[level])) {

            if (itCountLim[level] == tablePeriod[level] && generationAvailable[level]) {
                const MPAIndexMapper compTableIndexMapper = iterationToCompTableIndexMapper(
                        mpaSettings.useCompress, *mpaPeriod, pulledMPACompressor, currentPA[level].start);


                auto pa = getMPAFromMapper(compTableIndexMapper);
#ifndef NDEBUG
                if (level >= compTableIndexMapper.generatedLevels) {
                    throw std::invalid_argument("invalid level provided");
                }
                debugCheckMPAFromMapper(tableCache->tableSizeUsed, compTableIndexMapper.mapped, levels,
                                   compTableIndexMapper.generatedLevels);
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
    std::span<PA<Num>> MPATable<Num>::getMPAFromMapper(const MPAIndexMapper mapper) {
        const size_t levels = mpaPeriod->tablePeriod.size();
        PA<Num> *start = tableCache->mpaTable.data() + mapper.mapped * levels;
        size_t size = mapper.generatedLevels;
        debugCheckMPAFromMapper(tableCache->tableSizeUsed, mapper.mapped, levels, size);
        return std::span<PA<Num>>(start, size);
    }

    template<Number Num>
    std::span<const PA<Num>> MPATable<Num>::getMPAFromMapper(const MPAIndexMapper mapper) const {

        const size_t levels = mpaPeriod->tablePeriod.size();
        const PA<Num> *start = tableCache->mpaTable.data() + mapper.mapped * levels;
        size_t size = mapper.generatedLevels;
        debugCheckMPAFromMapper(tableCache->tableSizeUsed, mapper.mapped, levels, size);

        return std::span<const PA<Num>>(start, size);
    }

    template<Number Num>
    void MPATable<Num>::debugCheckMPAFromMapper(const size_t totalSize, const size_t pulledTableIndex, const size_t levels,
                                           const size_t generatedLevels) {
#ifndef NDEBUG
        if (levels == 0) {
            throw std::invalid_argument("levels is zero");
        }
        if (totalSize < pulledTableIndex * levels + generatedLevels) {
            throw std::invalid_argument("generatedLevels out of range");
        }
        if (levels < generatedLevels) {
            throw std::invalid_argument("levels out of range");
        }
#endif
    }

    template<Number Num>
    void MPATable<Num>::refreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                       const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                                       std::vector<PAGenerator<Num>> &currentPA, uint64_t iteration) {


        // reset current and lower level count when it reached limit
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
    void MPATable<Num>::stepOnce(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                 const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                                 std::vector<PAGenerator<Num>> &currentPA, uint64_t &pulledTableIndex,
                                 uint64_t &iteration) {
        if (mpaSettings.useCompress) {
            const bool jumped = tryJumpTableGeneration(itCount, itCountLim, currentPA, generationAvailable, iteration);

            if (!jumped) {
                currentPA[0].step();
                ++itCount[0];
                ++iteration;
            }
        } else {
            uint64_t level = 0;
            while (level < tablePeriod.size() && itCount[level] == 0 && itCountLim[level] == tablePeriod[level]) {
                ++level;
            }
            if (level > 0) {
                debugCheckMPAFromMapper(tableCache->tableSizeUsed, pulledTableIndex, tablePeriod.size(), level);
                tableCache->nonCompToPulledIndexMapper[iteration] = {pulledTableIndex++, level};
            } else {
                tableCache->nonCompToPulledIndexMapper[iteration] = {UINT64_MAX, 0};
            }
            currentPA[0].step();
            ++itCount[0];
            ++iteration;
        }
    }

    template<Number Num>
    void MPATable<Num>::generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                                      const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration) {


        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t longestPeriod = tablePeriod.back();
        const auto epsilonPower = mpaSettings.epsilonPower;

        if (longestPeriod < mpaSettings.minSkipReference)
            return;

        const size_t levels = tablePeriod.size();
        fitBufferSize();


        const double epsilon = pow(10, epsilonPower);
        uint64_t iteration = 1;


        std::vector<uint64_t> itCount(levels, 0);
        std::vector<uint64_t> itCountLim(levels, 0);
        std::vector generationAvailable(levels, true);
        std::vector<PAGenerator<Num>> currentPA(levels, PAGenerator<Num>(reference, epsilon, dcMax, 1));

        for (uint64_t i = 0; i < levels; ++i) {
            itCountLim[i] = tablePeriod[i];
        }



        uint64_t pulledTableIndex = 0;

        while (iteration <= longestPeriod) {
            if (state.interruptRequested())
                return;

            actionPerCreatingTableIteration(iteration,
                                            static_cast<double>(iteration) / static_cast<double>(longestPeriod));

            stepOnce(itCount, itCountLim, tablePeriod, generationAvailable, currentPA, pulledTableIndex, iteration);
            verifyPA(itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
            refreshCounter(itCount, itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
        }
    }

    template<Number Num>
    MPAIndexMapper MPATable<Num>::iterationToPulledTableIndexMapper(const MPAPeriod &mpaPeriod,
                                                                    const uint64_t iteration) {
        //
        // get index <=> Inverse calculation of index compression
        // First approach : check the remainder == 1
        //
        // [3, 11, 26]
        // 1 4 7 12 15 18 23 27 30 33 38
        // 3 1 1  2  1  1  2  3  1  1  2


        const auto &tablePeriod = mpaPeriod.tablePeriod;
        const auto &skippableIterationCounts = mpaPeriod.skippableIterationsCount;

        if (iteration == 0) {
            return {UINT64_MAX, 0};
        }
        if (iteration == 1) {
            return {0, tablePeriod.size()};
        }


        uint64_t index = 0;
        uint64_t levels = 0;
        uint64_t remainder = iteration;
        uint64_t maxSkip = UINT64_MAX;
        for (uint64_t i = tablePeriod.size(); i > 0; --i)
        {
            const uint64_t period = tablePeriod[i - 1];
            const uint64_t count = skippableIterationCounts[i - 1];
            const uint64_t quotient = remainder / period;

            remainder -= quotient * period;
            maxSkip = std::min(period, maxSkip - quotient * period);
            if (remainder == 0 || maxSkip < tablePeriod[0]) return MPAIndexMapper{UINT64_MAX, 0};

            levels += maxSkip >= period && remainder == 1;
            index += quotient * count;
        }

        return remainder == 1 && levels > 0 ? MPAIndexMapper{index, levels} : MPAIndexMapper{UINT64_MAX, 0};
    }


    template<Number Num>
    MPAIndexMapper
    MPATable<Num>::iterationToCompTableIndexMapper(const bool useCompress, const MPAPeriod &mpaPeriod,
                                                   const std::vector<ArrayCompressionTool> &pulledMPACompressor,
                                                   const uint64_t iteration) const {
        if (useCompress) {
            const auto [pulled, levels] = iterationToPulledTableIndexMapper(mpaPeriod, iteration);
            if (pulled == UINT64_MAX) {
                return MPAIndexMapper{UINT64_MAX, 0};
            }

            const uint64_t mapped = ArrayCompressor::compress(pulledMPACompressor, pulled);
            return MPAIndexMapper{mapped, levels};
        } else {
            return tableCache->nonCompToPulledIndexMapper[iteration];
        }
    }


    template<Number Num>
    const PA<Num> *MPATable<Num>::lookup(const uint64_t refIteration, const complex<Num> dz) const {

        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }

        const MPAIndexMapper mapper =
                iterationToCompTableIndexMapper(mpaSettings.useCompress, *mpaPeriod, pulledMPACompressor, refIteration);

        if (mapper.mapped == UINT64_MAX) {
            return nullptr;
        }


        debugCheckMPAFromMapper(tableCache->tableSizeUsed, mapper.mapped, mpaPeriod->tablePeriod.size(),
                           mapper.generatedLevels);

        const auto table = getMPAFromMapper(mapper);
        const Num r = dz.norm_approx();

        switch (mpaSettings.mpaSelectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                const PA<Num> *pa = nullptr;

                for (const PA<Num> &test: table) {
#ifndef NDEBUG
                    if (test.skip == 0) {
                        throw std::logic_error("zero skips detected");
                    }
#endif

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
#ifndef NDEBUG
                if (pa.skip == 0) {
                    throw std::logic_error("zero skips detected");
                }
#endif
                if (!pa.isValid(r)) {
                    return nullptr;
                }

                for (uint64_t j = table.size(); j > 0; --j) {
                    const PA<Num> &test = table[j - 1];
#ifndef NDEBUG
                    if (test.skip == 0) {
                        throw std::logic_error("zero skips detected");
                    }
#endif

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
