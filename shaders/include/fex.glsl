#include <math_constants.glsl>


#ifndef FEX_INCLUDE
#define FEX_INCLUDE


struct Fex {
    int exponent;
    float mantissa;
};

Fex fex(float a) {
    return Fex(0, a);
}

#endif