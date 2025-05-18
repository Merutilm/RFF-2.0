//
// Created by Merutilm on 2025-05-17.
//

#pragma once
#include <string>

struct dex_exp;
struct dex_std;
struct dex_trigonometric;

class double_exp {
    int exp2;
    double mantissa;

    friend dex_exp;
    friend dex_std;
    friend dex_trigonometric;

public:
    explicit double_exp();

    explicit double_exp(int exp2, double mantissa);

    static void dex_add(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_sub(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_mul(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_div(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_sqr(double_exp *result, const double_exp &v);

    static void dex_sqrt(double_exp *result, const double_exp &v);

    static void dex_mul_2exp(double_exp *result, const double_exp &v, int exp2);

    static void dex_div_2exp(double_exp *result, const double_exp &v, int exp2);

    static void dex_cpy(double_exp *result, double v);

    static void dex_cpy(double_exp *result, const double_exp &v);

    bool operator==(const double_exp &other) const = default;

    friend double_exp operator+(const double_exp &a, const double_exp &b);

    friend double_exp operator+(const double_exp &a, double b);

    friend double_exp operator+(double a, const double_exp &b);

    friend double_exp operator-(const double_exp &a, const double_exp &b);

    friend double_exp operator-(const double_exp &a);

    friend double_exp operator-(const double_exp &a, double b);

    friend double_exp operator-(double a, const double_exp &b);

    friend double_exp operator*(const double_exp &a, const double_exp &b);

    friend double_exp operator*(const double_exp &a, double b);

    friend double_exp operator*(double a, const double_exp &b);

    friend double_exp operator/(const double_exp &a, const double_exp &b);

    friend double_exp operator/(const double_exp &a, double b);

    friend double_exp operator/(double a, const double_exp &b);

    double_exp &operator+=(const double_exp &other);

    double_exp &operator+=(double other);

    double_exp &operator-=(const double_exp &other);

    double_exp &operator-=(double other);

    double_exp &operator*=(const double_exp &other);

    double_exp &operator*=(double other);

    double_exp &operator/=(const double_exp &other);

    double_exp &operator/=(double other);

    std::partial_ordering operator<=>(const double_exp &other) const;

    std::partial_ordering operator<=>(double other) const;

    static double_exp value(double value);

    static void fix_value(double_exp *result);

    char sgn() const;

    bool isinf() const;

    bool isnan() const;

    bool is_zero() const;

    explicit operator double() const;

    std::string to_string() const;

    int get_exp2() const;

    double get_mantissa() const;
};
