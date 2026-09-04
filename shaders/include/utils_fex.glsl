#include <fex.glsl>

#ifndef UTILS_FEX_INCLUDE
#define UTILS_FEX_INCLUDE


#define EXP_MASK 0x7f800000U
#define EXP_MANTISSA_MASK 0x7fffffffU
#define SGN_MANTISSA_MASK 0x807fffffU
#define NORMALIZED_EXP_BITS 0x3f000000U
#define NORMALIZED_EXP_SUB 0x007e
#define NORMALIZE_CONSTANT_MAX 1e9f
#define NORMALIZE_CONSTANT_MIN 1e-9f
#define MANTISSA_BIT_COUNT 23

float ldexp_neg(float mantissa, int exponent) {
    int mts_bits = floatBitsToInt(mantissa);
    uint mts_ubits = mts_bits & EXP_MANTISSA_MASK;
    int f_shift = int(mts_ubits >> MANTISSA_BIT_COUNT) + exponent;
    // do not consider < smallest normalized value
    return f_shift < 0 ? 0 : intBitsToFloat(mts_bits - (-exponent << MANTISSA_BIT_COUNT));
}

Fex fex_neg(Fex a) {
    return Fex(a.exponent, -a.mantissa);
}

Fex fex_abs(Fex a) {
    return Fex(a.exponent, abs(a.mantissa));
}


Fex fex_add(Fex a, Fex b) {
    if (b.mantissa == 0) return a;
    if (a.mantissa == 0) return b;
    int d_exp2 = a.exponent - b.exponent;
    return Fex(max(a.exponent, b.exponent), ldexp_neg(a.mantissa, min(0, d_exp2)) + ldexp_neg(b.mantissa, min(0, -d_exp2)));
}

Fex fex_sub(Fex a, Fex b) {
    if (b.mantissa == 0) return a;
    if (a.mantissa == 0) return fex_neg(b);
    int d_exp2 = a.exponent - b.exponent;
    return Fex(max(a.exponent, b.exponent), ldexp_neg(a.mantissa, min(0, d_exp2)) - ldexp_neg(b.mantissa, min(0, -d_exp2)));
}

Fex fex_mul(Fex a, float b) {
    return Fex(a.exponent, a.mantissa * b);
}

Fex fex_mul(Fex a, Fex b) {
    return Fex(a.exponent + b.exponent, a.mantissa * b.mantissa);
}

Fex fex_sqr(Fex a) {
    return Fex(a.exponent * 2, a.mantissa * a.mantissa);
}

Fex fex_div(Fex a, Fex b) {
    return Fex(a.exponent - b.exponent, a.mantissa / b.mantissa);
}


Fex fex_exp10(float a) {
    float raw_exp2 = a * LN10 / LN2;
    int exponent = int(raw_exp2);
    return Fex(exponent, exp2(raw_exp2 - exponent));
}

int sgn(Fex a) {
    return int(0 < a.mantissa) - int(a.mantissa < 0);
}

void fex_normalize(inout Fex a) {
    if(a.mantissa==0)
    {
        a.exponent=0;
        return;
    }

    uint mts_bits = floatBitsToUint(a.mantissa);
    a.mantissa = uintBitsToFloat(mts_bits & SGN_MANTISSA_MASK | NORMALIZED_EXP_BITS);
    a.exponent += int((mts_bits & EXP_MASK) >> MANTISSA_BIT_COUNT) - NORMALIZED_EXP_SUB;
}

void fex_try_normalize(inout Fex a) {
    float mts = a.mantissa * sgn(a);
    if (mts > NORMALIZE_CONSTANT_MAX || mts < NORMALIZE_CONSTANT_MIN) {
        fex_normalize(a);
    }
}

float fex_cast(Fex a) {
    return ldexp(a.mantissa, int(a.exponent));
}

bool fex_lt(Fex a, Fex b) {
    Fex v = fex_sub(a, b);
    return v.mantissa < 0;
}

bool fex_eq(Fex a, Fex b) {
    Fex v = fex_sub(a, b);
    return v.mantissa == 0;
}

bool fex_gt(Fex a, Fex b) {
    Fex v = fex_sub(a, b);
    return v.mantissa > 0;
}

bool fex_le(Fex a, Fex b) {
    Fex v = fex_sub(a, b);
    return v.mantissa <= 0;
}

bool fex_ge(Fex a, Fex b) {
    Fex v = fex_sub(a, b);
    return v.mantissa >= 0;
}

Fex fex_min(Fex a, Fex b) {
    return fex_lt(a, b) ? a : b;
}

Fex fex_max(Fex a, Fex b) {
    return fex_gt(a, b) ? a : b;
}


#endif