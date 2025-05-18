//
// Created by Merutilm on 2025-05-18.
//

#include "MPATable.h"

#include "ArrayCompressor.h"
#include "../ui/RFF.h"


MPATable::MPATable(const MPASettings *mpaSettings) : mpaSettings(*mpaSettings){

}


std::vector<ArrayCompressionTool> MPATable::createPulledMPACompressor(
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

uint64_t MPATable::binarySearch(const std::vector<uint64_t> &arr, const uint64_t key) {

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


uint64_t MPATable::iterationToCompTableIndex(const MPACompressionMethod &mpaCompressionMethod,
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

uint64_t MPATable::iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
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
