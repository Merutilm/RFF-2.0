//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../calc/fp_complex.h"
#include "../mrthy/ArrayCompressionTool.h"

namespace merutilm::rff {
    struct MandelbrotReference {
        const fp_complex center;
        const std::vector<ArrayCompressionTool> compressor;
        const std::vector<uint64_t> period;
        const fp_complex lastReference;
        const fp_complex fpgBn;

        MandelbrotReference(fp_complex &&center, std::vector<ArrayCompressionTool> &&compressor,
                            std::vector<uint64_t> &&period, fp_complex &&lastReference, fp_complex &&fpgBn);

        virtual ~MandelbrotReference() = default;

        /**
         * @return the array length of reference.
         */
        virtual size_t length() const = 0;

        virtual uint64_t longestPeriod() const = 0;
    };
}