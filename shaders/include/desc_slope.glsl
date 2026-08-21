#ifndef DESC_SLOPE_INCLUDE
#define DESC_SLOPE_INCLUDE

layout (set = DESC_SLOPE, binding = 0) uniform SlopeUBO {
    float depth;
    float reflection_ratio;
    float opacity;
    float zenith;
    float azimuth;
} slope_settings;

#endif