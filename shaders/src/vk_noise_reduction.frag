#version 450
#include <common.glsl>

// define descriptors

// include descriptors

// include utilities

layout (set = 0, binding = 0) uniform sampler2D canvas;
layout (set = 1, binding = 0) uniform NoiseReductionUBO {
    bool use;
    uint similar_count_threshold;
    float diff_threshold;
} noise_reduction_settings;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;


vec4 get_pixel(ivec2 p, ivec2 size)
{
    return texelFetch(
        canvas,
        clamp(p, ivec2(0), size - ivec2(1)),
        0
    );
}


float col_diff2(vec3 a, vec3 b)
{
    vec3 d = a - b;
    return dot(d, d);
}


void main()
{
    ivec2 size = textureSize(canvas, 0);
    ivec2 coord = ivec2(gl_FragCoord.xy);

    vec4 center = get_pixel(coord, size);

    if (!noise_reduction_settings.use) {
        color = center;
        return;
    }


    vec4 p[8];

    p[0] = get_pixel(coord + ivec2(-1, -1), size);
    p[1] = get_pixel(coord + ivec2(0, -1), size);
    p[2] = get_pixel(coord + ivec2(1, -1), size);
    p[3] = get_pixel(coord + ivec2(-1, 0), size);
    p[4] = get_pixel(coord + ivec2(1, 0), size);
    p[5] = get_pixel(coord + ivec2(-1, 1), size);
    p[6] = get_pixel(coord + ivec2(0, 1), size);
    p[7] = get_pixel(coord + ivec2(1, 1), size);

    float diffs2[8];

    for (int i = 0; i < 8; ++i)
    {
        diffs2[i] = col_diff2(center.rgb, p[i].rgb);
    }

    int similar_count = 0;

    for (int i = 0; i < 8; ++i)
    {
        if (diffs2[i] < noise_reduction_settings.diff_threshold * noise_reduction_settings.diff_threshold) ++similar_count;
    }

    if (similar_count <= noise_reduction_settings.similar_count_threshold)
    {
        vec3 sum_similar = vec3(0.0);
        vec3 sum = vec3(0.0);
        int cnt = 0;

        for (int i = 0; i < 8; ++i)
        {
            bool similar = false;

            for (int j = 0; j < 8; ++j)
            {
                if (col_diff2(p[i].rgb, p[j].rgb) < noise_reduction_settings.diff_threshold * noise_reduction_settings.diff_threshold)
                {
                    similar = true;
                    break;
                }
            }

            sum += p[i].rgb;

            if (similar)
            {
                sum_similar += p[i].rgb;
                ++cnt;
            }
        }

        if (cnt == 0) color = vec4(sum / 8, 1);
        else color = vec4(sum_similar / cnt, 1);
    }
    else
    {
        color = center;
    }
}
