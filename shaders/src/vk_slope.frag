#version 450
#include <common.glsl>

// define descriptors
#define DESC_ITERATION 1

// include descriptors
#include <desc_iteration.glsl>

// include utilities
#include <utils_iteration.glsl>

layout (set = 0, binding = 0) uniform sampler2D canvas;


layout (set = 2, binding = 0) uniform SlopeUBO {
    float depth;
    float reflection_ratio;
    float opacity;
    float zenith;
    float azimuth;
} slope_settings;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

void main() {

    ivec2 iter_coord = ivec2(gl_FragCoord.xy);

    if(slope_settings.reflection_ratio >= 1 || slope_settings.depth == 0){
        color = texelFetch(canvas, ivec2(iter_coord), 0);
        return;
    }

    float multiplier = float(iteration_info_settings.extent.x) / 1280;

    float aRad = radians(slope_settings.azimuth);
    float zRad = radians(slope_settings.zenith);

    double ld = get_iteration(iter_coord, ivec2(-1, -1));
    double d = get_iteration(iter_coord, ivec2(0, -1));
    double rd = get_iteration(iter_coord, ivec2(1, -1));
    double l = get_iteration(iter_coord, ivec2(-1, 0));
    double r = get_iteration(iter_coord, ivec2(1, 0));
    double lu = get_iteration(iter_coord, ivec2(-1, 1));
    double u = get_iteration(iter_coord, ivec2(0, 1));
    double ru = get_iteration(iter_coord, ivec2(1, 1));

    float dzDx = float((rd + 2 * r + ru) - (ld + 2 * l + lu)) * slope_settings.depth * multiplier;
    float dzDy = float((lu + 2 * u + ru) - (ld + 2 * d + rd)) * slope_settings.depth * multiplier;
    float slope = atan(radians(length(vec2(dzDx, dzDy))), 1);
    float aspect = atan(dzDy, -dzDx);
    float shade = max(slope_settings.reflection_ratio, cos(zRad) * cos(slope) + sin(zRad) * sin(slope) * cos(aRad + aspect));
    float fShade = 1 - slope_settings.opacity * (1 - shade);


    color = vec4(texelFetch(canvas, ivec2(iter_coord), 0).rgb * fShade, 1);
}
