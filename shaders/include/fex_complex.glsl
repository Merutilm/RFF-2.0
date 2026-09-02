#include <fex.glsl>
#ifndef FEX_COMPLEX_INCLUDE
#define FEX_COMPLEX_INCLUDE

struct FexComplex {
    Fex re;
    Fex im;
};

FexComplex fex_complex(vec2 a) {
    return FexComplex(fex(a.x), fex(a.y));
}

#endif
