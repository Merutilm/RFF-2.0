//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include <vector>

#include "LightPA.h"
#include "MPATable.h"
#include "../calc/rff_math.h"
#include "../formula/LightMandelbrotReference.h"
#include "../attr/FrtMPAAttribute.h"

namespace merutilm::rff2 {
    class LightMPATable final : public MPATable<LightMandelbrotReference, double>{

    public:


        explicit LightMPATable(const ParallelRenderState &state, const LightMandelbrotReference &reference,
                      const FrtMPAAttribute *mpaSettings, double dcMax, ApproxTableCache &tableRef,
                      std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) : MPATable(state, reference, mpaSettings, dcMax, tableRef, std::move(actionPerCreatingTableIteration)) {

        };


        ~LightMPATable() override = default;

        LightMPATable(const LightMPATable &) = delete;

        LightMPATable &operator=(const LightMPATable &) = delete;

        LightMPATable(LightMPATable &&) noexcept = delete;

        LightMPATable &operator=(LightMPATable &&) noexcept = delete;

        LightPA *lookup(uint64_t refIteration, double dzr, double dzi) const;

        size_t getLength() override;

    };

    // DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE
    // DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE
    // DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE
    // DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE
    // DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE  DEFINITION OF LIGHT MPA TABLE

    inline LightPA *LightMPATable::lookup(const uint64_t refIteration, const double dzr, const double dzi) const {
        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }
        const uint64_t index = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod, pulledMPACompressor,
                                                         refIteration);

        if (index >= tableRef.lightTable.size()) {
            return nullptr;
        }

        std::vector<LightPA> &table = tableRef.lightTable[index];
        if (table.empty()) {
            return nullptr;
        }

        const double r = rff_math::hypot_approx(dzr, dzi);

        switch (mpaSettings.mpaSelectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                LightPA *pa = nullptr;

                for (LightPA &test: table) {
                    if (test.isValid(r)) {
                        pa = &test;
                    } else return pa;
                }
                return pa;
            }
            case HIGHEST: {
                LightPA &pa = table.front();
                //This table cannot be empty because the pre-processing is done.

                if (!pa.isValid(r)) {
                    return nullptr;
                }

                for (uint64_t j = table.size(); j > 0; --j) {
                    LightPA &test = table[j - 1];
                    if (test.isValid(r)) {
                        return &test;
                    }
                }

                return &pa;
            }
            default: return nullptr;
        }
    }

    inline size_t LightMPATable::getLength() {
        return tableRef.lightTable.size();
    }
}