//
// Created by Merutilm on 2025-05-11.
//

#include "LightMPATable.h"

#include <cmath>
#include <iostream>
#include <execution>

#include "ApproxMath.h"
#include "../ui/RFF.h"


LightMPATable::LightMPATable(const ParallelRenderState &state, const LightMandelbrotReference *reference,
                             const MPASettings *mpaSettings, const double dcMax,
                             std::vector<std::vector<LightPA> > &&previousAllocatedTable,
                             std::function<void(uint64_t, double)> &&
                             actionPerCreatingTableIteration) : reference(reference), mpaSettings(*mpaSettings),
                                                                table(std::move(previousAllocatedTable)) {
    if (reference != nullptr) {
        initTable(*reference);
        generateTable(state, dcMax, std::move(actionPerCreatingTableIteration));
    }
}


LightMPATable::LightMPATable(LightMPATable && other) noexcept : mpaSettings(std::move(other.mpaSettings)){
    this->reference = std::move(other.reference);
    this->table = std::move(other.table);
    this->mpaPeriod = std::move(other.mpaPeriod);
    this->pulledMPACompressor = std::move(other.pulledMPACompressor);
}



void LightMPATable::generateTable(const ParallelRenderState &state, const double dcMax,
                                  std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) {
    const auto func = std::move(actionPerCreatingTableIteration);

    if (mpaPeriod == nullptr) {
        return;
    }

    const auto &tablePeriod = mpaPeriod->tablePeriod;
    const uint64_t longestPeriod = tablePeriod.back();
    const auto &tableElements = mpaPeriod->tableElements;
    const auto mpaCompressionMethod = mpaSettings.mpaCompressionMethod;
    const auto epsilonPower = mpaSettings.epsilonPower;

    if (longestPeriod < mpaSettings.minSkipReference) {
        return;
    }

    const double epsilon = pow(10, epsilonPower);
    uint64_t iteration = 1;
    uint64_t absIteration = 0;

    const size_t levels = tablePeriod.size();
    auto periodCount = std::vector<uint64_t>(levels, 0);
    auto currentPA = std::vector<std::unique_ptr<LightPA::Generator> >();
    currentPA.resize(levels);


    std::vector<LightPA> *mainReferenceMPAPtr = nullptr;

    const uint64_t size = iterationToCompTableIndex(mpaCompressionMethod, *mpaPeriod, pulledMPACompressor,
                                                    longestPeriod + 1);

    std::for_each(std::execution::par_unseq, table.begin(), table.end(), [](auto &v) {
        v.clear();
    });
    table.reserve(size);

    while (iteration <= longestPeriod) {
        if (absIteration % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) return;

        func(iteration, static_cast<double>(iteration) / static_cast<double>(longestPeriod));
        const uint64_t pulledTableIndex = iterationToPulledTableIndex(*mpaPeriod, iteration);
        const bool independent = ArrayCompressor::isIndependent(pulledMPACompressor, pulledTableIndex);
        const uint64_t containedIndex = ArrayCompressor::containedIndex(pulledMPACompressor, pulledTableIndex + 1);

        if (const ArrayCompressionTool *containedTool = containedIndex == UINT64_MAX
                                                            ? nullptr
                                                            : &pulledMPACompressor[containedIndex];
            containedTool != nullptr &&
            containedTool->start == pulledTableIndex + 1 &&
            mainReferenceMPAPtr != nullptr
        ) {
            const uint64_t level = binarySearch(tableElements, containedTool->end - containedTool->start + 2);
            //count itself and periodic point, +2

            const uint64_t compTableIndex = iterationToCompTableIndex(mpaCompressionMethod, *mpaPeriod,
                                                                      pulledMPACompressor, iteration);
            allocateTableSize(table, compTableIndex, levels);


            std::vector<LightPA> &mainReferenceMPA = *mainReferenceMPAPtr;
            auto &pa = table[compTableIndex];
            const LightPA &mainReferencePA = mainReferenceMPA[level];
            const uint64_t skip = mainReferencePA.skip;

            for (uint64_t i = 0; i < levels; ++i) {
                if (i <= level) {
                    pa.push_back(mainReferenceMPA[i]);
                    uint64_t count = skip;
                    for (uint64_t j = level; j > i; --j) {
                        count %= tablePeriod[j - 1];
                    }

                    periodCount[i] = count;
                } else {
                    if (currentPA[i] == nullptr) {
                        //its count is zero but has no element? -> Artificial PA
                        auto generator = LightPA::Generator::create(*reference, epsilon, dcMax, iteration);
                        generator->merge(mainReferencePA);
                        currentPA[i] = std::move(generator);
                    } else {
                        currentPA[i]->merge(mainReferencePA);
                    }
                    periodCount[i] += skip;
                }
            }

            iteration += skip;
        }
        bool resetLowerLevel = false;

        for (uint64_t level = levels; level > 0; --level) {
            const uint64_t i = level - 1;
            if (periodCount[i] == 0 && independent) {
                currentPA[i] = LightPA::Generator::create(*reference, epsilon, dcMax, iteration);
            }

            if (currentPA[i] != nullptr && periodCount[i] + RFF::Approximation::REQUIRED_PERTURBATION <
                tablePeriod[i]) {
                currentPA[i]->step();
            }


            periodCount[i]++;

            if (periodCount[i] == tablePeriod[i]) {
                if (const LightPA::Generator *currentLevel = currentPA[i].get();
                    currentLevel != nullptr &&
                    currentLevel->getSkip() == tablePeriod[i] - RFF::Approximation::REQUIRED_PERTURBATION
                ) {
                    //If the skip count is lower than its current period - perturbation,
                    //it can be replaced to several lower-level PA.

                    const uint64_t compTableIndex = iterationToCompTableIndex(
                        mpaCompressionMethod, *mpaPeriod, pulledMPACompressor, currentLevel->getStart());
                    allocateTableSize(table, compTableIndex, levels);

                    auto &pa = table[compTableIndex];
                    pa.push_back(currentLevel->build());

                    if (compTableIndex == 0) {
                        mainReferenceMPAPtr = &pa;
                    }
                }
                //Stop all lower level iteration for efficiency
                //because it is too hard to skipping to next part of the periodic point
                currentPA[i] = nullptr;
                resetLowerLevel = true;
            }

            if (resetLowerLevel) {
                periodCount[i] = 0;
            }
        }
        ++iteration;
        ++absIteration;
    }
}


