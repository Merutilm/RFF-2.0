//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include <algorithm>
#include <any>

#include "../constants/Constants.hpp"
#include "../data/ApproxTableManager.h"
#include "../settings/FrtMPACompressionMethod.h"
#include "ArrayCompressionTool.h"
#include "ArrayCompressor.h"
#include "DeepPAGenerator.h"
#include "LightPAGenerator.h"
#include "MPAPeriod.h"

#include "../../vulkan_helper/core/logger.hpp"

namespace merutilm::rff2 {
    template<typename Ref, typename Num>
    struct MPATable {
        static constexpr int REQUIRED_PERTURBATION = 2;

        const FrtMPASettings mpaSettings;

        // pulled mpa : fill only valid elements from the sparse mpa vector
        // pulled mpa compressor : distinct the elements from "pulled mpa"
        std::vector<ArrayCompressionTool> pulledMPACompressor = std::vector<ArrayCompressionTool>();

        // important data to generate
        std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;

        // table caches
        ApproxTableManager &tableRef;

        explicit MPATable(const ParallelRenderState &state, const Ref &reference, const FrtMPASettings *mpaSettings,
                          const Num &dcMax, ApproxTableManager &tableRef,
                          std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);


        virtual ~MPATable() = default;

    protected:
        [[nodiscard]] bool tryInit(const MB2Reference &reference);

        [[nodiscard]] std::vector<ArrayCompressionTool>
        generatePulledMPACompressor(const std::vector<ArrayCompressionTool> &referenceCompressor) const;

        static uint64_t binarySearch(const std::vector<uint64_t> &arr, uint64_t key);

        template<class PAB>
            requires std::is_base_of_v<PA, PAB>
        void fitBufferSize(std::unique_ptr<std::pmr::vector<std::pmr::vector<PAB>>> &table);


        template<typename PAB, typename PAG>
            requires std::is_base_of_v<PA, PAB> && std::is_base_of_v<PAGenerator, PAG>
        bool tryJumpTableGeneration(std::pmr::vector<std::pmr::vector<PAB>> &table, const Ref &reference,
                                    double epsilon, const Num &dcMax, std::vector<uint64_t> &periodCount,
                                    std::vector<std::optional<PAG>> &currentPA, uint64_t pulledTableIndex, uint64_t *currentIteration);

        template<typename PAB, typename PAG>
            requires std::is_base_of_v<PA, PAB> && std::is_base_of_v<PAGenerator, PAG>
        void stepOnce(std::pmr::vector<std::pmr::vector<PAB>> &table, const Ref &reference, double epsilon, const Num &dcMax,
                      std::vector<uint64_t> &periodCount, std::vector<std::optional<PAG>> &currentPA,
                      uint64_t pulledTableIndex, uint64_t *currentIteration,
                      bool jumped);

        template<typename PAB, typename PAG>
            requires std::is_base_of_v<PA, PAB> && std::is_base_of_v<PAGenerator, PAG>
        void generateTable(const ParallelRenderState &state, const Ref &reference, Num dcMax,
                           std::unique_ptr<std::pmr::vector<std::pmr::vector<PAB>>> &table,
                           std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

        template<typename PAB>
        void allocateWithCheckTableSize(std::pmr::vector<std::pmr::vector<PAB>> &table, uint64_t index,
                                        uint64_t levels);

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
        virtual size_t getLength() = 0;
    };

    // DEFINITION OF MPA TABLE


    template<typename Ref, typename Num>
    MPATable<Ref, Num>::MPATable(const ParallelRenderState &state, const Ref &reference,
                                 const FrtMPASettings *mpaSettings, const Num &dcMax, ApproxTableManager &tableRef,
                                 std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) :
        mpaSettings(*mpaSettings), tableRef(tableRef) {

        if (tryInit(reference)) {

            if constexpr (std::is_same_v<Ref, LightMB2Reference>) {
                generateTable<LightPA, LightPAGenerator>(state, reference, dcMax, tableRef.mpaLightTable,
                                                         std::move(actionPerCreatingTableIteration));
            } else {
                generateTable<DeepPA, DeepPAGenerator>(state, reference, dcMax, tableRef.mpaDeepTable,
                                                       std::move(actionPerCreatingTableIteration));
            }
        }
    }


