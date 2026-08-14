//
// Created by Merutilm on 2025-05-22.
//

#pragma once

#include "../calc/calculatable.hpp"
#include "../calc/complex.hpp"
#include "../mb/MB2Reference.h"
#include "ArrayCompressionTool.h"
#include "PA.h"

namespace merutilm::rff2 {


    template<Number Num>
    struct PAGenerator {

        uint64_t start;
        uint64_t skip = 0;
        const std::vector<ArrayCompressionTool> &compressors;
        const double epsilon;

        const std::vector<complex<Num>> &orbit;
        const Num dcMax;
        complex<Num> an = complex<Num>::ONE;
        complex<Num> bn = complex<Num>::ZERO;
        Num radius = calculatable::try_normalized_value(Num(DBL_MAX));

        explicit PAGenerator(const MB2Reference<Num> &reference, double epsilon, Num dcMax, uint64_t start);

        void reuse(uint64_t start);

        void merge(const PAGenerator &target);

        void merge(const PA<Num> &target);

        template<typename P>
        void merge(const P &target);

        void step();

        [[nodiscard]] PA<Num> build() {
#ifndef NDEBUG
            return PA<Num>{start, skip, an, bn, radius};
#else
            return PA<Num>{skip, an, bn, radius};
#endif
        }
    };


    template<Number Num>
    void PAGenerator<Num>::reuse(uint64_t start) {
        this->start = start;
        this->skip = 0;
        an = complex<Num>::ONE;
        bn = complex<Num>::ZERO;
        radius = calculatable::try_normalized_value(Num(DBL_MAX));
    }

    template<Number Num>
    PAGenerator<Num>::PAGenerator(const MB2Reference<Num> &reference, const double epsilon, Num dcMax,
                                  const uint64_t start) :
        start(start), compressors(reference.compressor), epsilon(epsilon), orbit(reference.refOrbit), dcMax(dcMax) {}

    template<Number Num>
    void PAGenerator<Num>::merge(const PAGenerator &target) {
        merge<PAGenerator>(target);
    }


    template<Number Num>
    void PAGenerator<Num>::merge(const PA<Num> &target) {
        merge<PA<Num>>(target);
    }

    template<Number Num>
    template<typename P>
    void PAGenerator<Num>::merge(const P &target) {
#ifndef NDEBUG
        if (this->start + this->skip != target.start) {
            throw std::invalid_argument("value not match");
        }
#endif
        this->skip += target.skip;

#ifdef NDEBUG
        if (calculatable::is_zero(this->radius) || calculatable::is_zero(target.radius)) {
            this->radius = Num(0);
            return; //skip useless operation
        }
#endif
        this->radius = calculatable::try_normalized_value(std::clamp((target.radius - bn.norm_approx() * this->dcMax) / an.norm_approx(), Num(0), this->radius));


        const complex<Num> anMerge = target.an * an;
        const complex<Num> bnMerge = target.an * bn + target.bn;
        an = anMerge.try_normalized_value();
        bn = bnMerge.try_normalized_value();
    }

    template<Number Num>
    void PAGenerator<Num>::step() {
        const uint64_t iter = this->start + this->skip++; // k+n

#ifdef NDEBUG
        if (calculatable::is_zero(this->radius)) return; //skip useless operation
#endif
        const uint64_t index = ArrayCompressor::compress(this->compressors, iter);
        const complex<Num> z2 = 2.0 * this->orbit[index];

        this->radius = calculatable::try_normalized_value(
                std::clamp((this->epsilon * z2.norm_approx() - bn.norm_approx() * this->dcMax) / an.norm_approx(), Num(0), this->radius));

        an = (an * z2).try_normalized_value();
        bn = (bn * z2 + Num(1)).try_normalized_value();
    }


    using DeepPAGenerator = PAGenerator<dex>;
    using LightPAGenerator = PAGenerator<double>;
} // namespace merutilm::rff2
