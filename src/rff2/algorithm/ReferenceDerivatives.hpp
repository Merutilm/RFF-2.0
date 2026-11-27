//
// Created by Merutilm on 11/27/26.
//

#pragma once
#include "../calc/fixed_point_complex.hpp"
#include "../calc/iteration_types.hpp"
#include "../calc/templates.hpp"
#include "ReferenceCheckpoint.hpp"

namespace merutilm::rff2 {
    template<Number Num>
    struct ReferenceDerivatives {

        Num dcMax;
        bool useHNRE;

        complex<Num> fzgAn = complex<Num>::ONE;
        complex<Num> fpgBn = complex<Num>::ZERO;
        fixed_point_complex fzgAnArb;
        complex<Num> fzgAnPartition = complex<Num>::ONE;

        ReferenceDerivatives() = default;
        explicit ReferenceDerivatives(const Num &dcMax, const bool useHNRE) :
            dcMax(dcMax), useHNRE(useHNRE), fzgAnArb(1, 0, -1) {}

        void hnreSetExp10(const int64_t exp10) {
            if (!useHNRE)
                return;
            fzgAnArb.set_exp10(exp10);
        }

        void placeCheckpoint(fixed_point_complex &z, std::vector<ReferenceCheckpoint> &checkpoints,
                             const itt::ref refIteration) {
            checkpoints.emplace_back(z, refIteration, static_cast<complex<dex>>(fzgAnPartition));
            fzgAnPartition = complex<Num>::ONE;
        }

        void tryOverwriteApproximateDerivatives(int64_t exp10) {
            if (useHNRE)
                return;
            fzgAnArb = fixed_point_complex(fzgAn.re, fzgAn.im, exp10);
        }

        bool tryUpdate(const fixed_point_complex &z, const complex<Num> &z0, Num &radius2, const itt::ref refIteration,
                       std::array<fixed_point_decimal, fixed_point_util::TEMPS_COUNT> &temps) {
            radius2 = z0.norm_sqr();
            complex<Num> fpgBnTemp = fpgBn * z0 * Num(2) + Num(1);
            Num fpgRadius = fpgBnTemp.max() * dcMax;

            if (refIteration > 0 && fpgRadius > radius2) {
                return false;
            }
            if (refIteration > 0) {
                if (useHNRE) {
                    fixed_point_complex::mul(fzgAnArb, fzgAnArb, z, temps);
                    fixed_point_complex::dbl(fzgAnArb, fzgAnArb);
                }

                fzgAn = (fzgAn * z0 * Num(2)).try_normalized_value();
                fzgAnPartition = (fzgAnPartition * z0 * Num(2)).try_normalized_value();
            }

            fpgBn = fpgBnTemp.try_normalized_value();
            return true;
        }
    };

} // namespace merutilm::rff2
