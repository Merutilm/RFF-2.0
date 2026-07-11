//
// Created by Merutilm on 2025-05-22.
//

#pragma once

#include "../calc/calculatable.hpp"
#include "../calc/complex.hpp"
#include "../formula/MB2Reference.h"
#include "ArrayCompressionTool.h"
#include "PA.h"
#include "vulkan_helper/base/exception.hpp"

namespace merutilm::rff2 {

    template<Number Num>
    struct PAGeneratorBase {
        uint64_t start;
        uint64_t skip = 0;
        const std::vector<ArrayCompressionTool> &compressors;
        double epsilon;
        Num radius = Num(HUGE_VAL);

        const std::vector<complex<Num>> &orbit;
        Num dcMax;

        explicit PAGeneratorBase(const MB2Reference<Num> &reference, const double epsilon, const Num dcMax,
                                 const uint64_t start) :
            start(start), compressors(reference.compressor), epsilon(epsilon), orbit(reference.refOrbit), dcMax(dcMax) {
        }

    };


    template<Number Num, uint8_t MAX_DEGREE>
    struct PAGenerator : public PAGeneratorBase<Num> {

        /**
         * Generated in the order (1, 0), (0, 1), (2, 0), (1, 1), (0, 2), (3, 0), (2, 1), (1, 2), (0, 3) ...
         */
        std::vector<complex<dex>> tn;
        dex dcMaxPow = dex::ONE;
        Num origRadiusTest = Num(HUGE_VAL);

        explicit PAGenerator(const MB2Reference<Num> &reference, double epsilon, Num dcMax, uint64_t start);


        void merge(const PA<Num, MAX_DEGREE> &pa);

        void step();

        [[nodiscard]] PA<Num, MAX_DEGREE> build(std::pmr::monotonic_buffer_resource *const resource) {
            std::pmr::vector<complex<dex>> tnm(PABase<Num>::getLen(MAX_DEGREE), resource);
            memcpy(tnm.data(), tn.data(), tnm.size() * sizeof(complex<dex>));
            return PA<Num, MAX_DEGREE>{this->skip, std::move(tnm), this->radius};
        }


        [[nodiscard]] complex<dex> tnGet(uint32_t u, uint32_t v) const;
    };

    template<Number Num, uint8_t MAX_DEGREE>
    PAGenerator<Num, MAX_DEGREE>::PAGenerator(const MB2Reference<Num> &reference, const double epsilon, Num dcMax,
                                              const uint64_t start) :
        PAGeneratorBase<Num>(reference, epsilon, dcMax, start),
        tn(PABase<Num>::getLen(MAX_DEGREE + 1)) {
        tn[0] = complex<dex>::ONE;
        for (int i = 0; i < MAX_DEGREE; ++i) {
            dcMaxPow *= dex(dcMax);
        }
    }

    template<Number Num, uint8_t MAX_DEGREE>
    void PAGenerator<Num, MAX_DEGREE>::merge(const PA<Num, MAX_DEGREE> &pa) {
        // TODO merge
        throw vkh::exception_invalid_state("unsupported operation now");
    }