void LightMPATable::initTable(const LightMandelbrotReference &reference) {
    const auto &referencePeriod = reference.period;
    const uint64_t longestPeriod = reference.longestPeriod();

    if (const int minSkip = mpaSettings.minSkipReference;
        longestPeriod < minSkip) {
        this->pulledMPACompressor = std::vector<ArrayCompressionTool>();
        return;
    }

    const MPACompressionMethod compressionMethod = mpaSettings.mpaCompressionMethod;
    this->mpaPeriod = MPAPeriod::create(referencePeriod, mpaSettings);
    this->pulledMPACompressor = compressionMethod == MPACompressionMethod::STRONGEST
                                    ? createPulledMPACompressor(reference.compressor)
                                    : std::vector<ArrayCompressionTool>();
}


std::vector<ArrayCompressionTool> LightMPATable::createPulledMPACompressor(
    const std::vector<ArrayCompressionTool> &referenceCompressor) const {
    std::vector<ArrayCompressionTool> mpaTools;
    auto &tablePeriod = mpaPeriod->tablePeriod;
    auto &tablePeriodElements = mpaPeriod->tableElements;
    auto &isArtificial = mpaPeriod->isArtificial;

    for (ArrayCompressionTool tool: referenceCompressor) {
        const uint64_t start = tool.start;
        const uint64_t length = tool.range();
        const uint64_t index = binarySearch(tablePeriod, length + 1);

        // Check if the reference compressor is same as period.
        // However, The Computer doesn't know whether the compressor's length came from skipping to the periodic point, or being cut off in the middle.
        // So, Do check tableIndex too.

        if (const uint64_t tableIndex = iterationToPulledTableIndex(*mpaPeriod, start);
            index != UINT64_MAX &&
            tableIndex != UINT64_MAX &&
            !isArtificial[index]
        ) {
            const uint64_t periodElements = tablePeriodElements[index];
            mpaTools.emplace_back(1, tableIndex + 1, tableIndex + periodElements - 1);
        }
    }
    return mpaTools;
}

