//
// Created by Merutilm on 2025-05-05.
//

#pragma once
#include "gmp.h"
#include "GMPDecimalCalculator.h"

class GMPDecimal {
    int exp2;
    mpz_t value;

public:

    GMPDecimal(const std::string &value, int exp10);

    explicit GMPDecimal(const GMPDecimalCalculator &calc);

    ~GMPDecimal();

    GMPDecimal(const GMPDecimal& other);

    GMPDecimal& operator=(const GMPDecimal& other);

    GMPDecimal(GMPDecimal&& other) noexcept;

    GMPDecimal& operator=(GMPDecimal&& other) noexcept;

    std::string toString() const;

    int getExp2() const;

    bool isPositive() const;

    bool isZero() const;

    bool isNegative() const;

    GMPDecimalCalculator edit() const;
};
