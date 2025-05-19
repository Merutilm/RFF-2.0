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
