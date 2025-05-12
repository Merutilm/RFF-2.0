//
// Created by Merutilm on 2025-05-11.
//

#include "LightPA.h"

#include "ApproxMath.h"


LightPA::LightPA(const double anr, const double ani, const double bnr, const double bni, const uint64_t skip, const double radius) : anr(anr), ani(ani), bnr(bnr), bni(bni), skip(skip), radius(radius){

}


LightPA::Generator::Generator(const LightMandelbrotReference &reference, const double epsilon, const double dcMax,
                              const uint64_t start) : anr(1), ani(0), bnr(0), bni(0), skip(0), radius(DBL_MAX), start(start),
                                                      compressors(reference.compressor),
                                                      refReal(reference.refReal), refImag(reference.refImag), epsilon(epsilon),
                                                      dcMax(dcMax) {
}


void LightPA::Generator::merge(const LightPA &pa) {
    const double anrMerge = pa.anr * anr - pa.ani * ani;
    const double aniMerge = pa.anr * ani + pa.ani * anr;
    const double bnrMerge = pa.anr * bnr - pa.ani * bni + pa.bnr;
    const double bniMerge = pa.anr * bni + pa.ani * bnr + pa.bni;

    radius = std::min(radius, pa.radius);
    anr = anrMerge;
    ani = aniMerge;
    bnr = bnrMerge;
    bni = bniMerge;
    skip += pa.skip;
}

void LightPA::Generator::step() {
    const uint64_t iter = start + skip++; //n+k
    const uint64_t index = ArrayCompressor::compress(compressors, iter);

    const double z2r = 2 * refReal[index];
    const double z2i = 2 * refImag[index];
    const double anrStep = anr * z2r - ani * z2i;
    const double aniStep = anr * z2i + ani * z2r;
    const double bnrStep = bnr * z2r - bni * z2i + 1;
    const double bniStep = bnr * z2i + bni * z2r;

    const double z2l = ApproxMath::hypotApproximate(z2r, z2i);
    const double anlOriginal = ApproxMath::hypotApproximate(anr, ani);
    const double bnlOriginal = ApproxMath::hypotApproximate(bnr, bni);


    radius = std::min(radius, (epsilon * z2l - bnlOriginal * dcMax) / anlOriginal);
    anr = anrStep;
    ani = aniStep;
    bnr = bnrStep;
    bni = bniStep;
}


bool LightPA::isValid(const double dzRad) const {
    return dzRad < radius;
}