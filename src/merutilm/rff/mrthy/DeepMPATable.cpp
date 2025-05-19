//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMPATable.h"

#include <execution>

#include "ArrayCompressor.h"
#include "../calc/double_exp.h"


DeepMPATable::DeepMPATable(const ParallelRenderState &state, const DeepMandelbrotReference *reference,
                           const MPASettings *mpaSettings, const double_exp &dcMax,
                           std::vector<std::vector<DeepPA> > &&previousAllocatedTable,
                           std::function<void(uint64_t, double)> &&
                           actionPerCreatingTableIteration) : MPATable(mpaSettings), reference(reference),
                                                              table(std::move(previousAllocatedTable)) {
    if (reference != nullptr) {
        initTable(*reference);
        generateTable(state, dcMax, std::move(actionPerCreatingTableIteration));
    }
}





void DeepMPATable::generateTable(const ParallelRenderState &state, const double_exp &dcMax,
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
    auto currentPA = std::vector<std::unique_ptr<DeepPA::Generator> >();
    currentPA.resize(levels);


    std::vector<DeepPA> *mainReferenceMPAPtr = nullptr;

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


            std::vector<DeepPA> &mainReferenceMPA = *mainReferenceMPAPtr;
            auto &pa = table[compTableIndex];
            const DeepPA &mainReferencePA = mainReferenceMPA[level];
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
                        auto generator = DeepPA::Generator::create(*reference, epsilon, dcMax, iteration);
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
                currentPA[i] = DeepPA::Generator::create(*reference, epsilon, dcMax, iteration);
            }

            if (currentPA[i] != nullptr && periodCount[i] + REQUIRED_PERTURBATION <
                tablePeriod[i]) {
                currentPA[i]->step();
            }


            periodCount[i]++;

            if (periodCount[i] == tablePeriod[i]) {
                if (const DeepPA::Generator *currentLevel = currentPA[i].get();
                    currentLevel != nullptr &&
                    currentLevel->getSkip() == tablePeriod[i] - REQUIRED_PERTURBATION
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


void DeepMPATable::initTable(const DeepMandelbrotReference &reference) {
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


void DeepMPATable::allocateTableSize(std::vector<std::vector<DeepPA> > &table, const uint64_t index,
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
