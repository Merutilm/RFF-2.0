//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "ArrayCompressionTool.h"
#include "MPAPeriod.h"
#include "../settings/MPACompressionMethod.h"

struct MPATable {

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
