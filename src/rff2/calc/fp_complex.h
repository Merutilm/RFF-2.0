//
// Created by Merutilm on 2025-05-04.
//

#pragma once

#include "dex.h"
#include "fp_complex_mutable.h"
#include "fp_decimal.h"

namespace merutilm::rff2 {
    struct fp_complex {
        fp_decimal real;
        fp_decimal imag;


        fp_complex(const std::string &re, const std::string &im, int exp10);

        explicit fp_complex(const fp_complex_mutable &calc);

        fp_decimal &get_real();

        fp_decimal &get_imag();

        std::string to_string() const;

        fp_complex addCenterDouble(dex re, dex im, int exp10) const;

        fp_complex_mutable edit(int exp10) const;
    };

    //DEFINITION OF FP_COMPLEX

    inline fp_complex::fp_complex(const std::string &re, const std::string &im,
                                  const int exp10) : real(fp_decimal(re, exp10)),
                                                     imag(fp_decimal(im, exp10)) {
    }

    inline fp_complex::fp_complex(const fp_complex_mutable &calc) : real(calc.getRealClone()),
                                                                    imag(calc.getImagClone()) {
    }


    inline fp_decimal &fp_complex::get_real() {
        return real;
    }

    inline fp_decimal &fp_complex::get_imag() {
        return imag;
    }


    inline std::string fp_complex::to_string() const {
        const std::string re = real.to_string();
        const std::string im = imag.to_string() + "i";

        if (real.is_zero() && imag.is_zero()) {
            return "0";
        }
        if (real.is_zero()) {
            return im;
        }
        if (imag.is_zero()) {
            return re;
        }

        if (imag.is_positive()) {
            return re + "+" + im;
        }

        return re + im;
    }

    inline fp_complex fp_complex::addCenterDouble(const dex re, const dex im, const int exp10) const {
        auto calc = edit(exp10);
        const auto add = fp_complex_mutable(re, im, exp10);
        calc += add;
        return fp_complex(calc);
    }


    inline fp_complex_mutable fp_complex::edit(const int exp10) const {
        return fp_complex_mutable(real.edit(), imag.edit(), exp10);
    }
}
