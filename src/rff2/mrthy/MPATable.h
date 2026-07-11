//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>
#include <any>

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

    struct MPATableBase {

        virtual ~MPATableBase() = default;

        virtual size_t getLength() const = 0;
    };


    template<Number Num, uint8_t MAX_DEGREE>
    struct MPATable : MPATableBase {

        static constexpr int REQUIRED_PERTURBATION = 2;
        const FrtMPASettings mpaSettings;

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        std::vector<ArrayCompressionTool> pulledMPACompressor = std::vector<ArrayCompressionTool>();

        // important data to generate
        std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;

        // table caches
        std::optional<ApproxTableManager<Num, MAX_DEGREE>> tableManager;

        explicit MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                          const FrtMPASettings *mpaSettings, Num dcMax,
                          std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);


    protected:
        [[nodiscard]] bool tryInit(const MB2Reference<Num> &reference);

        [[nodiscard]] std::vector<ArrayCompressionTool>
        generatePulledMPACompressor(const std::vector<ArrayCompressionTool> &referenceCompressor) const;

        static uint64_t binarySearch(const std::vector<uint64_t> &arr, uint64_t key);

        void fitBufferSize();


        bool tryJumpTableGeneration(const MB2Reference<Num> &reference, double epsilon, Num dcMax,
                                    std::vector<uint64_t> &periodCount,
                                    std::vector<std::optional<PAGenerator<Num, MAX_DEGREE>>> &currentPA,
                                    uint64_t pulledTableIndex, uint64_t *currentIteration);


        void stepOnce(const MB2Reference<Num> &reference, double epsilon, Num dcMax, std::vector<uint64_t> &periodCount,
                      std::vector<std::optional<PAGenerator<Num, MAX_DEGREE>>> &currentPA, uint64_t pulledTableIndex,
                      const uint64_t *currentIteration, bool jumped);


        void generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference, Num dcMax,
                           std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

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
        [[nodiscard]] PA<Num, MAX_DEGREE> *lookup(uint64_t refIteration, complex<Num> dz) const;

        [[nodiscard]] size_t getLength() const override;
    };

    // DEFINITION OF MPA TABLE


    template<Number Num, uint8_t MAX_DEGREE>
    MPATable<Num, MAX_DEGREE>::MPATable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                                        const FrtMPASettings *mpaSettings, Num dcMax,
                                        std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) :
        mpaSettings(*mpaSettings) {

        if (tryInit(reference)) {
            generateTable(state, reference, dcMax, std::move(actionPerCreatingTableIteration));
        }
    }


    //[re] init mpa periods and compressors
    template<Number Num, uint8_t MAX_DEGREE>
    bool MPATable<Num, MAX_DEGREE>::tryInit(const MB2Reference<Num> &reference) {
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

    template<Number Num, uint8_t MAX_DEGREE>
    std::vector<ArrayCompressionTool> MPATable<Num, MAX_DEGREE>::generatePulledMPACompressor(
            const std::vector<ArrayCompressionTool> &referenceCompressor) const {
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

    template<Number Num, uint8_t MAX_DEGREE>
    uint64_t MPATable<Num, MAX_DEGREE>::binarySearch(const std::vector<uint64_t> &arr, const uint64_t key) {
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


    template<Number Num, uint8_t MAX_DEGREE>
    void MPATable<Num, MAX_DEGREE>::fitBufferSize() {


        // no compression : lastCompIndex > skippableIterationsCount.back()
        // pulled : lastCompIndex == skippableIterationsCount.back()
        // compressed : lastCompIndex < skippableIterationsCount.back()

        const uint64_t longestPeriod = mpaPeriod->tablePeriod.back();
        const uint64_t levels = mpaPeriod->tablePeriod.size();
        const uint64_t skippableIterationsCount = mpaPeriod->skippableIterationsCount.back();
        const uint64_t lastCompIndex = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                                 pulledMPACompressor, longestPeriod + 1);
        const uint64_t bufferSize =
                levels * PA<Num, MAX_DEGREE>::memoryUsage() * std::min(lastCompIndex, skippableIterationsCount) +
                lastCompIndex * sizeof(std::pmr::vector<PA<Num, MAX_DEGREE>>);
        tableManager.emplace(bufferSize, lastCompIndex);
    }

    template<Number Num, uint8_t MAX_DEGREE>
    bool MPATable<Num, MAX_DEGREE>::tryJumpTableGeneration(
            const MB2Reference<Num> &reference, double epsilon, Num dcMax, std::vector<uint64_t> &periodCount,
            std::vector<std::optional<PAGenerator<Num, MAX_DEGREE>>> &currentPA, const uint64_t pulledTableIndex,
            uint64_t *const currentIteration) {

        const ArrayCompressionTool *containedTool = ArrayCompressor::find(pulledMPACompressor, pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != pulledTableIndex + 1) {
            return false;
        }
        auto &table = *tableManager->mpaTable;
        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t levels = tablePeriod.size();
        const std::pmr::vector<PA<Num, MAX_DEGREE>> &mainReferenceMPA = table[0];
        const auto &skippableIterationsCount = mpaPeriod->skippableIterationsCount;
        const uint64_t level = binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const uint64_t compTableIndex = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                                  pulledMPACompressor, *currentIteration);

        allocateWithCheckTableSize(compTableIndex, levels);
        auto &pa = table[compTableIndex];

        const PA<Num, MAX_DEGREE> &mainReferencePA = mainReferenceMPA[level];
        const uint64_t skip = mainReferencePA.skip;

        for (uint64_t i = level + 1; i < levels; ++i) {
            if (i <= level && periodCount[i] != 0) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} is not zero.",
                                   periodCount[i]);
                return false;
            }
            if (periodCount[i] + skip > tablePeriod[i] - REQUIRED_PERTURBATION) {
                vkh::logger::log("WARNING : Failed to compress!! \n what : the table period count {} + "
                                   "skip {} exceeds its period {}.",
                                   periodCount[i], skip, tablePeriod[i]);
                return false;
            }
        }


        for (uint64_t i = 0; i < levels; ++i) {
            if (i <= level) {
                pa.push_back(mainReferenceMPA[i]);
                uint64_t count = skip;
                for (uint64_t j = level; j > i; --j) {
                    count %= tablePeriod[j - 1];
                }
                currentPA[i] = std::nullopt;
                periodCount[i] = count;
            } else {
                if (currentPA[i] == std::nullopt) {
                    currentPA[i].emplace(reference, epsilon, dcMax, *currentIteration);
                }
                currentPA[i]->merge(mainReferencePA);
                periodCount[i] += skip;
            }
        }

        *currentIteration += skip;
        return true;
    }


    template<Number Num, uint8_t MAX_DEGREE>
    void MPATable<Num, MAX_DEGREE>::stepOnce(const MB2Reference<Num> &reference, double epsilon, Num dcMax,
                                             std::vector<uint64_t> &periodCount,
                                             std::vector<std::optional<PAGenerator<Num, MAX_DEGREE>>> &currentPA,
                                             const uint64_t pulledTableIndex, const uint64_t *const currentIteration,
                                             const bool jumped) {

        bool resetLowerLevel = false;
        const bool independent = ArrayCompressor::isIndependent(pulledMPACompressor, pulledTableIndex);

        auto &table = *tableManager->mpaTable;
        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t levels = tablePeriod.size();

        for (uint64_t i = levels; i > 0; --i) {
            const uint64_t level = i - 1;
            std::optional<PAGenerator<Num, MAX_DEGREE>> &currentLevel = currentPA[level];
            if (periodCount[level] == 0 && independent && !jumped) {
                currentLevel.emplace(reference, epsilon, dcMax, *currentIteration);
            }

            if (currentLevel != std::nullopt && periodCount[level] + REQUIRED_PERTURBATION < tablePeriod[level]) {
                currentLevel->step();
            }


            ++periodCount[level];

            if (periodCount[level] == tablePeriod[level]) {
                if (currentLevel != std::nullopt && currentLevel->skip == tablePeriod[level] - REQUIRED_PERTURBATION) {
                    const uint64_t compTableIndex = iterationToCompTableIndex(
                            mpaSettings.mpaCompressionMethod, *mpaPeriod, pulledMPACompressor, currentLevel->start);


                    if (compTableIndex == UINT64_MAX) {
                        vkh::logger::log_err("FATAL : FAILED TO CREATING TABLE!!\n what : iteration {} is not "
                                           "pullable. aborting the table creation...",
                                           currentLevel->start);
                        return;
                    }

                    allocateWithCheckTableSize(compTableIndex, levels);
                    auto &pa = table[compTableIndex];

                    if (pa.empty() || pa.back().skip < currentLevel->skip)
                        pa.push_back(currentLevel->build(tableManager->strictTableResource.get()));
                    else {
                        vkh::logger::log_err("WARNING : The insertion of pa generated from compressed index {} is not "
                                           "allowed. It might be a bug!",
                                           compTableIndex);
                    }
                }
                // Stop all lower level iteration for efficiency
                // because it is too hard to skipping to next part of the periodic point
                currentLevel = std::nullopt;
                resetLowerLevel = true;
            }

            if (resetLowerLevel) {
                periodCount[level] = 0;
            }
        }
    }


    template<Number Num, uint8_t MAX_DEGREE>
    void
    MPATable<Num, MAX_DEGREE>::generateTable(const ParallelRenderState &state, const MB2Reference<Num> &reference,
                                             Num dcMax,
                                             std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) {


        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t longestPeriod = tablePeriod.back();
        const auto epsilonPower = mpaSettings.epsilonPower;

        if (longestPeriod < mpaSettings.minSkipReference)
            return;

        const size_t levels = tablePeriod.size();
        fitBufferSize();

        const auto func = std::move(actionPerCreatingTableIteration);
        const double epsilon = pow(10, epsilonPower);
        uint64_t iteration = 1;
        uint64_t absIteration = 0;
        auto periodCount = std::vector<uint64_t>(levels, 0);
        auto currentPA = std::vector<std::optional<PAGenerator<Num, MAX_DEGREE>>>(levels);

        while (iteration <= longestPeriod) {
            if (state.interruptRequested())
                return;

            func(iteration, static_cast<double>(iteration) / static_cast<double>(longestPeriod));
            const uint64_t pulledTableIndex = iterationToPulledTableIndex(*mpaPeriod, iteration);
            bool jumped = false;

            //TODO unwrap this scope if merging pa supports other degree
            if constexpr (MAX_DEGREE == static_cast<uint8_t>(FrtMPADegree::P1_STANDARD)) {
                if (tryJumpTableGeneration(reference, epsilon, dcMax, periodCount, currentPA, pulledTableIndex,
                                           &iteration)) {
                    jumped = true;
                }
            }

            stepOnce(reference, epsilon, dcMax, periodCount, currentPA, pulledTableIndex, &iteration, jumped);

            ++iteration;
            ++absIteration;
        }
    }

    template<Number Num, uint8_t MAX_DEGREE>
    uint64_t MPATable<Num, MAX_DEGREE>::iterationToPulledTableIndex(const MPAPeriod &mpaPeriod,
                                                                    const uint64_t iteration) {
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
            if (i < tablePeriod.size() && remainder + tablePeriod[0] - REQUIRED_PERTURBATION + 1 > tablePeriod[i]) {
                return UINT64_MAX;
                // Insufficient length, ("Pulled Table Index" must be skipped for at least "shortest period")
            }


            index += remainder / tablePeriod[i - 1] * tablePeriodElements[i - 1];
            remainder %= tablePeriod[i - 1];
        }
        return remainder == 1 ? index : UINT64_MAX;
    }


    template<Number Num, uint8_t MAX_DEGREE>
    uint64_t MPATable<Num, MAX_DEGREE>::iterationToCompTableIndex(
            const FrtMPACompressionMethod &mpaCompressionMethod, const MPAPeriod &mpaPeriod,
            const std::vector<ArrayCompressionTool> &pulledMPACompressor, const uint64_t iteration) {
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


    template<Number Num, uint8_t MAX_DEGREE>
    void MPATable<Num, MAX_DEGREE>::allocateWithCheckTableSize(const uint64_t index, const uint64_t levels) {
        auto &table = *tableManager->mpaTable;
        if (table.size() <= index) {
            throw vkh::exception_init("index out of range");
        }
        if (table[index].empty()) {
            table[index].reserve(levels);
        }
    }


    template<Number Num, uint8_t MAX_DEGREE>
    PA<Num, MAX_DEGREE> *MPATable<Num, MAX_DEGREE>::lookup(const uint64_t refIteration, const complex<Num> dz) const {

        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }

        const uint64_t index = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                         pulledMPACompressor, refIteration);

        if (index >= tableManager->mpaTable->size()) {
            return nullptr;
        }

        std::pmr::vector<PA<Num, MAX_DEGREE>> &table = (*this->tableManager->mpaTable)[index];
        if (table.empty()) {
            return nullptr;
        }

        const Num r = dz.norm_approx();

        switch (mpaSettings.mpaSelectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                PA<Num, MAX_DEGREE> *pa = nullptr;

                for (PA<Num, MAX_DEGREE> &test: table) {
                    if (test.isValid(r)) {
                        pa = &test;
                    } else
                        return pa;
                }
                return pa;
            }
            case HIGHEST: {
                PA<Num, MAX_DEGREE> &pa = table.front();
                // This table cannot be empty because the pre-processing is done.

                if (!pa.isValid(r)) {
                    return nullptr;
                }

                for (uint64_t j = table.size(); j > 0; --j) {
                    PA<Num, MAX_DEGREE> &test = table[j - 1];
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

    template<Number Num, uint8_t MAX_DEGREE>
    size_t MPATable<Num, MAX_DEGREE>::getLength() const {
        return tableManager && tableManager->mpaTable ? tableManager->mpaTable->size() : 0;
    }

    template<uint8_t MAX_DEGREE>
    using LightMPATable = MPATable<double, MAX_DEGREE>;

    template<uint8_t MAX_DEGREE>
    using DeepMPATable = MPATable<dex, MAX_DEGREE>;
} // namespace merutilm::rff2
