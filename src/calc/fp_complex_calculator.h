#pragma once

#include <array>

#include "dex.h"
#include "fp_decimal_calculator.h"

namespace merutilm::rff2 {
    class fp_complex_calculator {

        fp_decimal_calculator real;
        fp_decimal_calculator imag;
        std::array<fp_decimal_calculator, 4> temp;

    public:
        explicit fp_complex_calculator(const std::string &re, const std::string &im, int exp10);

        explicit fp_complex_calculator(const fp_decimal_calculator &re, const fp_decimal_calculator &im, int exp10);

        explicit fp_complex_calculator(double re, double im, int exp10);

        explicit fp_complex_calculator(const dex &re, const dex &im, int exp10);

        ~fp_complex_calculator();

        fp_complex_calculator(const fp_complex_calculator &other);

        fp_complex_calculator &operator=(const fp_complex_calculator &other);

        fp_complex_calculator(fp_complex_calculator &&other) noexcept;

        fp_complex_calculator &operator=(fp_complex_calculator &&other) noexcept;

    private:
        static void fpc_add(fp_complex_calculator &a, const fp_complex_calculator &b);

        static void fpc_sub(fp_complex_calculator &a, const fp_complex_calculator &b);

        static void fpc_mul(fp_complex_calculator &a, const fp_complex_calculator &b);

        static void fpc_div(fp_complex_calculator &a, const fp_complex_calculator &b);

        static void fpc_square(fp_complex_calculator &a);

        static void fpc_doubled(fp_complex_calculator &a);

        static void fpc_halved(fp_complex_calculator &a);

    public:
        static fp_complex_calculator init(const std::string &re, const std::string &im, int precision);


        friend fp_complex_calculator &operator+=(fp_complex_calculator &a, const fp_complex_calculator &b) {
            fpc_add(a, b);
            return a;
        }

        friend fp_complex_calculator &operator*=(fp_complex_calculator &a, const fp_complex_calculator &b) {
            fpc_mul(a, b);
            return a;
        }

        friend fp_complex_calculator &operator-=(fp_complex_calculator &a, const fp_complex_calculator &b) {
            fpc_sub(a, b);
            return a;
        }

        friend fp_complex_calculator &operator/=(fp_complex_calculator &a, const fp_complex_calculator &b) {
            fpc_div(a, b);
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
}