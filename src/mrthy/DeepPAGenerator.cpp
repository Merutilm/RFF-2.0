//
// Created by Merutilm on 2025-05-22.
//

#include "DeepPAGenerator.h"

#include "ArrayCompressor.h"
#include "../calc/double_exp_math.h"

namespace merutilm::rff2 {
    DeepPAGenerator::DeepPAGenerator(const DeepMandelbrotReference &reference, const double epsilon, const dex &dcMax,
                                                    const uint64_t start, std::array<dex, 8> &temps) : PAGenerator(start, 0, reference.compressor, epsilon), anr(dex::ONE),
                                                                                         ani(dex::ZERO),
                                                                                         bnr(dex::ZERO), bni(dex::ZERO),
                                                                                         radius(dex::ONE),
                                                                                         temps(temps),
                                                                                         refReal(reference.refReal),
                                                                                         refImag(reference.refImag), dcMax(dcMax) {
    }


    void DeepPAGenerator::merge(const PA &pa) {
        const auto &target = static_cast<const DeepPA &>(pa);
        dex::mul(&temps[0], anr, target.anr);
        dex::mul(&temps[1], ani, target.ani);
        dex::sub(&temps[0], temps[0], temps[1]);
        dex::mul(&temps[1], anr, target.ani);
        dex::mul(&temps[2], ani, target.anr);
        dex::cpy(&anr, temps[0]);
        dex::add(&ani, temps[1], temps[2]);
        dex::mul(&temps[0], bnr, target.anr);
        dex::mul(&temps[1], bni, target.ani);
        dex::sub(&temps[0], temps[0], temps[1]);
        dex::add(&temps[0], temps[0], target.bnr);
        dex::mul(&temps[1], bnr, target.ani);
        dex::mul(&temps[2], bni, target.anr);
        dex::add(&temps[1], temps[1], temps[2]);
        dex::cpy(&bnr, temps[0]);
        dex::add(&bni, temps[1], target.bni);
        dex_std::min(&radius, radius, target.radius);
        radius.try_normalize();
        anr.try_normalize();
        ani.try_normalize();
        bnr.try_normalize();
        bni.try_normalize();
        skip += target.skip;
    }

    void DeepPAGenerator::step() {
        const uint64_t iter = start + skip++; //n+k
        const uint64_t index = ArrayCompressor::compress(compressors, iter);
        dex::mul_2exp(&temps[0], refReal[index], 1);
        dex::mul_2exp(&temps[1], refImag[index], 1);
        dex::mul(&temps[2], anr, temps[0]);
        dex::mul(&temps[3], ani, temps[1]);
        dex::sub(&temps[2], temps[2], temps[3]);
        dex::mul(&temps[3], anr, temps[1]);
        dex::mul(&temps[4], ani, temps[0]);
        dex::add(&temps[3], temps[3], temps[4]);
        dex::mul(&temps[4], bnr, temps[0]);
        dex::mul(&temps[5], bni, temps[1]);
        dex::sub(&temps[4], temps[4], temps[5]);
        dex::cpy(&temps[5], 1);
        dex::add(&temps[4], temps[4], temps[5]);
        dex::mul(&temps[5], bnr, temps[1]);
        dex::mul(&temps[6], bni, temps[0]);
        dex::add(&temps[5], temps[5], temps[6]);
        dex_trigonometric::hypot_approx(&temps[6], anr, ani);
        dex_trigonometric::hypot_approx(&temps[7], bnr, bni);
        dex_trigonometric::hypot_approx(&temps[0], temps[0], temps[1]);
        dex::cpy(&temps[1], epsilon);
        dex::mul(&temps[0], temps[0], temps[1]);
        dex::mul(&temps[1], temps[7], dcMax);
        dex::sub(&temps[0], temps[0], temps[1]);
        dex::div(&temps[0], temps[0], temps[6]);
        dex_std::min(&radius, radius, temps[0], &temps[1]);
        dex::cpy(&anr, temps[2]);
        dex::cpy(&ani, temps[3]);
        dex::cpy(&bnr, temps[4]);
        dex::cpy(&bni, temps[5]);
        radius.try_normalize();
        anr.try_normalize();
        ani.try_normalize();
        bnr.try_normalize();
        bni.try_normalize();
    }
}