    //[re] init mpa periods and compressors
    template<typename Ref, typename Num>
    bool MPATable<Ref, Num>::tryInit(const MB2Reference &reference) {
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

    template<typename Ref, typename Num>
    std::vector<ArrayCompressionTool> MPATable<Ref, Num>::generatePulledMPACompressor(
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

    template<typename Ref, typename Num>
    uint64_t MPATable<Ref, Num>::binarySearch(const std::vector<uint64_t> &arr, const uint64_t key) {
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


    template<typename Ref, typename Num>
    template<typename PAB>
        requires std::is_base_of_v<PA, PAB>
    void MPATable<Ref, Num>::fitBufferSize(std::unique_ptr<std::pmr::vector<std::pmr::vector<PAB>>> &table) {


        // no compression : lastCompIndex > skippableIterationsCount.back()
        // pulled : lastCompIndex == skippableIterationsCount.back()
        // compressed : lastCompIndex < skippableIterationsCount.back()

        const uint64_t longestPeriod = mpaPeriod->tablePeriod.back();
        const uint64_t levels = mpaPeriod->tablePeriod.size();
        const uint64_t skippableIterationsCount = mpaPeriod->skippableIterationsCount.back();
        const uint64_t lastCompIndex = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                                 pulledMPACompressor, longestPeriod + 1);
        const uint64_t bufferSize = levels * sizeof(PAB) * std::min(lastCompIndex, skippableIterationsCount) +
                                    lastCompIndex * sizeof(std::pmr::vector<PAB>);
        tableRef.reinit(lastCompIndex, bufferSize, table);
    }

    template<typename Ref, typename Num>
    template<typename PAB, typename PAG>
        requires std::is_base_of_v<PA, PAB> && std::is_base_of_v<PAGenerator, PAG>
    bool MPATable<Ref, Num>::tryJumpTableGeneration(std::pmr::vector<std::pmr::vector<PAB>> &table,
                                                    const Ref &reference, double epsilon, const Num &dcMax,
                                                    std::vector<uint64_t> &periodCount,
                                                    std::vector<std::optional<PAG>> &currentPA,
                                                    const uint64_t pulledTableIndex,
                                                    uint64_t *const currentIteration) {

        const ArrayCompressionTool *containedTool = ArrayCompressor::find(pulledMPACompressor, pulledTableIndex + 1);
        if (containedTool == nullptr || containedTool->start != pulledTableIndex + 1) {
            return false;
        }
        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t levels = tablePeriod.size();
        const std::pmr::vector<PAB> &mainReferenceMPA = table[0];
        const auto &skippableIterationsCount = mpaPeriod->skippableIterationsCount;
        const uint64_t level = binarySearch(skippableIterationsCount, containedTool->end - containedTool->start + 2);
        // count itself and periodic point, +2

        const uint64_t compTableIndex = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod,
                                                                  pulledMPACompressor, *currentIteration);

        allocateWithCheckTableSize<PAB>(table, compTableIndex, levels);
        auto &pa = table[compTableIndex];

        const PAB &mainReferencePA = mainReferenceMPA[level];
        const uint64_t skip = mainReferencePA.skip;

        for (uint64_t i = level + 1; i < levels; ++i) {
            if (i <= level && periodCount[i] != 0) {
                vkh::logger::w_log(
                        L"WARNING : Failed to compress!! \n what : the table period count {} is not zero.",
                        periodCount[i]);
                return false;
            }
            if (periodCount[i] + skip > tablePeriod[i] - REQUIRED_PERTURBATION) {
                vkh::logger::w_log(L"WARNING : Failed to compress!! \n what : the table period count {} + "
                                       L"skip {} exceeds its period {}.",
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
                    currentPA[i]->merge(mainReferencePA);
                } else {
                    currentPA[i]->merge(mainReferencePA);
                }
                periodCount[i] += skip;
            }
        }

        *currentIteration += skip;
        return true;
    }


    template<typename Ref, typename Num>
    template<typename PAB, typename PAG>
        requires std::is_base_of_v<PA, PAB> && std::is_base_of_v<PAGenerator, PAG>
    void MPATable<Ref, Num>::stepOnce(std::pmr::vector<std::pmr::vector<PAB>> &table, const Ref &reference,
                                      double epsilon, const Num &dcMax, std::vector<uint64_t> &periodCount,
                                      std::vector<std::optional<PAG>> &currentPA, const uint64_t pulledTableIndex,
                                      uint64_t *const currentIteration,
                                      const bool jumped) {

        bool resetLowerLevel = false;
        const bool independent = ArrayCompressor::isIndependent(pulledMPACompressor, pulledTableIndex);

        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t levels = tablePeriod.size();

        for (uint64_t i = levels; i > 0; --i) {
            const uint64_t level = i - 1;
            std::optional<PAG> &currentLevel = currentPA[level];
            if (periodCount[level] == 0 && independent && !jumped) {
                currentLevel.emplace(reference, epsilon, dcMax, *currentIteration);
            }

            if (currentLevel != std::nullopt && periodCount[level] + REQUIRED_PERTURBATION < tablePeriod[level]) {
                currentLevel->step();
            }


            ++periodCount[level];

            if (periodCount[level] == tablePeriod[level]) {
                if (currentLevel != std::nullopt && currentLevel->getSkip() == tablePeriod[level] - REQUIRED_PERTURBATION) {
                    const uint64_t compTableIndex =
                            iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod, pulledMPACompressor,
                                                      currentLevel->getStart());


                    if (compTableIndex == UINT64_MAX) {
                        vkh::logger::w_log(L"FATAL : FAILED TO CREATING TABLE!!\n what : iteration {} is not "
                                               L"pullable. aborting the table creation...",
                                               currentLevel->getStart());
                        return;
                    }

                    allocateWithCheckTableSize<PAB>(table, compTableIndex, levels);
                    auto &pa = table[compTableIndex];

                    if (pa.empty() || pa.back().skip < currentLevel->getSkip())
                        pa.push_back(currentLevel->build());
                    else {
                        vkh::logger::w_log(L"WARNING : The insertion of pa generated from compressed index {} is not allowed. It might be a bug!", compTableIndex);
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


    template<typename Ref, typename Num>
    template<typename PAB, typename PAG>
        requires std::is_base_of_v<PA, PAB> && std::is_base_of_v<PAGenerator, PAG>
    void MPATable<Ref, Num>::generateTable(const ParallelRenderState &state, const Ref &reference, Num dcMax,
                                           std::unique_ptr<std::pmr::vector<std::pmr::vector<PAB>>> &table,
                                           std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) {


        const auto &tablePeriod = mpaPeriod->tablePeriod;
        const uint64_t longestPeriod = tablePeriod.back();
        const auto epsilonPower = mpaSettings.epsilonPower;

        if (longestPeriod < mpaSettings.minSkipReference)
            return;

        const size_t levels = tablePeriod.size();
        fitBufferSize(table);

        const auto func = std::move(actionPerCreatingTableIteration);
        const double epsilon = pow(10, epsilonPower);
        uint64_t iteration = 1;
        uint64_t absIteration = 0;
        auto periodCount = std::vector<uint64_t>(levels, 0);
        auto currentPA = std::vector<std::optional<PAG>>(levels);

        while (iteration <= longestPeriod) {
            if (absIteration % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested())
                return;

            func(iteration, static_cast<double>(iteration) / static_cast<double>(longestPeriod));
            const uint64_t pulledTableIndex = iterationToPulledTableIndex(*mpaPeriod, iteration);
            bool jumped = false;

            if (tryJumpTableGeneration(*table, reference, epsilon, dcMax, periodCount, currentPA, pulledTableIndex, &iteration)) {
                jumped = true;
            }

            stepOnce(*table, reference, epsilon, dcMax, periodCount, currentPA, pulledTableIndex,
                     &iteration, jumped);

            ++iteration;
            ++absIteration;
        }
    }

    template<typename Ref, typename Num>
    uint64_t MPATable<Ref, Num>::iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
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


    template<typename Ref, typename Num>
    uint64_t MPATable<Ref, Num>::iterationToCompTableIndex(const FrtMPACompressionMethod &mpaCompressionMethod,
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

    template<typename Ref, typename Num>
    template<typename PAB>
    void MPATable<Ref, Num>::allocateWithCheckTableSize(std::pmr::vector<std::pmr::vector<PAB>> &table,
                                                        const uint64_t index, const uint64_t levels) {
        if (table.size() <= index) {
            throw vkh::exception_init("index out of range");
        }
        if (table[index].empty()) {
            table[index].reserve(levels);
        }
    }
} // namespace merutilm::rff2
