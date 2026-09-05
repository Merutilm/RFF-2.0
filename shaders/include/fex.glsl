#include <math_constants.glsl>


#ifndef FEX_INCLUDE
#define FEX_INCLUDE


struct Fex {
    int exponent;
    float mantissa;
};

struct FexComplex {
    Fex re;
    Fex im;
};

Fex fex(float a) {
    return Fex(0, a);
}

FexComplex fex_complex(vec2 a) {
    return FexComplex(fex(a.x), fex(a.y));
}

#endif