//
// Created by Merutilm on 2025-05-22.
//

#include "DeepPAGenerator.h"

#include "ArrayCompressor.h"
#include "../calc/double_exp_math.h"

namespace merutilm::rff2 {
    DeepPAGenerator::DeepPAGenerator(const DeepMB2Reference &reference, const double epsilon, const dex dcMax,
                                                    const uint64_t start) : PAGenerator(start, 0, reference.compressor, epsilon), anr(dex::ONE),
                                                                                         ani(dex::ZERO),
                                                                                         bnr(dex::ZERO), bni(dex::ZERO),
                                                                                         radius(dex::ONE),
                                                                                         refReal(reference.refReal),
                                                                                         refImag(reference.refImag), dcMax(dcMax) {
    }


    void DeepPAGenerator::merge(const PA &pa) {
        const auto &target = static_cast<const DeepPA &>(pa);
        const dex anrMerge = target.anr * anr - target.ani * ani;
        const dex aniMerge = target.anr * ani + target.ani * anr;
        const dex bnrMerge = target.anr * bnr - target.ani * bni + target.bnr;
        const dex bniMerge = target.anr * bni + target.ani * bnr + target.bni;

        radius = std::min(radius, target.radius);
        radius.try_normalize();

        anr = anrMerge;
        ani = aniMerge;
        bnr = bnrMerge;
        bni = bniMerge;

        anr.try_normalize();
        ani.try_normalize();
        bnr.try_normalize();
        bni.try_normalize();
        skip += target.skip;
    }

    void DeepPAGenerator::step() {
        const uint64_t iter = start + skip++; // n+k
        const uint64_t index = ArrayCompressor::compress(compressors, iter);

        const dex z2r = dex(2) * refReal[index];
        const dex z2i = dex(2) * refImag[index];
        const dex anrStep = anr * z2r - ani * z2i;
        const dex aniStep = anr * z2i + ani * z2r;
        const dex bnrStep = bnr * z2r - bni * z2i + dex::ONE;
        const dex bniStep = bnr * z2i + bni * z2r;
        const dex z2l = dex_trig::hypot_approx(z2r, z2i);
        const dex anlOriginal = dex_trig::hypot_approx(anr, ani);
        const dex bnlOriginal = dex_trig::hypot_approx(bnr, bni);
        radius = std::min(radius, (dex(epsilon) * z2l - bnlOriginal * dcMax) / anlOriginal);
        anr = anrStep;
        ani = aniStep;
        bnr = bnrStep;
        bni = bniStep;

        radius.try_normalize();
        anr.try_normalize();
        ani.try_normalize();
        bnr.try_normalize();
        bni.try_normalize();
    }
}