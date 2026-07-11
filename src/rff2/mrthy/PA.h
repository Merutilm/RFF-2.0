//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <cstdint>
#include "../calc/calculatable.hpp"
#include "../calc/complex.hpp"
#include "../calc/dex.h"
namespace merutilm::rff2 {

    template<Number Num>
    struct PABase {

        uint64_t skip;
        Num radius;

        PABase(const uint64_t skip, const Num radius) : skip(skip), radius(radius) {}

        [[nodiscard]] bool isValid(const Num dzRad) const {
            return dzRad < radius;
        }

        static constexpr size_t getLen(const uint16_t maxDegree) {
            return (maxDegree + 1) * (maxDegree + 2) / 2 - 1;
        }
    };

    template<Number Num, uint8_t MAX_DEGREE>
    struct PA : PABase<Num>{

        std::pmr::vector<complex<dex>> tn;

        explicit PA(const uint64_t skip, std::pmr::vector<complex<dex>> &&tn, const Num radius) : PABase<Num>(skip, radius), tn(std::move(tn)) {
        }

        [[nodiscard]] complex<Num> apply(const complex<Num> dz, const complex<Num> dc) {
            complex<dex> result = complex<dex>::ZERO;
            std::array<complex<dex>, MAX_DEGREE + 1> tnTemp = {};

            tnTemp[0] = complex<dex>::ONE;
            const complex dzx = {dex(dz.re), dex(dz.im)};
            const complex dcx = {dex(dc.re), dex(dc.im)};
            complex<dex> dcPow = dcx;
            uint32_t index = 0;

            for (uint32_t i = 1; i < MAX_DEGREE + 1; ++i) {

                //  z       c                   (i = 1)
                //  z       zc      c^2         (i = 2)
                //  z       z^2c    zc^2    c^3 (i = 3)
                //  ...

                for (uint32_t j = 0; j < i; ++j) {
                    tnTemp[j] *= dzx;
                }
                tnTemp[i] = dcPow;
                dcPow = (dcPow * dcx).try_normalized_value();

                //  T[0]z + T[1]c                            (i = 1)
                //  T[2]z^2 + T[3]zc + T[4]c^2               (i = 2)
                //  T[5]z^3 + T[6]z^2c + T[7]zc^2 + T[8]c^3  (i = 3)
                //  ...

                for (uint32_t j = 0; j < i + 1; ++j, ++index) {
                    result += tn[index] * tnTemp[j];
                }
            }
            result = result.try_normalized_value();
            return complex<Num>{Num(result.re), Num(result.im)};
        }

        static constexpr size_t memoryUsage() {
            return sizeof(PA) + PABase<Num>::getLen(MAX_DEGREE) * sizeof(complex<dex>);
        }
    };

    template<Number Num>
    struct PA<Num, 1> : PABase<Num>{
        complex<Num> an;
        complex<Num> bn;

        explicit PA(const uint64_t skip, const complex<Num> an, const complex<Num> bn, const Num radius) : PABase<Num>(skip, radius), an(an), bn(bn) {
        }


        [[nodiscard]] complex<Num> apply(const complex<Num> dz, const complex<Num> dc) {
            return an * dz + bn * dc;
        };

        static constexpr size_t memoryUsage() {
            return sizeof(PA);
        }
    };

    template<uint8_t MAX_DEGREE>
    using LightPA = PA<double, MAX_DEGREE>;

    template<uint8_t MAX_DEGREE>
    using DeepPA = PA<dex, MAX_DEGREE>;
}