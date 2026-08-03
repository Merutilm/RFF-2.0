//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>

#include "../constants/Constants.hpp"
#include "../data/ApproxTableManager.h"
#include "../formula/MB2Reference.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtMPACompressionMethod.h"
#include "ArrayCompressionTool.h"
#include "ArrayCompressor.h"
#include "MPAPeriod.h"
#include "PAGenerator.h"

#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {


    template<Number Num>
    struct MPATable {

        static constexpr int PERTURBATION_REQ = 2;
        const FrtMPASettings mpaSettings;

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        std::vector<ArrayCompressionTool> pulledMPACompressor = std::vector<ArrayCompressionTool>();

        // important data to generate
        std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;

        // table caches
        std::optional<ApproxTableManager<Num>> tableManager;

        explicit MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                          const FrtMPASettings *mpaSettings, Num dcMax,
                          const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);


    protected:
        [[nodiscard]] bool tryInit(const MB2Reference<Num> &reference);

        [[nodiscard]] std::vector<ArrayCompressionTool>
        generatePulledMPACompressor(const std::vector<ArrayCompressionTool> &referenceCompressor) const;

        static uint64_t binarySearch(const std::vector<uint64_t> &arr, uint64_t key);

        void fitBufferSize();


        bool tryJumpTableGeneration(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                    std::vector<std::optional<PAGenerator<Num>>> &currentPA,
                                    std::vector<bool> &generationAvailable, uint64_t *iteration);
        void verifyPA(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
                      std::vector<bool> &generationAvailable, std::vector<std::optional<PAGenerator<Num>>> &currentPA,
                      uint64_t iteration);
        void refreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                            const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                            std::vector<std::optional<PAGenerator<Num>>> &currentPA, uint64_t iteration);


        void generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                           const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration);

        void allocateWithCheckTableSize(uint64_t index, uint64_t levels);

        /**
         * Gets the pulled table index of MPA Table.
         * @param mpaPeriod The generated MPA Period
         * @param iteration The iteration to pull
         * @return The pulled index. if not found, returns @code UINT64_MAX@endcode
         */
        static uint64_t iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, uint64_t iteration);

        /**
         * Gets the finally compressed table index of MPA Table.
         * @param mpaCompressionMethod The MPA compression Method
         * @param mpaPeriod The generated MPA Period
         * @param pulledMPACompressor The compressor of pulled MPA table
         * @param iteration The iteration to pull
         * @return The finally compressed index. if not found, returns @code UINT64_MAX@endcode
         */
        static uint64_t iterationToCompTableIndex(const FrtMPACompressionMethod &mpaCompressionMethod,
                                                  const MPAPeriod &mpaPeriod,
                                                  const std::vector<ArrayCompressionTool> &pulledMPACompressor,
                                                  uint64_t iteration);


    public:
        [[nodiscard]] PA<Num> *lookup(uint64_t refIteration, complex<Num> dz) const;

        [[nodiscard]] size_t getLength() const;
    };

    // DEFINITION OF MPA TABLE


    template<Number Num>
    MPATable<Num>::MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                            const FrtMPASettings *mpaSettings, Num dcMax,
                            const std::function<void(uint64_t, float)> &actionPerCreatingTableIteration) :
        mpaSettings(*mpaSettings) {

        if (tryInit(reference)) {
            generateTable(state, reference, dcMax, actionPerCreatingTableIteration);
        }
    }


    //[re] init mpa periods and compressors
    template<Number Num>
    bool MPATable<Num>::tryInit(const MB2Reference<Num> &reference) {
        const auto &referencePeriod = reference.period;
        const uint64_t longestPeriod = reference.longestPeriod();

        if (const int minSkip = mpaSettings.minSkipReference; longestPeriod < minSkip) {
            this->mpaPeriod = nullptr;
            this->pulledMPACompressor = std::vector<ArrayCompressionTool>();
            return false;
        }

        const FrtMPACompressionMethod compressionMethod = mpaSettings.mpaCompressionMethod;
        this->mpaPeriod = MPAPeriod::generate(referencePeriod, mpaSettings);
        this->pulledMPACompressor = compressionMethod == FrtMPACompressionMethod::STRONGEST
                                            ? generatePulledMPACompressor(reference.compressor)
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

            if (const uint64_t tableIndex = iterationToPulledTableIndex(*mpaPeriod, start);
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
        // pulled : lastCompIndex == skippableIterationsCount.back()
        // compressed : lastCompIndex < skippableIterationsCount.back()

        const uint64_t longestPeriod = mpaPeriod->tablePeriod.back();
        const uint64_t levels = mpaPeriod->tablePeriod.size();
        const uint64_t skippableIterationsCount = mpaPeriod->skippableIterationsCount.back();
        const uint64_t lastCompIndex = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                                 pulledMPACompressor, longestPeriod + 1);
        const uint64_t bufferSize = levels * sizeof(PA<Num>) * std::min(lastCompIndex, skippableIterationsCount) +
                                    lastCompIndex * sizeof(std::pmr::vector<PA<Num>>);
        tableManager.emplace(bufferSize, lastCompIndex);
    }

    template<Number Num>
    bool MPATable<Num>::tryJumpTableGeneration(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                               std::vector<std::optional<PAGenerator<Num>>> &currentPA,
                                               std::vector<bool> &generationAvailable, uint64_t *const iteration) {

        if (pulledMPACompressor.empty()) return false;

        const uint64_t pulledTableIndex = iterationToPulledTableIndex(*mpaPeriod, *iteration);
        const ArrayCompressionTool *containedTool = ArrayCompressor::find(pulledMPACompressor, pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != pulledTableIndex + 1) {
            return false;
        }
        auto &table = *tableManager->mpaTable;
        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t levels = tablePeriod.size();
        const std::pmr::vector<PA<Num>> &mainReferenceMPA = table[0];
        const auto &skippableIterationsCount = mpaPeriod->skippableIterationsCount;
        const uint64_t level = binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const uint64_t compTableIndex = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                                  pulledMPACompressor, *iteration);

        allocateWithCheckTableSize(compTableIndex, levels);
        auto &pa = table[compTableIndex];

        const PA<Num> &mainReferencePA = mainReferenceMPA[level];
        const uint64_t skip = mainReferencePA.skip;

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

        *iteration += skip;

        for (uint64_t i = 0; i < std::min(level + 1, levels); ++i) {
            pa.push_back(mainReferenceMPA[i]);
            currentPA[i]->reuse(*iteration);
            itCount[i] = 0;
            itCountLim[i] = PERTURBATION_REQ;
            generationAvailable[i] = false;
        }

        if (level + 1 < levels) {
            itCount[level + 1] += skip;
            currentPA[level + 1]->merge(mainReferencePA);
        }
        return true;
    }

    template<Number Num>
    void MPATable<Num>::verifyPA(const std::vector<uint64_t> &itCountLim, const std::vector<uint64_t> &tablePeriod,
                                 std::vector<bool> &generationAvailable,
                                 std::vector<std::optional<PAGenerator<Num>>> &currentPA, uint64_t iteration) {
        uint64_t level = 0;
        const uint64_t levels = tablePeriod.size();
        auto &table = *tableManager->mpaTable;

        while (level < levels && (currentPA[level]->skip == tablePeriod[level] - PERTURBATION_REQ ||
                                  itCountLim[level] != tablePeriod[level])) {

            if (itCountLim[level] == tablePeriod[level] && generationAvailable[level]) {
                const uint64_t compTableIndex = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                                          pulledMPACompressor, currentPA[level]->start);
                allocateWithCheckTableSize(compTableIndex, levels);
                auto &pa = table[compTableIndex];

                if (pa.empty() || pa.back().skip < currentPA[level]->skip) {
                    pa.emplace_back(currentPA[level]->build());
                }
                generationAvailable[level] = false;
            }

            if (level < levels - 1) {
                currentPA[level + 1]->merge(*currentPA[level]);
                currentPA[level]->reuse(iteration);
            }
            ++level;
        }
    }

    template<Number Num>
    void MPATable<Num>::refreshCounter(std::vector<uint64_t> &itCount, std::vector<uint64_t> &itCountLim,
                                       const std::vector<uint64_t> &tablePeriod, std::vector<bool> &generationAvailable,
                                       std::vector<std::optional<PAGenerator<Num>>> &currentPA, uint64_t iteration) {


        uint64_t level = 0;

        // reset current and lower level count when it reached limit
        // Worst case, O(1).
        while (level < tablePeriod.size() - 1 && itCount[level] == itCountLim[level]) {
            itCount[level + 1] += itCount[level];
            currentPA[level + 1]->merge(*currentPA[level]);
            currentPA[level]->reuse(iteration);
            ++level;
        }

        while (level > 0) {
            --level;
            itCountLim[level] = std::min(tablePeriod[level], itCountLim[level + 1] - itCount[level + 1]);
            itCount[level] = 0;
            generationAvailable[level] = true;
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
        uint64_t absIteration = 0;
        // auto periodCount = std::vector<uint64_t>(levels, 0);
        auto currentPA = std::vector<std::optional<PAGenerator<Num>>>(levels);
        for (auto &pa: currentPA) {
            pa.emplace(reference, epsilon, dcMax, 1);
        }


        std::vector<uint64_t> itCount(levels, 0);
        std::vector<uint64_t> itCountLim(levels, 0); // copy the initial value.
        std::vector<bool> generationAvailable(levels, true);

        for (uint64_t i = 0; i < levels; ++i) {
            itCountLim[i] = tablePeriod[i];
        }


        std::vector<bool> itResetFlag(levels, false);
        auto &table = *tableManager->mpaTable;


        while (iteration <= longestPeriod) {
            if (state.interruptRequested())
                return;

            actionPerCreatingTableIteration(iteration,
                                            static_cast<double>(iteration) / static_cast<double>(longestPeriod));

            const bool jumped = tryJumpTableGeneration(itCount, itCountLim, currentPA, generationAvailable, &iteration);

            if (!jumped) {
                currentPA[0]->step();
                ++itCount[0];
                ++iteration;
                ++absIteration;
            }

            verifyPA(itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
            refreshCounter(itCount, itCountLim, tablePeriod, generationAvailable, currentPA, iteration);
        }
    }

    template<Number Num>
    uint64_t MPATable<Num>::iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
        //
        // get index <=> Inverse calculation of index compression
        // First approach : check the remainder == 1
        //
        // [3, 11, 26]
        // 1 4 7 12 15 18 23 27 30 33 38
        //
        // test input : 23
        // search period : period 11
        // 23 % 11 = 1, 23/11 = 2.xxx(3*2 elements)
        // 1 % 3 = 1, 1/3 = 0.xxx(1*0 elements)
        // result = 3*2=6
        //
        // test input : 30
        // search period : period 26
        // 30 % 26 = 4, 30/26 = 1.xxx(7*1 elements)
        // 4 % 3 = 1, 4/3 = 1.xxx(1 element)
        // result = 7*1+1=8
        //
        // test input : 29
        // search period : period 26
        // 29 % 26 = 3, 29/26 = 1.xxx(7*1 elements)
        // 3 % 3 = 0, 3/3 = 1.xxx(1 element)
        // result = UINT64_MAX (last remainder is not one)
        //
        //
        //

        if (iteration == 0) {
            return UINT64_MAX;
        }

        const auto &tablePeriod = mpaPeriod.tablePeriod;
        const auto &tablePeriodElements = mpaPeriod.skippableIterationsCount;

        uint64_t index = 0;
        uint64_t remainder = iteration;

        for (uint64_t i = tablePeriod.size(); i > 0; --i) {
            if (remainder < tablePeriod[i - 1]) {
                continue;
            }
            // p[4, 1000]
            // 1 5 9 13 .... 993 997 1001
            // 997 % 1000 = 997
            // 997 % 4 = 1
            // 997 + 4 - 2 + 1 = 1000
            if (i < tablePeriod.size() && remainder + tablePeriod[0] - PERTURBATION_REQ + 1 > tablePeriod[i]) {
                return UINT64_MAX;
                // Insufficient length, ('Pulled Table Index' must be skipped for at least 'shortest period')
            }


            index += remainder / tablePeriod[i - 1] * tablePeriodElements[i - 1];
            remainder %= tablePeriod[i - 1];
        }
        return remainder == 1 ? index : UINT64_MAX;
    }


    template<Number Num>
    uint64_t MPATable<Num>::iterationToCompTableIndex(const FrtMPACompressionMethod &mpaCompressionMethod,
                                                      const MPAPeriod &mpaPeriod,
                                                      const std::vector<ArrayCompressionTool> &pulledMPACompressor,
                                                      const uint64_t iteration) {
        switch (mpaCompressionMethod) {
            using enum FrtMPACompressionMethod;
            case NO_COMPRESSION:
                return iteration;
            case LITTLE_COMPRESSION:
                return iterationToPulledTableIndex(mpaPeriod, iteration);
            case STRONGEST: {
                const uint64_t index = iterationToPulledTableIndex(mpaPeriod, iteration);
                return index == UINT64_MAX ? UINT64_MAX : ArrayCompressor::compress(pulledMPACompressor, index);
            }
            default:
                return iteration;
        }
    }


    template<Number Num>
    void MPATable<Num>::allocateWithCheckTableSize(const uint64_t index, const uint64_t levels) {
        auto &table = *tableManager->mpaTable;
        if (table.size() <= index) {
            throw vkh::exception_init("index out of range");
        }
        if (table[index].empty()) {
            table[index].reserve(levels);
        }
    }


    template<Number Num>
    PA<Num> *MPATable<Num>::lookup(const uint64_t refIteration, const complex<Num> dz) const {

        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }

        const uint64_t index = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                         pulledMPACompressor, refIteration);

        if (index >= tableManager->mpaTable->size()) {
            return nullptr;
        }

        std::pmr::vector<PA<Num>> &table = (*this->tableManager->mpaTable)[index];
        if (table.empty()) {
            return nullptr;
        }

        const Num r = dz.norm_approx();

        switch (mpaSettings.mpaSelectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                PA<Num> *pa = nullptr;

                for (PA<Num> &test: table) {
                    if (test.isValid(r)) {
                        pa = &test;
                    } else
                        return pa;
                }
                return pa;
            }
            case HIGHEST: {
                PA<Num> &pa = table.front();
                // This table cannot be empty because the pre-processing is done.

                if (!pa.isValid(r)) {
                    return nullptr;
                }

                for (uint64_t j = table.size(); j > 0; --j) {
                    PA<Num> &test = table[j - 1];
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
        return tableManager && tableManager->mpaTable ? tableManager->mpaTable->size() : 0;
    }

    using LightMPATable = MPATable<double>;
    using DeepMPATable = MPATable<dex>;
} // namespace merutilm::rff2
