//
// Created by Merutilm on 2025-05-04.
//

#pragma once

#include "double_exp.h"
#include "gmp.h"
#include "fp_complex_calculator.h"
#include "fp_decimal.h"
#include "gmpxx.h"

struct fp_complex {
    fp_decimal real;
    fp_decimal imag;


    fp_complex(const std::string &re, const std::string &im, int exp10);

    explicit fp_complex(const fp_complex_calculator& calc);

    fp_decimal &get_real();

    fp_decimal &get_imag();

    std::string to_string() const;

    fp_complex addCenterDouble(double_exp re, double_exp im, int exp10) const;

    fp_complex_calculator edit(int exp10) const;


};
