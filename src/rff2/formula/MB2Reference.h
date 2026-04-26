//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../calc/fixed_point_complex.hpp"
#include "../mrthy/ArrayCompressionTool.h"
#include "Reference.hpp"

namespace merutilm::rff2 {
    struct MB2Reference : public Reference{
        const fixed_point_complex_i1 center;
        const std::vector<ArrayCompressionTool> compressor;
        const std::vector<uint64_t> period;
        const fixed_point_complex fpgReference;
        const fixed_point_complex fpgBn;

        MB2Reference(fixed_point_complex_i1 &&center, std::vector<ArrayCompressionTool> &&compressor,
                            std::vector<uint64_t> &&period, fixed_point_complex &&fpgReference, fixed_point_complex &&fpgBn) :
            center(std::move(center)), compressor(std::move(compressor)), period(std::move(period)),
            fpgReference(std::move(fpgReference)), fpgBn(std::move(fpgBn)) {}

        [[nodiscard]] virtual uint64_t longestPeriod() const = 0;
    };
}