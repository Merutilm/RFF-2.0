//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <vector>

#include "LightPA.h"
#include "MPAPeriod.h"
#include "../formula/LightMandelbrotReference.h"
#include "../settings/MPASettings.h"

class LightMPATable {
    const LightMandelbrotReference *reference;
    const MPASettings mpaSettings;
    std::vector<ArrayCompressionTool> pulledMPACompressor = std::vector<ArrayCompressionTool>();
    std::unique_ptr<MPAPeriod> mpaPeriod = nullptr;
    std::vector<std::vector<LightPA>> table = std::vector<std::vector<LightPA>>();

public:


    explicit LightMPATable(const ParallelRenderState &state, const LightMandelbrotReference *reference,
                  const MPASettings *mpaSettings, double dcMax, std::vector<std::vector<LightPA>> &&previousAllocatedTable,
                  std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);


    ~LightMPATable() = default;

    LightMPATable(const LightMPATable &) = delete;

    LightMPATable &operator=(const LightMPATable &) = delete;

    LightMPATable(LightMPATable &&) noexcept;

    LightMPATable &operator=(LightMPATable &&) noexcept = delete;

    void generateTable(const ParallelRenderState &state, double dcMax,
                       std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

    void initTable(const LightMandelbrotReference &reference);

private:
    std::vector<ArrayCompressionTool> createPulledMPACompressor(
        const std::vector<ArrayCompressionTool> &referenceCompressor) const;

    static uint64_t binarySearch(const std::vector<uint64_t> &arr, uint64_t key);

public:
    static void allocateTableSize(std::vector<std::vector<LightPA>> &table, uint64_t index, uint64_t levels);

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

    std::vector<std::vector<LightPA>> &&extractVector() {
        return std::move(table);
    };

    LightPA *lookup(uint64_t refIteration, double dzr, double dzi);
};
