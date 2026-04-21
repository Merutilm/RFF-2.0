//
// Created by Merutilm on 2025-05-09.
//

#pragma once

#include <vector>
#include "../calc/fp_complex.h"

#include "../parallel/ParallelRenderState.h"
#include "../settings/FractalSettings.h"
#include "MB2Reference.h"

namespace merutilm::rff2 {
    struct LightMB2Reference final : public MB2Reference {

        const std::vector<double> refReal;
        const std::vector<double> refImag;


        explicit LightMB2Reference(fp_complex &&center, std::vector<double> &&refReal,
                                          std::vector<double> &&refImag, std::vector<ArrayCompressionTool> &&compressor,
                                          std::vector<uint64_t> &&period, fp_complex &&fpgReference,
                                          fp_complex &&fpgBn);

        static CreationResult generateReference(const ParallelRenderState &state, const FractalSettings &calc, int exp10,
                        uint64_t refInitialCapacity, uint64_t fixedPeriod, double dcMax, bool strictFPG,
                        std::function<void(uint64_t)> &&actionPerRefCalcIteration, std::unique_ptr<LightMB2Reference> *result);

        [[nodiscard]] double real(uint64_t refIteration) const;

        [[nodiscard]] double imag(uint64_t refIteration) const;

        [[nodiscard]] size_t length() const override;

        [[nodiscard]] uint64_t longestPeriod() const override;
    };
} // namespace merutilm::rff2
