//
// Created by Merutilm on 2025-05-05.
//

#pragma once

#include "double_exp.h"
#include "gmp.h"

struct fp_decimal_calculator final {
    int exp2;
    mpz_t value;
    mpz_t temp;

    explicit fp_decimal_calculator();

    explicit fp_decimal_calculator(mpz_srcptr value, int exp2);

    explicit fp_decimal_calculator(const double_exp &d, int exp10);

    explicit fp_decimal_calculator(double d, int exp10);

    explicit fp_decimal_calculator(const std::string &str, int exp10);

    ~fp_decimal_calculator();


    fp_decimal_calculator(const fp_decimal_calculator &other);

    fp_decimal_calculator &operator=(const fp_decimal_calculator &other);

    fp_decimal_calculator(fp_decimal_calculator &&other) noexcept;

    fp_decimal_calculator &operator=(fp_decimal_calculator &&other) noexcept;

    static void fst_mul(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

    static void fst_add(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

    static void fst_sub(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

    static void fst_div(fp_decimal_calculator &out, const fp_decimal_calculator &a, const fp_decimal_calculator &b);

    static void fst_dbl(fp_decimal_calculator &out, const fp_decimal_calculator &target);

    static void fst_hvl(fp_decimal_calculator &out, const fp_decimal_calculator &target);

    static void fst_swap(fp_decimal_calculator &a, fp_decimal_calculator &b);

    static void negate(fp_decimal_calculator &target);

    static int exp2ToExp10(int exp2);

    static int exp10ToExp2(int precision);

    double double_value();

    double_exp double_exp_value();

    void setExp10(int exp10);
};
