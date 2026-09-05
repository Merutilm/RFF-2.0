#include <fex.glsl>

#ifndef MPA_INCLUDE
#define MPA_INCLUDE

#extension GL_EXT_shader_explicit_arithmetic_types_int64: require

struct PA {
    uint64_t skip;
    vec2 an;
    vec2 bn;
    float radius;
    float _padd; // Explicitly pad the struct to 32 bytes for array stride
};

struct FexPA {
    uint64_t skip;
    FexComplex an;
    FexComplex bn;
    Fex radius;
};

struct MPIndexMapper {
    uint64_t mapped;
    uint64_t levels;
};


#endif