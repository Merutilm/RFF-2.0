#ifndef DESC_FRACTAL_SURFACE_INCLUDE
#define DESC_FRACTAL_SURFACE_INCLUDE

layout (set = DESC_FRACTAL_SURFACE, binding = 0) uniform FractalSurfaceUBO {
    float depth;
    float shadow_brightness;
    float shadow_opacity;
    float light_zenith;
    float light_azimuth;
    float light_sharpness;
    float light_strength;
    float distortion_strength;
    float reflection_ratio;
    float refraction_ratio;
    float wave_frequency;
    vec2 wave_offset;
    float wave_speed;
    float wave_strength;
} surface_settings;

#endif