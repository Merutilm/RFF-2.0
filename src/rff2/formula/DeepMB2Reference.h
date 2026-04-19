//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../calc/fp_complex.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FractalSettings.h"
#include "MB2Reference.h"

struct ArrayCompressionTool;

namespace merutilm::rff2 {
    struct DeepMB2Reference final : public MB2Reference {
        const std::vector<dex> refReal;
        const std::vector<dex> refImag;


        explicit DeepMB2Reference(fp_complex &&center, std::vector<dex> &&refReal, std::vector<dex> &&refImag,
                                         std::vector<ArrayCompressionTool> &&compressor, std::vector<uint64_t> &&period,
                                         fp_complex &&fpgReference, fp_complex &&fpgBn);

        static CreationResult createReference(const ParallelRenderState &state, const FractalSettings &calc, int exp10,
                                              uint64_t initialPeriod, dex dcMax, bool strictFPG,
                                              std::function<void(uint64_t)> &&actionPerRefCalcIteration,  std::unique_ptr<DeepMB2Reference> *result);


        [[nodiscard]] dex real(uint64_t refIteration) const;

        [[nodiscard]] dex imag(uint64_t refIteration) const;

        [[nodiscard]] size_t length() const override;

        [[nodiscard]] uint64_t longestPeriod() const override;
    };
} // namespace merutilm::rff2
