#version 450
#define PI 3.141592653589793238

layout (set = 0, binding = 0) uniform sampler2D canvas;

layout (set = 1, binding = 0) uniform IterUBO {
    uvec2 extent;
    double max_value;
} iteration_info_attr;

layout (set = 1, binding = 1) buffer IterSSBO {
    double iterations[];
} iteration_attr;

layout (set = 2, binding = 0) uniform SlopeUBO {
    float depth;
    float reflection_ratio;
    float opacity;
    float zenith;
    float azimuth;
} slope_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

double get_iteration(uvec2 iter_coord, uvec2 offset){
    iter_coord.y = iteration_info_attr.extent.y - iter_coord.y;
    iter_coord += offset;
    return iteration_attr.iterations[iter_coord.y * iteration_info_attr.extent.x + iter_coord.x];
}

void main() {

    uvec2 iter_coord = uvec2(gl_FragCoord.xy);


    if(slope_attr.reflection_ratio >= 1 || slope_attr.depth == 0){
        color = texelFetch(canvas, ivec2(iter_coord), 0);
        return;
    }

    float multiplier = float(iteration_info_attr.extent.x) / 1280;

    float aRad = radians(slope_attr.azimuth);
    float zRad = radians(slope_attr.zenith);

    double ld = get_iteration(iter_coord, uvec2(-1, -1));
    double d = get_iteration(iter_coord, uvec2(0, -1));
    double rd = get_iteration(iter_coord, uvec2(1, -1));
    double l = get_iteration(iter_coord, uvec2(-1, 0));
    double r = get_iteration(iter_coord, uvec2(1, 0));
    double lu = get_iteration(iter_coord, uvec2(-1, 1));
    double u = get_iteration(iter_coord, uvec2(0, 1));
    double ru = get_iteration(iter_coord, uvec2(1, 1));

    float dzDx = float((rd + 2 * r + ru) - (ld + 2 * l + lu)) * slope_attr.depth * multiplier;
    float dzDy = float((lu + 2 * u + ru) - (ld + 2 * d + rd)) * slope_attr.depth * multiplier;
    float slope = atan(radians(length(vec2(dzDx, dzDy))), 1);
    float aspect = atan(dzDy, -dzDx);
    float shade = max(slope_attr.reflection_ratio, cos(zRad) * cos(slope) + sin(zRad) * sin(slope) * cos(aRad + aspect));
    float fShade = 1 - slope_attr.opacity * (1 - shade);


    color = vec4(texelFetch(canvas, ivec2(iter_coord), 0).rgb * fShade, 1);
}
