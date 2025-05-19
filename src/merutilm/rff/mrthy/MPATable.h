//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "ArrayCompressionTool.h"
#include "ArrayCompressor.h"
#include "MPAPeriod.h"
#include "../settings/MPACompressionMethod.h"
#include "../ui/RFF.h"

struct MPATable {

    static constexpr int REQUIRED_PERTURBATION = 2;

    const MPASettings mpaSettings;
    std::vector<ArrayCompressionTool> pulledMPACompressor = std::vector<ArrayCompressionTool>();
    std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;

    explicit MPATable(const MPASettings *mpaSettings);

    virtual ~MPATable() = default;

protected:
    std::vector<ArrayCompressionTool> createPulledMPACompressor(
        const std::vector<ArrayCompressionTool> &referenceCompressor) const;

    static uint64_t binarySearch(const std::vector<uint64_t> &arr, uint64_t key);

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
    static uint64_t iterationToCompTableIndex(const MPACompressionMethod &mpaCompressionMethod, const MPAPeriod &mpaPeriod,
                                              const std::vector<ArrayCompressionTool> &pulledMPACompressor,
                                              uint64_t iteration);

};

// DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE
// DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE
// DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE
// DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE
// DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE  DEFINITION OF MPA TABLE

inline uint64_t MPATable::iterationToCompTableIndex(const MPACompressionMethod &mpaCompressionMethod,
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

inline uint64_t MPATable::iterationToPulledTableIndex(const MPAPeriod &mpaPeriod, const uint64_t iteration) {
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
    const auto &tablePeriodElements = mpaPeriod.tableElements;

    uint64_t index = 0;
    uint64_t remainder = iteration;

    for (uint64_t i = tablePeriod.size(); i > 0; --i) {
        if (remainder < tablePeriod[i - 1]) {
            continue;
        }
        if (i < tablePeriod.size() && remainder + tablePeriod[0] - REQUIRED_PERTURBATION +
            1 > tablePeriod[i]) {
            return UINT64_MAX;
            //Insufficient length, ("Pulled Table Index" must be skipped for at least "shortest period")
        }


        index += remainder / tablePeriod[i - 1] * tablePeriodElements[i - 1];
        remainder %= tablePeriod[i - 1];
    }
    return remainder == 1 ? index : UINT64_MAX;
}
