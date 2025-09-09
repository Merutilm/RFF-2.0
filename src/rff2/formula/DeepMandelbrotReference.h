//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "MandelbrotReference.h"
#include "../calc/fp_complex.h"
#include "../parallel/ParallelRenderState.h"
#include "../attr/FractalAttribute.h"

struct ArrayCompressionTool;

namespace merutilm::rff2 {
    struct DeepMandelbrotReference final : public MandelbrotReference{
        const std::vector<dex> refReal;
        const std::vector<dex> refImag;


        DeepMandelbrotReference(fp_complex &&center, std::vector<dex> &&refReal,
                                 std::vector<dex> &&refImag, std::vector<ArrayCompressionTool> &&compressor,
                                 std::vector<uint64_t> &&period, fp_complex &&fpgReference, fp_complex &&fpgBn);

        static std::unique_ptr<DeepMandelbrotReference> createReference(const ParallelRenderState &state,
                                                                         const FractalAttribute &calc, int exp10,
                                                                         uint64_t initialPeriod, dex dcMax, bool
                                                                         strictFPG,
                                                                         std::function<void(uint64_t)> &&
                                                                         actionPerRefCalcIteration);



        dex real(uint64_t refIteration) const;

        dex imag(uint64_t refIteration) const;

        size_t length() const override;

        uint64_t longestPeriod() const override;

    };
}