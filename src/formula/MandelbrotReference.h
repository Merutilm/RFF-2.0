//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../calc/fp_complex.h"
#include "../mrthy/ArrayCompressionTool.h"

namespace merutilm::rff2 {
    struct MandelbrotReference {
        const fp_complex center;
        const std::vector<ArrayCompressionTool> compressor;
        const std::vector<uint64_t> period;
        const fp_complex lastReference;
        const fp_complex fpgBn;

        MandelbrotReference(fp_complex &&center, std::vector<ArrayCompressionTool> &&compressor,
        std::vector<uint64_t> &&period, fp_complex &&lastReference, fp_complex &&fpgBn) : center(std::move(center)),
                                                    compressor(std::move(compressor)),
                                                    period(std::move(period)),
                                                    lastReference(std::move(lastReference)),
                                                    fpgBn(std::move(fpgBn)){}

        virtual ~MandelbrotReference() = default;

        /**
         * @return the array length of reference.
         */
        virtual size_t length() const = 0;

        virtual uint64_t longestPeriod() const = 0;
    };
}