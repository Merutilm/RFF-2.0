//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <optional>

#include "../precision/Center.h"
#include <vector>

#include "../approx/ArrayCompressor.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/CalculationSettings.h"

struct LightMandelbrotReference {
    const Center center;
    const std::vector<double> refReal;
    const std::vector<double> refImag;
    const std::vector<ArrayCompressionTool> compressor;
    const std::vector<uint64_t> period;
    const Center lastReference;
    const Center fpgBn;


    LightMandelbrotReference(Center &&center, std::vector<double> &&refReal,
                             std::vector<double> &&refImag, std::vector<ArrayCompressionTool> &&compressor,
                             std::vector<uint64_t> &&period, Center &&lastReference, Center &&fpgBn);

    static std::unique_ptr<LightMandelbrotReference> createReference(const ParallelRenderState &state,
                                                                     const CalculationSettings &calc, int exp10,
                                                                     uint64_t initialPeriod, double dcMax, bool
                                                                     strictFPGBn,
                                                                     std::function<void(uint64_t)> &&
                                                                     actionPerRefCalcIteration);

    double real(uint64_t refIteration) const;

    double imag(uint64_t refIteration) const;

    /**
     * @return the array length of reference.
     */
    size_t length() const;

    uint64_t longestPeriod() const;
};
