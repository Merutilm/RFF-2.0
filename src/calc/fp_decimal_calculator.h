//
// Created by Merutilm on 2025-05-05.
//

#pragma once

#include "dex.h"
#include "gmp.h"

namespace merutilm::rff2 {
    struct fp_decimal_calculator final {
        int exp2;
        mpz_t value = {};
        mpz_t temp = {};

        explicit fp_decimal_calculator();

        explicit fp_decimal_calculator(mpz_srcptr value, int exp2);

        explicit fp_decimal_calculator(const dex &d, int exp10);

        explicit fp_decimal_calculator(double d, int exp10);

        explicit fp_decimal_calculator(const std::string &str, int exp10);

        ~fp_decimal_calculator();


        fp_decimal_calculator(const fp_decimal_calculator &other);

        fp_decimal_calculator &operator=(const fp_decimal_calculator &other);

        fp_decimal_calculator(fp_decimal_calculator &&other) noexcept;

        fp_decimal_calculator &operator=(fp_decimal_calculator &&other) noexcept;

        static void fp_mul(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

        static void fp_add(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

        static void fp_sub(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

        static void fp_div(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

        static void fp_dbl(fp_decimal_calculator &out, const fp_decimal_calculator &target);

        static void fp_hlv(fp_decimal_calculator &out, const fp_decimal_calculator &target);

        static void fp_swap(fp_decimal_calculator &a, fp_decimal_calculator &b);

        static void negate(fp_decimal_calculator &target);

        static int exp2ToExp10(int exp2);

        static int exp10ToExp2(int precision);

        double double_value();

        void double_exp_value(dex *result);

        void setExp10(int exp10);
    };
}
