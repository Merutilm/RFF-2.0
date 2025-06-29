//
// Created by Merutilm on 2025-05-22.
//

#pragma once
#include <memory>

#include "PA.h"

namespace merutilm::rff2 {
    struct PAGenerator {

        uint64_t start;
        uint64_t skip;
        const std::vector<ArrayCompressionTool> &compressors;
        double epsilon;

        explicit PAGenerator(uint64_t start, uint64_t skip, const std::vector<ArrayCompressionTool> &compressors, double epsilon);

        uint64_t getStart() const;

        uint64_t getSkip() const;

        virtual ~PAGenerator() = default;

        virtual void merge(const PA &pa) = 0;

        virtual void step() = 0;

    };

    inline PAGenerator::PAGenerator(const uint64_t start, const uint64_t skip, const std::vector<ArrayCompressionTool> &compressors, const double epsilon) : start(
        start), skip(skip), compressors(compressors), epsilon(epsilon) {
    }

    inline uint64_t PAGenerator::getStart() const {
        return start;
    }

    inline uint64_t PAGenerator::getSkip() const {
        return skip;
    }
}