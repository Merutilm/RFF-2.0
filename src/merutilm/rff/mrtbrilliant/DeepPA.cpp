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
                             const uint64_t start) : anr(RFF::Precision::DEX_ONE), ani(RFF::Precision::DEX_ZERO),
                                                     bnr(RFF::Precision::DEX_ZERO), bni(RFF::Precision::DEX_ZERO),
                                                     skip(0), radius(RFF::Precision::DEX_POSITIVE_INFINITY), start(start),
                                                     compressors(reference.compressor),
                                                     refReal(reference.refReal), refImag(reference.refImag),
                                                     epsilon(epsilon),
                                                     dcMax(dcMax) {
}


void DeepPA::Generator::merge(const DeepPA &pa) {
    const double_exp anrMerge = pa.anr * anr - pa.ani * ani;
    const double_exp aniMerge = pa.anr * ani + pa.ani * anr;
    const double_exp bnrMerge = pa.anr * bnr - pa.ani * bni + pa.bnr;
    const double_exp bniMerge = pa.anr * bni + pa.ani * bnr + pa.bni;

    radius = std::min(radius, pa.radius);
    anr = anrMerge;
    ani = aniMerge;
    bnr = bnrMerge;
    bni = bniMerge;
    skip += pa.skip;
}

void DeepPA::Generator::step() {
    const uint64_t iter = start + skip++; //n+k
    const uint64_t index = ArrayCompressor::compress(compressors, iter);

    double_exp z2r = refReal[index];
    double_exp z2i = refImag[index];
    double_exp::dex_mul_2exp(&z2r, z2r, 1);
    double_exp::dex_mul_2exp(&z2i, z2i, 1);

    const double_exp anrStep = anr * z2r - ani * z2i;
    const double_exp aniStep = anr * z2i + ani * z2r;
    const double_exp bnrStep = bnr * z2r - bni * z2i + 1;
    const double_exp bniStep = bnr * z2i + bni * z2r;

    const double_exp z2l = dex_trigonometric::hypot_approx(z2r, z2i);
    const double_exp anlOriginal = dex_trigonometric::hypot_approx(anr, ani);
    const double_exp bnlOriginal = dex_trigonometric::hypot_approx(bnr, bni);


    radius = std::min(radius, (epsilon * z2l - bnlOriginal * dcMax) / anlOriginal);
    anr = anrStep;
    ani = aniStep;
    bnr = bnrStep;
    bni = bniStep;
}


bool DeepPA::isValid(const double_exp &dzRad) const {
    return dzRad < radius;
}
