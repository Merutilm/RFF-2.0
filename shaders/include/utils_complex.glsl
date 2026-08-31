#ifndef UTILS_COMPLEX_INCLUDE
#define UTILS_COMPLEX_INCLUDE


vec2 mul_comp(vec2 a, vec2 b)
{
    return vec2(
    a.x * b.x - a.y * b.y,
    a.x * b.y + a.y * b.x
    );
}


float norm2(vec2 a)
{
    return a.x * a.x + a.y * a.y;
}

float norm_approx(vec2 v) {
    v.x = abs(v.x);
    v.y = abs(v.y);
    float min = min(v.x, v.y);
    float max = max(v.x, v.y);

    if (max == 0) {
        return 0;
    }

    return max + 0.428f * min / max * min;
}

#endif