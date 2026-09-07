#ifndef DESC_COLOR_INCLUDE
#define DESC_COLOR_INCLUDE

layout (set = DESC_COLOR, binding = 0) uniform ColorUBO {
    float gamma;
    float exposure;
    float hue;
    float saturation;
    float brightness;
    float contrast;
} color_settings;


#endif