//
// Created by Merutilm on 2025-05-22.
//

#pragma once

#include "../formula/MB2Reference.h"
#include "ArrayCompressionTool.h"
#include "PA.h"
#include "../calc/calculatable.hpp"

namespace merutilm::rff2 {
    template<Number Num>
    struct PAGenerator {

        uint64_t start;
        uint64_t skip;
        const std::vector<ArrayCompressionTool> &compressors;
        double epsilon;

        Num anr = Num(1);
        Num ani = Num(0);
        Num bnr = Num(0);
        Num bni = Num(0);
        Num radius;

        const std::vector<Num> &refReal;
        const std::vector<Num> &refImag;
        Num dcMax;

        explicit PAGenerator(const MB2Reference<Num> &reference, double epsilon, Num dcMax, uint64_t start);

        [[nodiscard]] uint64_t getStart() const;

        [[nodiscard]] uint64_t getSkip() const;


        void merge(const PA<Num> &pa);

        void step();

        [[nodiscard]] PA<Num> build() const{
            return PA<Num>{skip, anr, ani, bnr, bni,  radius};
        }


    };

    template<Number Num>
    void PAGenerator<Num>::merge(const PA<Num> &pa) {
        const auto &target = pa;
        const Num anrMerge = target.anr * anr - target.ani * ani;
        const Num aniMerge = target.anr * ani + target.ani * anr;
        const Num bnrMerge = target.anr * bnr - target.ani * bni + target.bnr;
        const Num bniMerge = target.anr * bni + target.ani * bnr + target.bni;

        radius = calculatable::try_normalized_value(std::min(radius, target.radius));

        anr = calculatable::try_normalized_value(anrMerge);
        ani = calculatable::try_normalized_value(aniMerge);
        bnr = calculatable::try_normalized_value(bnrMerge);
        bni = calculatable::try_normalized_value(bniMerge);

        skip += target.skip;
    }
    template<Number Num>
    void PAGenerator<Num>::step() {
        const uint64_t iter = start + skip++; // n+k
        const uint64_t index = ArrayCompressor::compress(compressors, iter);

        const Num z2r = Num(2) * refReal[index];
        const Num z2i = Num(2) * refImag[index];
        const Num anrStep = anr * z2r - ani * z2i;
        const Num aniStep = anr * z2i + ani * z2r;
        const Num bnrStep = bnr * z2r - bni * z2i + Num(1);
        const Num bniStep = bnr * z2i + bni * z2r;
        const Num z2l = calculatable::hypot_approx(z2r, z2i);
        const Num anlOriginal = calculatable::hypot_approx(anr, ani);
        const Num bnlOriginal = calculatable::hypot_approx(bnr, bni);
        radius = calculatable::try_normalized_value(std::min(radius, (Num(epsilon) * z2l - bnlOriginal * dcMax) / anlOriginal));
        anr = calculatable::try_normalized_value(anrStep);
        ani = calculatable::try_normalized_value(aniStep);
        bnr = calculatable::try_normalized_value(bnrStep);
        bni = calculatable::try_normalized_value(bniStep);
    }

    template<Number Num>
    PAGenerator<Num>::PAGenerator(const MB2Reference<Num> &reference, const double epsilon, Num dcMax, const uint64_t start) :
    start(start), skip(0), compressors(reference.compressor), epsilon(epsilon), radius(Num(HUGE_VAL)),
                                                                          refReal(reference.refReal), refImag(reference.refImag),
                                                                          dcMax(dcMax) {
    }

    template<Number Num>
    uint64_t PAGenerator<Num>::getStart() const {
        return start;
    }

    template<Number Num>
    uint64_t PAGenerator<Num>::getSkip() const {
        return skip;
    }

    using DeepPAGenerator = PAGenerator<dex>;
    using LightPAGenerator = PAGenerator<double>;
}