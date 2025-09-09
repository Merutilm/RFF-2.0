//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <optional>

#include "../calc/fp_complex.h"
#include <vector>

#include "MandelbrotReference.h"
#include "../mrthy/ArrayCompressor.h"
#include "../parallel/ParallelRenderState.h"
#include "../attr/FractalAttribute.h"

namespace merutilm::rff2 {
    struct LightMandelbrotReference final : public MandelbrotReference{

        const std::vector<double> refReal;
        const std::vector<double> refImag;


        explicit LightMandelbrotReference(fp_complex &&center, std::vector<double> &&refReal,
                                 std::vector<double> &&refImag, std::vector<ArrayCompressionTool> &&compressor,
                                 std::vector<uint64_t> &&period, fp_complex &&fpgReference, fp_complex &&fpgBn);

        static std::unique_ptr<LightMandelbrotReference> createReference(const ParallelRenderState &state,
                                                                         const FractalAttribute &calc, int exp10,
                                                                         uint64_t initialPeriod, double dcMax, bool
                                                                         strictFPG,
                                                                         std::function<void(uint64_t)> &&
                                                                         actionPerRefCalcIteration);

        double real(uint64_t refIteration) const;

        double imag(uint64_t refIteration) const;

        size_t length() const override;

        uint64_t longestPeriod() const override;
    };
}