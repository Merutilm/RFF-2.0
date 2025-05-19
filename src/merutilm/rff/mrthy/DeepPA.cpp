//
// Created by Merutilm on 2025-05-18.
//

#include "DeepPA.h"

#include "ArrayCompressor.h"
#include "../calc/approx_math.h"
#include "../calc/double_exp_math.h"
#include "../ui/RFF.h"

DeepPA::DeepPA(const double_exp &anr, const double_exp &ani, const double_exp &bnr, const double_exp &bni,
               const uint64_t skip, const double_exp &radius) : anr(anr), ani(ani), bnr(bnr), bni(bni), skip(skip),
                                                                radius(radius) {
}


DeepPA::Generator::Generator(const DeepMandelbrotReference &reference, const double epsilon, const double_exp &dcMax,
                             const uint64_t start, std::array<double_exp, 8> &temps) : anr(double_exp::DEX_ONE),
    ani(double_exp::DEX_ZERO),
    bnr(double_exp::DEX_ZERO), bni(double_exp::DEX_ZERO),
    skip(0), radius(double_exp::DEX_POSITIVE_INFINITY), start(start),
    temps(temps),
    compressors(reference.compressor), refReal(reference.refReal),
    refImag(reference.refImag),
    epsilon(epsilon), dcMax(dcMax) {
}


void DeepPA::Generator::merge(const DeepPA &pa) {
    double_exp::dex_mul(&temps[0], anr, pa.anr);
    double_exp::dex_mul(&temps[1], ani, pa.ani);
    double_exp::dex_sub(&temps[0], temps[0], temps[1]);
    double_exp::dex_mul(&temps[1], anr, pa.ani);
    double_exp::dex_mul(&temps[2], ani, pa.anr);
    double_exp::dex_cpy(&anr, temps[0]);
    double_exp::dex_add(&ani, temps[1], temps[2]);
    double_exp::dex_mul(&temps[0], bnr, pa.anr);
    double_exp::dex_mul(&temps[1], bni, pa.ani);
    double_exp::dex_sub(&temps[0], temps[0], temps[1]);
    double_exp::dex_add(&temps[0], temps[0], pa.bnr);
    double_exp::dex_mul(&temps[1], bnr, pa.ani);
    double_exp::dex_mul(&temps[2], bni, pa.anr);
    double_exp::dex_add(&temps[1], temps[1], temps[2]);
    double_exp::dex_cpy(&bnr, temps[0]);
    double_exp::dex_add(&bni, temps[1], pa.bni);
    dex_std::min(&radius, radius, pa.radius);
    if (radius.normalize_required()) double_exp::normalize(&radius);
    if (anr.normalize_required()) double_exp::normalize(&anr);
    if (ani.normalize_required()) double_exp::normalize(&ani);
    if (bnr.normalize_required()) double_exp::normalize(&bnr);
    if (bni.normalize_required()) double_exp::normalize(&bni);
    skip += pa.skip;

    // const double_exp anrMerge = pa.anr * anr - pa.ani * ani;
    // const double_exp aniMerge = pa.anr * ani + pa.ani * anr;
    // const double_exp bnrMerge = pa.anr * bnr - pa.ani * bni + pa.bnr;
    // const double_exp bniMerge = pa.anr * bni + pa.ani * bnr + pa.bni;
    //
    // radius = std::min(radius, pa.radius);
    // anr = anrMerge;
    // ani = aniMerge;
    // bnr = bnrMerge;
    // bni = bniMerge;
}

void DeepPA::Generator::step() {
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
    if (radius.normalize_required()) double_exp::normalize(&radius);
    if (anr.normalize_required()) double_exp::normalize(&anr);
    if (ani.normalize_required()) double_exp::normalize(&ani);
    if (bnr.normalize_required()) double_exp::normalize(&bnr);
    if (bni.normalize_required()) double_exp::normalize(&bni);
}
