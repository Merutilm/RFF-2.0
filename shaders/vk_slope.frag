#version 450
#define PI 3.141592653589793238

layout (set = 0, binding = 0) uniform sampler2D canvas;

layout (set = 1, binding = 0) buffer IterSSBO {
    uvec2 extent;
    double max_value;
    double iterations[];
} iteration_attr;

layout (set = 2, binding = 0) uniform SlopeUBO{
    float depth;
    float reflectionRatio;
    float opacity;
    float zenith;
    float azimuth;
} slope_attr;

layout (set = 3, binding = 0) uniform ResolutionUBO{
    uvec2 swapchain_extent;
    float clarityMultiplier;
} resolution_attr;

layout (location = 0) in vec4 fragColor;
layout (location = 1) in vec4 fragTexcoord;

layout (location = 0) out vec4 color;

double getIteration(uvec2 iterCoord){
    iterCoord.y = iteration_attr.extent.y - iterCoord.y;
    return iteration_attr.iterations[iterCoord.y * iteration_attr.extent.x + iterCoord.x];
}

void main() {

    uvec2 iterCoord = uvec2(gl_FragCoord.xy);

    color = texelFetch(canvas, ivec2(iterCoord), 0);
    if(slope_attr.reflectionRatio >= 1 || slope_attr.depth == 0){
        return;
    }


    float aRad = radians(slope_attr.azimuth);
    float zRad = radians(slope_attr.zenith);

    double ld = getIteration(iterCoord + uvec2(-1, -1));
    double d = getIteration(iterCoord + uvec2(0, -1));
    double rd = getIteration(iterCoord + uvec2(1, -1));
    double l = getIteration(iterCoord + uvec2(-1, 0));
    double r = getIteration(iterCoord + uvec2(1, 0));
    double lu = getIteration(iterCoord + uvec2(-1, 1));
    double u = getIteration(iterCoord + uvec2(0, 1));
    double ru = getIteration(iterCoord + uvec2(1, 1));

    float dzDx = float((rd + 2 * r + ru) - (ld + 2 * l + lu)) * slope_attr.depth * resolution_attr.clarityMultiplier;
    float dzDy = float((lu + 2 * u + ru) - (ld + 2 * d + rd)) * slope_attr.depth * resolution_attr.clarityMultiplier;
    float slope = atan(radians(length(vec2(dzDx, dzDy))), 1);
    float aspect = atan(dzDy, -dzDx);
    float shade = max(slope_attr.reflectionRatio, cos(zRad) * cos(slope) + sin(zRad) * sin(slope) * cos(aRad + aspect));
    float fShade = 1 - slope_attr.opacity * (1 - shade);

    color = vec4(color.rgb * fShade, 1);
}