uint64_t LightMPATable::binarySearch(const std::vector<uint64_t> &arr, const uint64_t key) {

    if (arr.empty()) {
        return UINT64_MAX;
    }

    uint64_t low = 0;
    uint64_t high = arr.size() - 1;

    while (low <= high) {
        const uint64_t mid = (low + high) >> 1;
        if (const uint64_t value = arr[mid];
            value < key
        ) {
            low = mid + 1;
        } else if (value > key) {
            if (mid == 0) {
                return UINT64_MAX;
            }
            high = mid - 1;
        } else return mid;
    }
    return UINT64_MAX;
}


void LightMPATable::allocateTableSize(std::vector<std::vector<LightPA> > &table, const uint64_t index,
                                      const uint64_t levels) {
    while (table.size() < index) {
        table.emplace_back();
    }
    if (table.size() == index) {
        table.emplace_back();
        table.back().reserve(levels);
    }
    table[index].reserve(levels);
}


uint64_t LightMPATable::iterationToCompTableIndex(const MPACompressionMethod &mpaCompressionMethod,
                                                  const MPAPeriod &mpaPeriod,
                                                  const std::vector<ArrayCompressionTool> &pulledMPACompressor,
                                                  const uint64_t iteration) {
    switch (mpaCompressionMethod) {
            using enum MPACompressionMethod;
        case NO_COMPRESSION: return iteration;
        case LITTLE_COMPRESSION: return iterationToPulledTableIndex(mpaPeriod, iteration);
        case STRONGEST: {
            const uint64_t index = iterationToPulledTableIndex(mpaPeriod, iteration);
            return index == UINT64_MAX ? UINT64_MAX : ArrayCompressor::compress(pulledMPACompressor, index);
        }
        default: return iteration;
    }
}

uint64_t LightMPATable::iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
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
    const auto &tablePeriodElements = mpaPeriod.tableElements;

    uint64_t index = 0;
    uint64_t remainder = iteration;

    for (uint64_t i = tablePeriod.size(); i > 0; --i) {
        if (remainder < tablePeriod[i - 1]) {
            continue;
        }
        if (i < tablePeriod.size() && remainder + tablePeriod[0] - RFF::Approximation::REQUIRED_PERTURBATION +
            1 > tablePeriod[i]) {
            return UINT64_MAX;
            //Insufficient length, ("Pulled Table Index" must be skipped for at least "shortest period")
        }


        index += remainder / tablePeriod[i - 1] * tablePeriodElements[i - 1];
        remainder %= tablePeriod[i - 1];
    }
    return remainder == 1 ? index : UINT64_MAX;
}


LightPA *LightMPATable::lookup(const uint64_t refIteration, const double dzr, const double dzi) {
    if (refIteration == 0 || mpaPeriod == nullptr) {
        return nullptr;
    }
    const uint64_t index = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod, pulledMPACompressor,
                                                     refIteration);

    if (index >= table.size()) {
        return nullptr;
    }

    std::vector<LightPA> &table = this->table[index];
    if (table.empty()) {
        return nullptr;
    }

    const double r = ApproxMath::hypotApproximate(dzr, dzi);

    switch (mpaSettings.mpaSelectionMethod) {
            using enum MPASelectionMethod;
        case LOWEST: {
            LightPA *pa = nullptr;

            for (LightPA &test: table) {
                if (test.isValid(r)) {
                    pa = &test;
                } else return pa;
            }
            return pa;
        }
        case HIGHEST: {
            LightPA &pa = table.front();
            //This table cannot be empty because the pre-processing is done.

            if (!pa.isValid(r)) {
                return nullptr;
            }

            for (uint64_t j = table.size(); j > 0; --j) {
                LightPA &test = table[j - 1];
                if (test.isValid(r)) {
                    return &test;
                }
            }

            return &pa;
        }
        default: return nullptr;
    }
}
