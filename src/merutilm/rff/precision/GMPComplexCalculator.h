#pragma once

#include <array>
#include <memory>
#include "GMPDecimalCalculator.h"

class GMPComplexCalculator {

    GMPDecimalCalculator real;
    GMPDecimalCalculator imag;
    std::array<GMPDecimalCalculator, 4> temp;

public:
    explicit GMPComplexCalculator(const std::string &re, const std::string &im, int exp10);

    explicit GMPComplexCalculator(const GMPDecimalCalculator &re, const GMPDecimalCalculator &im, int exp10);

    explicit GMPComplexCalculator(double re, double im, int exp10);

    ~GMPComplexCalculator();

    GMPComplexCalculator(const GMPComplexCalculator &other);

    GMPComplexCalculator &operator=(const GMPComplexCalculator &other);

    GMPComplexCalculator(GMPComplexCalculator &&other) noexcept;

    GMPComplexCalculator &operator=(GMPComplexCalculator &&other) noexcept;

private:
    static void mpc_add(GMPComplexCalculator &a, const GMPComplexCalculator &b);

    static void mpc_sub(GMPComplexCalculator &a, const GMPComplexCalculator &b);

    static void mpc_mul(GMPComplexCalculator &a, const GMPComplexCalculator &b);

    static void mpc_div(GMPComplexCalculator &a, const GMPComplexCalculator &b);

    static void mpc_square(GMPComplexCalculator &a);

    static void mpc_doubled(GMPComplexCalculator &a);

public:
    static GMPComplexCalculator init(const std::string &re, const std::string &im, int precision);

    GMPComplexCalculator &operator+=(const GMPComplexCalculator &b);

    GMPComplexCalculator &operator*=(const GMPComplexCalculator &b);

    GMPComplexCalculator &operator-=(const GMPComplexCalculator &b);

    GMPComplexCalculator &operator/=(const GMPComplexCalculator &b);

    GMPComplexCalculator &square();

    GMPComplexCalculator &doubled();

    GMPDecimalCalculator &getReal();

    GMPDecimalCalculator &getImag();

    GMPDecimalCalculator getRealClone() const;

    GMPDecimalCalculator getImagClone() const;

    void setExp10(int exp10);
};
