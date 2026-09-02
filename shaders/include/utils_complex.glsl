#include <fex_complex.glsl>
#include <utils_fex.glsl>

#ifndef UTILS_COMPLEX_INCLUDE
#define UTILS_COMPLEX_INCLUDE

FexComplex fex_complex_div(FexComplex a, Fex b) {
    return FexComplex(fex_div(a.re, b), fex_div(a.im, b));
}


FexComplex fex_complex_add(FexComplex a, FexComplex b) {
    return FexComplex(fex_add(a.re, b.re), fex_add(a.im, b.im));
}


FexComplex fex_complex_sub(FexComplex a, FexComplex b) {
    return FexComplex(fex_sub(a.re, b.re), fex_sub(a.im, b.im));
}

FexComplex fex_complex_mul(FexComplex a, Fex b)
{
    return FexComplex(
        fex_mul(a.re, b),
        fex_mul(a.im, b)
    );
}
FexComplex fex_complex_mul(FexComplex a, FexComplex b)
{
    return FexComplex(
        fex_sub(fex_mul(a.re, b.re), fex_mul(a.im, b.im)),
        fex_add(fex_mul(a.re, b.im), fex_mul(a.im, b.re))
    );
}

vec2 fex_complex_cast(FexComplex a) {
    return vec2(fex_cast(a.re), fex_cast(a.im));
}

vec2 complex_mul(vec2 a, vec2 b)
{
    return vec2(
    a.x * b.x - a.y * b.y,
    a.x * b.y + a.y * b.x
    );
}

float norm2(vec2 a)
{
    return a.x * a.x + a.y * a.y;
}

Fex norm2(FexComplex a)
{
    return fex_add(fex_sqr(a.re), fex_sqr(a.im));
}

float norm_approx(vec2 v) {
    v.x = abs(v.x);
    v.y = abs(v.y);
    float min = min(v.x, v.y);
    float max = max(v.x, v.y);

    if (max == 0) {
        return 0;
    }

    return max + 0.428f * min / max * min;
}


Fex norm_approx(FexComplex v) {
    v.re = fex_abs(v.re);
    v.im = fex_abs(v.im);
    Fex min = fex_min(v.re, v.im);
    Fex max = fex_max(v.re, v.im);

    if (max.mantissa == 0) {
        return fex(0);
    }

    return fex_mul(fex_div(fex_add(max, fex_mul(fex(0.428f), min)), max), min);
}


#endif