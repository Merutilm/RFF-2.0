#version 450

uniform sampler2D inputTex;

uniform float gamma;
uniform float exposure;
uniform float hue;
uniform float saturation;
uniform float brightness;
uniform float contrast;

in vec4 fColor;
out vec4 color;

float grayScale(vec3 c) {
    return c.r * 0.3 + c.g * 0.59 + c.b * 0.11;
}

vec3 fixColor(vec3 col) {
    return clamp(col, vec3(0, 0, 0), vec3(1, 1, 1));
}

float getHue(vec3 col) {
    float high = max(max(col.r, col.g), col.b);
    float low = min(min(col.r, col.g), col.b);
    if(high == low){
        return 0;
    }

    float mid = col.r + col.g + col.b - high - low;
    float range = high - low;
    float rat = (mid - low) / range;
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

vec3 addHue(vec3 col, float add) {

    float high = max(max(col.r, col.g), col.b);
    float low = min(min(col.r, col.g), col.b);
    float off = mod(getHue(col) + add, 1) * 6;
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

    vec3 c = texelFetch(inputTex, ivec2(gl_FragCoord.xy), 0).rgb;

    c = fixColor(pow(c, vec3(1 / gamma)));
    c = fixColor(c * (1 + exposure) / (1 - exposure));
    c = fixColor(addHue(c, hue));
    float gray = grayScale(c);
    c = fixColor(c + (c - vec3(gray, gray, gray)) * saturation);
    c = fixColor(c + brightness);
    c = fixColor((c - 0.5) / (1 - contrast) * (1 + contrast) + 0.5);
    color = vec4(c, 1);
}
