#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput canvas;

layout (set = 1, binding = 0) uniform ColorUBO {
    float gamma;
    float exposure;
    float hue;
    float saturation;
    float brightness;
    float contrast;
} color_attr;

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexcoord;

layout (location = 0) out vec4 color;

float grayscale(vec3 c) {
    return c.r * 0.3 + c.g * 0.59 + c.b * 0.11;
}

vec3 fix_color(vec3 col) {
    return clamp(col, vec3(0, 0, 0), vec3(1, 1, 1));
}

float get_hue(vec3 col) {
    float high = max(max(col.r, col.g), col.b);
    float low = min(min(col.r, col.g), col.b);
    if (high == low) {
        return 0;
    }

    float range = high - low;
    float mid = clamp(col.r + col.g + col.b - high - low, 0, 1);
    float rat = max(mid - low, 0) / range;

    float offset;
    // hue detection, 0 = low, 1 = high
    // ---------------------------------
    // index | 0   1   2   3   4   5
    // ---------------------------------
    // r     | 1 1 1 ~ 0 0 0 0 0 ~ 1 1
    // g     | 0 ~ 1 1 1 1 1 ~ 0 0 0 0
    // b     | 0 0 0 0 0 ~ 1 1 1 1 1 ~
    // ---------------------------------
    if (low == col.b) {
        if (high == col.r) {
            // 0 = b, ~ = g, 1 = r
            offset = rat;

        } else {
            // 0 = b, ~ = r, 1 = g
            offset = 2 - rat;
        }
    } else if (low == col.r) {
        if (high == col.g) {
            // 0 = r, ~ = b, 1 = g
            offset = 2 + rat;
        } else {
            // 0 = r, ~ = g, 1 = b
            offset = 4 - rat;
        }
    } else {
        if (high == col.b) {
            // 0 = g, ~ = r, 1 = b
            offset = 4 + rat;
        } else {
            // 0 = g, ~ = b, 1 = r
            offset = 6 - rat;
        }
    }

    return offset / 6;
}

vec3 add_hue(vec3 col, float add) {

    float high = max(max(col.r, col.g), col.b);
    float low = min(min(col.r, col.g), col.b);
    float hue = get_hue(col);
    float off = mod(hue + add, 1) * 6;
    int ioff = int(off);
    float doff = mod(off, 1);

    // ---------------------------------
    // index | 0   1   2   3   4   5
    // ---------------------------------
    // r     | 1 1 1 ~ 0 0 0 0 0 ~ 1 1
    // g     | 0 ~ 1 1 1 1 1 ~ 0 0 0 0
    // b     | 0 0 0 0 0 ~ 1 1 1 1 1 ~
    // ---------------------------------


    vec3 result;
    switch (ioff) {
        case 0: {
                    result = vec3(high, low - (low - high) * doff, low);
                    break;
                }
        case 1: {
                    result = vec3(high - (high - low) * doff, high, low);
                    break;
                }
        case 2: {
                    result = vec3(low, high, low - (low - high) * doff);
                    break;
                }
        case 3: {
                    result = vec3(low, high - (high - low) * doff, high);
                    break;
                }
        case 4: {
                    result = vec3(low - (low - high) * doff, low, high);
                    break;
                }
        case 5: {
                    result = vec3(high, low, high - (high - low) * doff);
                    break;
                }
    }
    return result;
}

void main() {

    vec3 c = subpassLoad(canvas).rgb;

    c = fix_color(pow(c, vec3(1 / color_attr.gamma)));
    c = fix_color(c * (1 + color_attr.exposure) / (1 - color_attr.exposure));
    c = fix_color(add_hue(c, color_attr.hue));
    float gray = grayscale(c);
    c = fix_color(c + (c - vec3(gray, gray, gray)) * color_attr.saturation);
    c = fix_color(c + color_attr.brightness);
    c = fix_color((c - 0.5) / (1 - color_attr.contrast) * (1 + color_attr.contrast) + 0.5);
    color = vec4(c, 1);
}
