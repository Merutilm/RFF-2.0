//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include "../calc/complex.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    class ComplexApproximator {
        double compressionThreshold;
    public:
        explicit ComplexApproximator(const double compressionThreshold) : compressionThreshold(compressionThreshold) {
        }

        [[nodiscard]] bool approximately(const complex<Num> &a, const complex<Num> &b) {
            const bool sr = rff_math::is_zero(b.re) && rff_math::is_zero(a.re);
            const bool si = rff_math::is_zero(b.im) && rff_math::is_zero(a.im);
            return (sr || std::fabs(static_cast<double>(b.re / a.re) - 1) <= compressionThreshold) &&
                   (si || std::fabs(static_cast<double>(b.im / a.im) - 1) <= compressionThreshold);
        }
    };
}