//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "DeepPA.h"
#include "MPATable.h"
#include "../calc/double_exp_math.h"
#include "../formula/DeepMB2Reference.h"

namespace merutilm::rff2 {
    class DeepMPATable final : public MPATable<DeepMB2Reference, dex, DeepPA>{

    public:


        explicit DeepMPATable(const ParallelRenderState &state, const DeepMB2Reference &reference,
                      const FrtMPASettings *mpaSettings, const dex dcMax,
                      std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration) : MPATable(state, reference, mpaSettings, dcMax, std::move(actionPerCreatingTableIteration)) {

        }


        ~DeepMPATable() override = default;

        DeepMPATable(const DeepMPATable &) = delete;

        DeepMPATable &operator=(const DeepMPATable &) = delete;

        DeepMPATable(DeepMPATable &&) noexcept = delete;

        DeepMPATable &operator=(DeepMPATable &&) noexcept = delete;

        DeepPA *lookup(uint64_t refIteration, dex dzr, dex dzi) const;

        size_t getLength() override;
    };

    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE
    // DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE  DEFINITION OF DEEP MPA TABLE


    inline DeepPA *DeepMPATable::lookup(const uint64_t refIteration, const dex dzr, const dex dzi) const {

        if (refIteration == 0 || mpaPeriod == nullptr) {
            return nullptr;
        }

        const uint64_t index = iterationToCompTableIndex(mpaSettings.mpaCompressionMethod, *mpaPeriod, pulledMPACompressor,
                                                         refIteration);

        if (index >= tableManager->mpaTable->size()) {
            return nullptr;
        }

        std::pmr::vector<DeepPA> &table = (*this->tableManager->mpaTable)[index];
        if (table.empty()) {
            return nullptr;
        }

        const dex r = dex_trig::hypot_approx(dzr, dzi);

        switch (mpaSettings.mpaSelectionMethod) {
            using enum FrtMPASelectionMethod;
            case LOWEST: {
                DeepPA *pa = nullptr;

                for (DeepPA &test: table) {
                    if (test.isValid(r)) {
                        pa = &test;
                    } else return pa;
                }
                return pa;
            }
            case HIGHEST: {
                DeepPA &pa = table.front();
                //This table cannot be empty because the pre-processing is done.

                if (!pa.isValid(r)) {
                    return nullptr;
                }

                for (uint64_t j = table.size(); j > 0; --j) {
                    DeepPA &test = table[j - 1];
                    if (test.isValid(r)) {
                        return &test;
                    }
                }

                return &pa;
            }
            default: return nullptr;
        }
    }

    inline size_t DeepMPATable::getLength() {
        return tableManager && tableManager->mpaTable ? tableManager->mpaTable->size() : 0;
    }
}