//
// Created by Merutilm on 2025-05-22.
//

#include "LightPAGenerator.h"
#include <cfloat>
#include "../calc/rff_math.h"

namespace merutilm::rff2 {
    LightPAGenerator::LightPAGenerator(const LightMandelbrotReference &reference, const double epsilon, const double dcMax,
                                                      const uint64_t start) : PAGenerator(start, 0, reference.compressor, epsilon), anr(1), ani(0), bnr(0), bni(0), radius(DBL_MAX),
                                                                              refReal(reference.refReal), refImag(reference.refImag),
                                                                              dcMax(dcMax) {
    }


    void LightPAGenerator::merge(const PA &pa) {
        const auto &target = static_cast<const LightPA &>(pa);
        const double anrMerge = target.anr * anr - target.ani * ani;
        const double aniMerge = target.anr * ani + target.ani * anr;
        const double bnrMerge = target.anr * bnr - target.ani * bni + target.bnr;
        const double bniMerge = target.anr * bni + target.ani * bnr + target.bni;

        radius = std::min(radius, target.radius);
        anr = anrMerge;
        ani = aniMerge;
        bnr = bnrMerge;
        bni = bniMerge;
        skip += target.skip;
    }

    void LightPAGenerator::step() {
        const uint64_t iter = start + skip++; //n+k
        const uint64_t index = ArrayCompressor::compress(compressors, iter);

        const double z2r = 2 * refReal[index];
        const double z2i = 2 * refImag[index];
        const double anrStep = anr * z2r - ani * z2i;
        const double aniStep = anr * z2i + ani * z2r;
        const double bnrStep = bnr * z2r - bni * z2i + 1;
        const double bniStep = bnr * z2i + bni * z2r;

        const double z2l = rff_math::hypot_approx(z2r, z2i);
        const double anlOriginal = rff_math::hypot_approx(anr, ani);
        const double bnlOriginal = rff_math::hypot_approx(bnr, bni);


        radius = std::min(radius, (epsilon * z2l - bnlOriginal * dcMax) / anlOriginal);
        anr = anrStep;
        ani = aniStep;
        bnr = bnrStep;
        bni = bniStep;
    }
}