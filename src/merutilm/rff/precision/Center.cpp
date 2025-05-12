//
// Created by Merutilm on 2025-05-04.
//

#include "Center.h"

#include <iostream>


Center::Center(const std::string &re, const std::string &im, const int exp10) : real(GMPDecimal(re, exp10)),
                                                                                imag(GMPDecimal(im, exp10)) {
}

Center::Center(const GMPComplexCalculator &calc) : real(calc.getRealClone()), imag(calc.getImagClone()) {
}


GMPDecimal &Center::getReal() {
    return real;
}

GMPDecimal &Center::getImag() {
    return imag;
}


std::string Center::toString() const {
    const std::string re = real.toString();
    const std::string im = imag.toString() + "i";

    if (real.isZero() && imag.isZero()) {
        return "0";
    }
    if (real.isZero()) {
        return im;
    }
    if (imag.isZero()) {
        return re;
    }

    if (imag.isPositive()) {
        return re + "+" + im;
    }

    return re + im;
}

Center Center::addCenterDouble(const double re, const double im, const int exp10) const {
    auto calc = edit(exp10);
    const auto add = GMPComplexCalculator(re, im, exp10);
    calc += add;
    return Center(calc);
}


GMPComplexCalculator Center::edit(const int exp10) const {
    return GMPComplexCalculator(real.edit(), imag.edit(), exp10);
}
