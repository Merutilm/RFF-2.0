//
// Created by Merutilm on 2025-05-04.
//

#pragma once

#include "gmp.h"
#include "GMPComplexCalculator.h"
#include "GMPDecimal.h"
#include "gmpxx.h"

struct Center {
    GMPDecimal real;
    GMPDecimal imag;


    Center(const std::string &re, const std::string &im, int exp10);

    explicit Center(const GMPComplexCalculator& calc);

    GMPDecimal &getReal();

    GMPDecimal &getImag();

    std::string toString() const;

    Center addCenterDouble(double re, double im, int exp10) const;

    GMPComplexCalculator edit(int exp10) const;
};
