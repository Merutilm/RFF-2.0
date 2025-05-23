//
// Created by Merutilm on 2025-05-22.
//

#include "DeepPAGenerator.h"

#include "ArrayCompressor.h"
#include "../calc/double_exp_math.h"

DeepPAGenerator::DeepPAGenerator(const DeepMandelbrotReference &reference, const double epsilon, const double_exp &dcMax,
                           const uint64_t start, std::array<double_exp, 8> &temps) : PAGenerator(start, 0, reference.compressor, epsilon), anr(double_exp::DEX_ONE),
                                                                                     ani(double_exp::DEX_ZERO),
                                                                                     bnr(double_exp::DEX_ZERO), bni(double_exp::DEX_ZERO),
                                                                                     radius(double_exp::DEX_ONE),
                                                                                     temps(temps),
                                                                                     refReal(reference.refReal),
                                                                                     refImag(reference.refImag), dcMax(dcMax) {
}


void DeepPAGenerator::merge(const PA &pa) {
    const auto &target = static_cast<const DeepPA &>(pa);
    double_exp::dex_mul(&temps[0], anr, target.anr);
    double_exp::dex_mul(&temps[1], ani, target.ani);
    double_exp::dex_sub(&temps[0], temps[0], temps[1]);
    double_exp::dex_mul(&temps[1], anr, target.ani);
    double_exp::dex_mul(&temps[2], ani, target.anr);
    double_exp::dex_cpy(&anr, temps[0]);
    double_exp::dex_add(&ani, temps[1], temps[2]);
    double_exp::dex_mul(&temps[0], bnr, target.anr);
    double_exp::dex_mul(&temps[1], bni, target.ani);
    double_exp::dex_sub(&temps[0], temps[0], temps[1]);
    double_exp::dex_add(&temps[0], temps[0], target.bnr);
    double_exp::dex_mul(&temps[1], bnr, target.ani);
    double_exp::dex_mul(&temps[2], bni, target.anr);
    double_exp::dex_add(&temps[1], temps[1], temps[2]);
    double_exp::dex_cpy(&bnr, temps[0]);
    double_exp::dex_add(&bni, temps[1], target.bni);
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
    double_exp::dex_mul_2exp(&temps[0], refReal[index], 1);
    double_exp::dex_mul_2exp(&temps[1], refImag[index], 1);
    double_exp::dex_mul(&temps[2], anr, temps[0]);
    double_exp::dex_mul(&temps[3], ani, temps[1]);
    double_exp::dex_sub(&temps[2], temps[2], temps[3]);
    double_exp::dex_mul(&temps[3], anr, temps[1]);
    double_exp::dex_mul(&temps[4], ani, temps[0]);
    double_exp::dex_add(&temps[3], temps[3], temps[4]);
    double_exp::dex_mul(&temps[4], bnr, temps[0]);
    double_exp::dex_mul(&temps[5], bni, temps[1]);
    double_exp::dex_sub(&temps[4], temps[4], temps[5]);
    double_exp::dex_cpy(&temps[5], 1);
    double_exp::dex_add(&temps[4], temps[4], temps[5]);
    double_exp::dex_mul(&temps[5], bnr, temps[1]);
    double_exp::dex_mul(&temps[6], bni, temps[0]);
    double_exp::dex_add(&temps[5], temps[5], temps[6]);
    dex_trigonometric::hypot_approx(&temps[6], anr, ani);
    dex_trigonometric::hypot_approx(&temps[7], bnr, bni);
    dex_trigonometric::hypot_approx(&temps[0], temps[0], temps[1]);
    double_exp::dex_cpy(&temps[1], epsilon);
    double_exp::dex_mul(&temps[0], temps[0], temps[1]);
    double_exp::dex_mul(&temps[1], temps[7], dcMax);
    double_exp::dex_sub(&temps[0], temps[0], temps[1]);
    double_exp::dex_div(&temps[0], temps[0], temps[6]);
    dex_std::min(&radius, radius, temps[0], &temps[1]);
    double_exp::dex_cpy(&anr, temps[2]);
    double_exp::dex_cpy(&ani, temps[3]);
    double_exp::dex_cpy(&bnr, temps[4]);
    double_exp::dex_cpy(&bni, temps[5]);
    radius.try_normalize();
    anr.try_normalize();
    ani.try_normalize();
    bnr.try_normalize();
    bni.try_normalize();
}