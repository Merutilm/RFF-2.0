//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../calc/rff_math.h"

struct ColorFloat {
    float r;
    float g;
    float b;
    float a;



    static ColorFloat random();

    template <typename F> requires std::is_invocable_r_v<float, F, float>
    ColorFloat forEachExceptAlpha(F&& func) const;

    template <typename F> requires std::is_invocable_r_v<float, F, float, float>
    ColorFloat forEachExceptAlpha(const ColorFloat &c, F&& func) const;

    static ColorFloat lerp(const ColorFloat &a, const ColorFloat &b, float rat);
};

inline ColorFloat ColorFloat::random() {
    return ColorFloat(rff_math::random_f(), rff_math::random_f(), rff_math::random_f(), 1);
}

template<typename F> requires std::is_invocable_r_v<float, F, float>
ColorFloat ColorFloat::forEachExceptAlpha(F &&func) const {
    return ColorFloat(func(r), func(g), func(b), a);
}

template<typename F> requires std::is_invocable_r_v<float, F, float, float>
ColorFloat ColorFloat::forEachExceptAlpha(const ColorFloat &c, F &&func) const {
    return ColorFloat(func(r, c.r), func(g, c.g), func(b, c.b), a);
}



inline ColorFloat ColorFloat::lerp(const ColorFloat &a, const ColorFloat &b, const float rat) {
    return ColorFloat{
        .r = a.r + (b.r - a.r) * rat,
        .g = a.g + (b.g - a.g) * rat,
        .b = a.b + (b.b - a.b) * rat,
        .a = 1
    };
}
