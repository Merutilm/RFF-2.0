#pragma once

#include <array>

#include "double_exp.h"
#include "fp_decimal_calculator.h"

class fp_complex_calculator {

    fp_decimal_calculator real;
    fp_decimal_calculator imag;
    std::array<fp_decimal_calculator, 4> temp;

public:
    explicit fp_complex_calculator(const std::string &re, const std::string &im, int exp10);

    explicit fp_complex_calculator(const fp_decimal_calculator &re, const fp_decimal_calculator &im, int exp10);

    explicit fp_complex_calculator(double re, double im, int exp10);

    explicit fp_complex_calculator(const double_exp &re, const double_exp &im, int exp10);

    ~fp_complex_calculator();

    fp_complex_calculator(const fp_complex_calculator &other);

    fp_complex_calculator &operator=(const fp_complex_calculator &other);

    fp_complex_calculator(fp_complex_calculator &&other) noexcept;

    fp_complex_calculator &operator=(fp_complex_calculator &&other) noexcept;

private:
    static void mpc_add(fp_complex_calculator &a, const fp_complex_calculator &b);

    static void mpc_sub(fp_complex_calculator &a, const fp_complex_calculator &b);

    static void mpc_mul(fp_complex_calculator &a, const fp_complex_calculator &b);

    static void mpc_div(fp_complex_calculator &a, const fp_complex_calculator &b);

    static void mpc_square(fp_complex_calculator &a);

    static void mpc_doubled(fp_complex_calculator &a);

    static void mpc_halved(fp_complex_calculator &a);

public:
    static fp_complex_calculator init(const std::string &re, const std::string &im, int precision);


    friend fp_complex_calculator &operator+=(fp_complex_calculator &a, const fp_complex_calculator &b) {
        mpc_add(a, b);
        return a;
    }

    friend fp_complex_calculator &operator*=(fp_complex_calculator &a, const fp_complex_calculator &b) {
        mpc_mul(a, b);
        return a;
    }

    friend fp_complex_calculator &operator-=(fp_complex_calculator &a, const fp_complex_calculator &b) {
        mpc_sub(a, b);
        return a;
    }

    friend fp_complex_calculator &operator/=(fp_complex_calculator &a, const fp_complex_calculator &b) {
        mpc_div(a, b);
        return a;
    }

    fp_complex_calculator &square();

    fp_complex_calculator &doubled();

    fp_complex_calculator &halved();

    fp_complex_calculator &negate();

    fp_decimal_calculator &getReal();

    fp_decimal_calculator &getImag();

    fp_decimal_calculator getRealClone() const;

    fp_decimal_calculator getImagClone() const;

    void setExp10(int exp10);
};
