//
// Created by Merutilm on 2025-05-05.
//

#pragma once

#include "gmp.h"

struct GMPDecimalCalculator final {
    int exp2;
    mpz_t value;
    mpz_t temp;

    GMPDecimalCalculator();

    GMPDecimalCalculator(mpz_srcptr value, int exp2);

    GMPDecimalCalculator(double d, int exp10);

    explicit GMPDecimalCalculator(const std::string &str, int exp10);

    ~GMPDecimalCalculator();

    GMPDecimalCalculator(const GMPDecimalCalculator &other);

    GMPDecimalCalculator &operator=(const GMPDecimalCalculator &other);

    GMPDecimalCalculator(GMPDecimalCalculator &&other) noexcept;

    GMPDecimalCalculator &operator=(GMPDecimalCalculator &&other) noexcept;

    static void fst_mul(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b);

    static void fst_add(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b);

    static void fst_sub(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b);

    static void fst_div(GMPDecimalCalculator &out, const GMPDecimalCalculator &a, const GMPDecimalCalculator &b);

    static void fst_dbl(GMPDecimalCalculator &out, const GMPDecimalCalculator &target);

    static void fst_swap(GMPDecimalCalculator &a, GMPDecimalCalculator &b);

    static int exp2ToExp10(int exp2);

    static int exp10ToExp2(int precision);

    double doubleValue();

    void setExp10(int exp10);
};
