//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "DeepPA.h"
#include "MPATable.h"
#include "../formula/DeepMandelbrotReference.h"

class DeepMPATable final : public MPATable{
    const DeepMandelbrotReference *reference;
    std::vector<std::vector<DeepPA>> table = std::vector<std::vector<DeepPA>>();

public:


    explicit DeepMPATable(const ParallelRenderState &state, const DeepMandelbrotReference *reference,
                  const MPASettings *mpaSettings, const double_exp &dcMax, std::vector<std::vector<DeepPA>> &&previousAllocatedTable,
                  std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);


    ~DeepMPATable() = default;

    DeepMPATable(const DeepMPATable &) = delete;

    DeepMPATable &operator=(const DeepMPATable &) = delete;

    DeepMPATable(DeepMPATable &&) noexcept = default;

    DeepMPATable &operator=(DeepMPATable &&) noexcept = delete;

    void generateTable(const ParallelRenderState &state, const double_exp &dcMax,
                       std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

    void initTable(const DeepMandelbrotReference &reference);

    std::vector<std::vector<DeepPA>> &&extractVector() {
        return std::move(table);
    };

    DeepPA *lookup(uint64_t refIteration, const double_exp &dzr, const double_exp &dzi);
    
private:
    static void allocateTableSize(std::vector<std::vector<DeepPA>> &table, uint64_t index, uint64_t levels);
};
