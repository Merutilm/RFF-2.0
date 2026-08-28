#ifndef DESC_SMOOTH_ZOOM_INCLUDE
#define DESC_SMOOTH_ZOOM_INCLUDE

layout(set = DESC_SMOOTH_ZOOM, binding = 0) uniform SmoothZoomUBO{
    vec2 pos_delta;
    float log_zoom_delta;
} smooth_zoom_settings;

#endif