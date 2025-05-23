//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "DeepPA.h"
#include "MPATable.h"
#include "../calc/double_exp_math.h"
#include "../formula/DeepMandelbrotReference.h"

class DeepMPATable final : public MPATable{
    const DeepMandelbrotReference *reference;
    std::vector<std::vector<DeepPA>> &table;

public:


    explicit DeepMPATable(const ParallelRenderState &state, const DeepMandelbrotReference *reference,
                  const MPASettings *mpaSettings, const double_exp &dcMax, std::vector<std::vector<DeepPA>> &deepTableRef,
                  std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);


    ~DeepMPATable() override = default;

    DeepMPATable(const DeepMPATable &) = delete;

    DeepMPATable &operator=(const DeepMPATable &) = delete;

    DeepMPATable(DeepMPATable &&) noexcept = delete;

    DeepMPATable &operator=(DeepMPATable &&) noexcept = delete;

    void generateTable(const ParallelRenderState &state, const double_exp &dcMax,
                       std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) const;

    void initTable(const DeepMandelbrotReference &reference);

    std::vector<std::vector<DeepPA>> &getVector() const {
        return table;
    }

    DeepPA *lookup(uint64_t refIteration, const double_exp &dzr, const double_exp &dzi, std::array<double_exp, 4> &temps) const;

    int getLength() override;

private:
    static void allocateTableSize(std::vector<std::vector<DeepPA>> &table, uint64_t index, uint64_t levels);
};

// DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
// DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
// DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
// DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
// DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE


inline DeepPA *DeepMPATable::lookup(const uint64_t refIteration, const double_exp &dzr, const double_exp &dzi, std::array<double_exp, 4> &temps) const {

    if (refIteration == 0 || mpaPeriod == nullptr) {
        return nullptr;
    }

    const uint64_t index = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod, pulledMPACompressor,
                                                     refIteration);

    if (index >= table.size()) {
        return nullptr;
    }

    std::vector<DeepPA> &table = this->table[index];
    if (table.empty()) {
        return nullptr;
    }

    dex_trigonometric::hypot_approx(&temps[0], dzr, dzi);

    switch (mpaSettings.mpaSelectionMethod) {
        using enum MPASelectionMethod;
        case LOWEST: {
            DeepPA *pa = nullptr;

            for (DeepPA &test: table) {
                if (test.isValid(&temps[1], temps[0])) {
                    pa = &test;
                } else return pa;
            }
            return pa;
        }
        case HIGHEST: {
            DeepPA &pa = table.front();
            //This table cannot be empty because the pre-processing is done.

            if (!pa.isValid(&temps[1], temps[0])) {
                return nullptr;
            }

            for (uint64_t j = table.size(); j > 0; --j) {
                DeepPA &test = table[j - 1];
                if (test.isValid(&temps[1], temps[0])) {
                    return &test;
                }
            }

            return &pa;
        }
        default: return nullptr;
    }
}

inline int DeepMPATable::getLength() {
    return table.size();
}