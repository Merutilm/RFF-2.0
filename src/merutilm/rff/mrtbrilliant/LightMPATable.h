//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <vector>

#include "LightPA.h"
#include "MPAPeriod.h"
#include "MPATable.h"
#include "../formula/LightMandelbrotReference.h"
#include "../settings/MPASettings.h"

class LightMPATable final : public MPATable{
    const LightMandelbrotReference *reference;
    std::vector<std::vector<LightPA>> table = std::vector<std::vector<LightPA>>();

public:


    explicit LightMPATable(const ParallelRenderState &state, const LightMandelbrotReference *reference,
                  const MPASettings *mpaSettings, double dcMax, std::vector<std::vector<LightPA>> &&previousAllocatedTable,
                  std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);


    ~LightMPATable() = default;

    LightMPATable(const LightMPATable &) = delete;

    LightMPATable &operator=(const LightMPATable &) = delete;

    LightMPATable(LightMPATable &&) noexcept = default;

    LightMPATable &operator=(LightMPATable &&) noexcept = delete;

    void generateTable(const ParallelRenderState &state, double dcMax,
                       std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

    void initTable(const LightMandelbrotReference &reference);

    std::vector<std::vector<LightPA>> &&extractVector() {
        return std::move(table);
    };

    LightPA *lookup(uint64_t refIteration, double dzr, double dzi);

private:
    static void allocateTableSize(std::vector<std::vector<LightPA>> &table, uint64_t index, uint64_t levels);

};