    template<Number Num, uint8_t MAX_DEGREE>
    void PAGenerator<Num, MAX_DEGREE>::step() {


        // tn list
        //  0      1      2      3      4      5      6      7      8      9      10     11
        // (1, 0) (0, 1) (2, 0) (1, 1) (0, 2) (3, 0) (2, 1) (1, 2) (0, 3) (4, 0) (3, 1) (2, 2)
        // if u = 3, v = 2,
        // find
        // (1, 0) (2, 2)
        // (0, 1) (3, 1)
        // (2, 0) (1, 2)
        // (1, 1) (2, 1)
        // (0, 2) (3, 0)
        // (3, 0) (0, 2)
        // (2, 1) (1, 1)
        // (1, 2) (2, 0)
        // (3, 1) (0, 1)
        // (2, 2) (1, 0)

        const uint64_t iter = this->start + this->skip++; // k+n
        const uint64_t index = ArrayCompressor::compress(this->compressors, iter);

        const complex<Num> z2 = Num(2) * this->orbit[index];
        const complex z2x = { dex(z2.re), dex(z2.im) };

        origRadiusTest = calculatable::try_normalized_value(std::min(origRadiusTest, (Num(this->epsilon) * z2.norm_approx() - Num(tn[1].norm_approx()) * Num(this->dcMax)) / Num(tn[0].norm_approx())));

        uint16_t u = 0;
        uint16_t v = MAX_DEGREE + 1;


        for (int i = tn.size() - 1; i >= 0; --i) {

            complex<dex> term = z2x * tn[i];
            uint16_t su = 1;
            uint16_t sv = 0;

            for (int si = 0; si < tn.size(); ++si) {

                if (u + v < su + sv || (u == su && v == sv)) break;

                if (u >= su && v >= sv) {
                    const uint16_t cu = u - su;
                    const uint16_t cv = v - sv;

                    term += tn[si] * tnGet(cu, cv);
                }
                if (su == 0) {
                    su = sv + 1;
                    sv = 0;
                }else {
                    --su;
                    ++sv;
                }
            }

            if (u == 0 && v == 1) term += complex<dex>::ONE;
            tn[i] = term.try_normalized_value();


            if (v == 0) {
                v = u - 1;
                u = 0;
            }else {
                ++u;
                --v;
            }
        }



        const dex numerator = std::max(dex::ZERO, dex(this->epsilon) - dex(MAX_DEGREE) * tnGet(1, MAX_DEGREE).norm_approx() * dcMaxPow);
        const dex denominator = dex(MAX_DEGREE) * tnGet(MAX_DEGREE + 1, 0).norm_approx();
        this->radius = std::min(this->radius, Num(calculatable::try_normalized_value(dex::nthRoot(numerator / denominator, MAX_DEGREE))));


        if (this->radius < origRadiusTest) {
            //TODO experimental
            // std::cout << this->skip << " " << static_cast<double>(this->radius) << " " << static_cast<double>(origRadiusTest) << std::endl;
            this->radius = origRadiusTest;
        }




    }

    template<Number Num, uint8_t MAX_DEGREE>
    complex<dex> PAGenerator<Num, MAX_DEGREE>::tnGet(const uint32_t u, const uint32_t v) const {
        const uint32_t degree = u + v;
#ifndef NDEBUG
        if (degree == 0)
            throw std::invalid_argument("degree cannot be zero");
#endif
        return tn[(degree * (degree + 1) >> 1) - 1 + v];
    }


    template<Number Num>
    struct PAGenerator<Num, 1> : PAGeneratorBase<Num> {

        complex<Num> an = {Num(1), Num(0)};
        complex<Num> bn = {Num(0), Num(0)};

        explicit PAGenerator(const MB2Reference<Num> &reference, double epsilon, Num dcMax, uint64_t start);

        void merge(const PA<Num, 1> &pa);

        void step();

        [[nodiscard]] PA<Num, 1> build([[maybe_unused]] std::pmr::monotonic_buffer_resource *const resource) { return PA<Num, 1>{this->skip, an, bn, this->radius}; }
    };

    template<Number Num>
    PAGenerator<Num, 1>::PAGenerator(const MB2Reference<Num> &reference, const double epsilon, Num dcMax,
                                  const uint64_t start) : PAGeneratorBase<Num>(reference, epsilon, dcMax, start) {}
    template<Number Num>
    void PAGenerator<Num, 1>::merge(const PA<Num, 1> &pa) {
        const auto &target = pa;
        const complex<Num> anMerge = target.an * an;
        const complex<Num> bnMerge = target.an * bn + target.bn;

        this->radius = calculatable::try_normalized_value(std::min(this->radius, target.radius));

        an = anMerge.try_normalized_value();
        bn = bnMerge.try_normalized_value();

        this->skip += target.skip;
    }

    template<Number Num>
    void PAGenerator<Num, 1>::step() {
        const uint64_t iter = this->start + this->skip++; // k+n
        const uint64_t index = ArrayCompressor::compress(this->compressors, iter);
        const complex<Num> z2 = Num(2) * this->orbit[index];

        this->radius = calculatable::try_normalized_value(
                std::min(this->radius, (Num(this->epsilon) * z2.norm_approx() - bn.norm_approx() * this->dcMax) / an.norm_approx()));

        an = (an * z2).try_normalized_value();
        bn = (bn * z2 + Num(1)).try_normalized_value();
    }


    template<uint8_t MAX_DEGREE>
    using DeepPAGenerator = PAGenerator<dex, MAX_DEGREE>;

    template<uint8_t MAX_DEGREE>
    using LightPAGenerator = PAGenerator<double, MAX_DEGREE>;
} // namespace merutilm::rff2
