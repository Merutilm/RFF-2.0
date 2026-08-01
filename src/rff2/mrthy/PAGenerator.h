//
// Created by Merutilm on 2025-05-22.
//

#pragma once

#include "../calc/calculatable.hpp"
#include "../calc/complex.hpp"
#include "../formula/MB2Reference.h"
#include "ArrayCompressionTool.h"
#include "PA.h"

namespace merutilm::rff2 {

    
    template<Number Num>
    struct PAGenerator {

        uint64_t start;
        uint64_t skip = 0;
        const std::vector<ArrayCompressionTool> &compressors;
        double epsilon;
        Num radius = Num(HUGE_VAL);

        const std::vector<complex<Num>> &orbit;
        Num dcMax;
        complex<Num> an = {Num(1), Num(0)};
        complex<Num> bn = {Num(0), Num(0)};

        explicit PAGenerator(const MB2Reference<Num> &reference, double epsilon, Num dcMax, uint64_t start);

        void merge(const PA<Num> &pa);

        void step();

        [[nodiscard]] PA<Num> build([[maybe_unused]] std::pmr::monotonic_buffer_resource *const resource) { return PA<Num>{this->skip, an, bn, this->radius}; }
    };

    template<Number Num>
    PAGenerator<Num>::PAGenerator(const MB2Reference<Num> &reference, const double epsilon, Num dcMax,
                                  const uint64_t start) : start(start), compressors(reference.compressor), epsilon(epsilon), orbit(reference.refOrbit), dcMax(dcMax) {}
    template<Number Num>
    void PAGenerator<Num>::merge(const PA<Num> &pa) {
        const auto &target = pa;
        const complex<Num> anMerge = target.an * an;
        const complex<Num> bnMerge = target.an * bn + target.bn;

        this->radius = calculatable::try_normalized_value(std::min(this->radius, target.radius));

        an = anMerge.try_normalized_value();
        bn = bnMerge.try_normalized_value();

        this->skip += target.skip;
    }

    template<Number Num>
    void PAGenerator<Num>::step() {
        const uint64_t iter = this->start + this->skip++; // k+n
        const uint64_t index = ArrayCompressor::compress(this->compressors, iter);
        const complex<Num> z2 = Num(2) * this->orbit[index];

        this->radius = calculatable::try_normalized_value(
                std::min(this->radius, (Num(this->epsilon) * z2.norm_approx() - bn.norm_approx() * this->dcMax) / an.norm_approx()));

        an = (an * z2).try_normalized_value();
        bn = (bn * z2 + Num(1)).try_normalized_value();
    }


    using DeepPAGenerator = PAGenerator<dex>;
    using LightPAGenerator = PAGenerator<double>;
} // namespace merutilm::rff2
