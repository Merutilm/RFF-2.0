//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "DeepPA.h"
#include "MPATable.h"
#include "../calc/double_exp_math.h"
#include "../formula/DeepMandelbrotReference.h"

namespace merutilm::rff2 {
    class DeepMPATable final : public MPATable<DeepMandelbrotReference, dex>{

    public:


        explicit DeepMPATable(const ParallelRenderState &state, const DeepMandelbrotReference &reference,
                      const FrtMPAAttribute *mpaSettings, const dex &dcMax, ApproxTableCache &tableRef,
                      std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) : MPATable(state, reference, mpaSettings, dcMax, tableRef, std::move(actionPerCreatingTableIteration)) {

        }


        ~DeepMPATable() override = default;

        DeepMPATable(const DeepMPATable &) = delete;

        DeepMPATable &operator=(const DeepMPATable &) = delete;

        DeepMPATable(DeepMPATable &&) noexcept = delete;

        DeepMPATable &operator=(DeepMPATable &&) noexcept = delete;

        DeepPA *lookup(uint64_t refIteration, const dex &dzr, const dex &dzi, std::array<dex, 4> &temps) const;

        size_t getLength() override;
    };

    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE


    inline DeepPA *DeepMPATable::lookup(const uint64_t refIteration, const dex &dzr, const dex &dzi, std::array<dex, 4> &temps) const {

        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }

        const uint64_t index = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod, pulledMPACompressor,
                                                         refIteration);

        if (index >= tableRef.deepTable.size()) {
            return nullptr;
        }

        std::vector<DeepPA> &table = this->tableRef.deepTable[index];
        if (table.empty()) {
            return nullptr;
        }

        dex_trigonometric::hypot_approx(&temps[0], dzr, dzi);

        switch (mpaSettings.mpaSelectionMethod) {
            using enum FrtMPASelectionMethod;
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

    inline size_t DeepMPATable::getLength() {
        return tableRef.deepTable.size();
    }
}