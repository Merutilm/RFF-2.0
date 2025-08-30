//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "../calc/rff_math.h"
#include <glm/glm.hpp>

namespace merutilm::rff2 {
    struct ColorUtils {
        ColorUtils() = delete;

        static glm::vec4 random();

        template<typename F> requires std::is_invocable_r_v<float, F, float>
        static glm::vec4 forEachExceptAlpha(const glm::vec4 &src, F &&func);

        template<typename F> requires std::is_invocable_r_v<float, F, float, float>
        static glm::vec4 forEachExceptAlpha(const glm::vec4 &src, const glm::vec4 &target, F &&func);

        static glm::vec4 lerp(const glm::vec4 &a, const glm::vec4 &b, float rat);
    };

    inline glm::vec4 ColorUtils::random() {
        return glm::vec4{rff_math::random_f(), rff_math::random_f(), rff_math::random_f(), 1};
    }

    template<typename F> requires std::is_invocable_r_v<float, F, float>
    glm::vec4 ColorUtils::forEachExceptAlpha(const glm::vec4 &src, F &&func) {
        return glm::vec4{func(src.r), func(src.g), func(src.b), src.a};
    }

    template<typename F> requires std::is_invocable_r_v<float, F, float, float>
    glm::vec4 ColorUtils::forEachExceptAlpha(const glm::vec4 &src, const glm::vec4 &target, F &&func) {
        return glm::vec4{func(src.r, target.r), func(src.g, target.g), func(src.b, target.b), src.a};
    }


    inline glm::vec4 ColorUtils::lerp(const glm::vec4 &a, const glm::vec4 &b, const float rat) {
        return glm::vec4{
            a.r + (b.r - a.r) * rat,
            a.g + (b.g - a.g) * rat,
            a.b + (b.b - a.b) * rat,
            1
        };
    }
}
