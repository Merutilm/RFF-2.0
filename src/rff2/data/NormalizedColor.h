//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../calc/rff_math.h"

namespace merutilm::rff2 {
    struct NormalizedColor {
        float r;
        float g;
        float b;
        float a;



        static NormalizedColor random();

        template <typename F> requires std::is_invocable_r_v<float, F, float>
        NormalizedColor forEachExceptAlpha(F&& func) const;

        template <typename F> requires std::is_invocable_r_v<float, F, float, float>
        NormalizedColor forEachExceptAlpha(const NormalizedColor &c, F&& func) const;

        static NormalizedColor lerp(const NormalizedColor &a, const NormalizedColor &b, float rat);
    };

    inline NormalizedColor NormalizedColor::random() {
        return NormalizedColor(rff_math::random_f(), rff_math::random_f(), rff_math::random_f(), 1);
    }

    template<typename F> requires std::is_invocable_r_v<float, F, float>
    NormalizedColor NormalizedColor::forEachExceptAlpha(F &&func) const {
        return NormalizedColor(func(r), func(g), func(b), a);
    }

    template<typename F> requires std::is_invocable_r_v<float, F, float, float>
    NormalizedColor NormalizedColor::forEachExceptAlpha(const NormalizedColor &c, F &&func) const {
        return NormalizedColor(func(r, c.r), func(g, c.g), func(b, c.b), a);
    }



    inline NormalizedColor NormalizedColor::lerp(const NormalizedColor &a, const NormalizedColor &b, const float rat) {
        return NormalizedColor{
            .r = a.r + (b.r - a.r) * rat,
            .g = a.g + (b.g - a.g) * rat,
            .b = a.b + (b.b - a.b) * rat,
            .a = 1
        };
    }
}