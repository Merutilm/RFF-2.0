//
// Created by Merutilm on 2025-05-28.
//

#include "ShdPalettePresets.h"

#include <algorithm>
#include <glm/glm.hpp>

#include "../../../util/ColorUtils.h"

namespace merutilm::rff2 {
#define PI static_cast<float>(M_PI)
    std::wstring ShdPalettePresets::Classic1::getName() const {
        return L"Classic 1";
    }

    ShdPaletteSettings ShdPalettePresets::Classic1::genPalette() const {
        ShdPaletteSettings p = {};
        for (uint8_t cnt = 0; cnt < 200; ++cnt) {
            const float i = PI * cnt / 100;
            const float r = 0.5f + 0.5f * std::sin(i - 2);
            const float g = 0.5f + 0.5f * std::sin(i - 1.3f);
            const float b = 0.5f + 0.5f * std::sin(i - 0.6f);
            p.colors.emplace_back(r, g, b, 1);
        }
        p.iterationInterval = 250;
        return p;
    }

    std::wstring ShdPalettePresets::Classic2::getName() const {
        return L"Classic 2";
    }

    ShdPaletteSettings ShdPalettePresets::Classic2::genPalette() const {
        ShdPaletteSettings p = {};
        for (uint8_t cnt = 0; cnt < 200; ++cnt) {
            const float i = PI * cnt / 100;
            const float r = 0.5f + 0.5f * std::sin(i - 2);
            const float g = 0.5f + 0.5f * std::sin(i - 0.6f);
            const float b = 0.5f + 0.5f * std::sin(i - 1.3f);
            p.colors.emplace_back(r, g, b, 1);
        }
        p.iterationInterval = 250;
        return p;
    }

    std::wstring ShdPalettePresets::Azure::getName() const {
        return L"Azure";
    }

    ShdPaletteSettings ShdPalettePresets::Azure::genPalette() const {
        ShdPaletteSettings p = {};
        for (uint8_t cnt = 0; cnt < 200; ++cnt) {
            const float i = PI * cnt / 100;
            const float r = 0.5f + 0.5f * std::sin(1.5f * std::sin(i) - 0.5f);
            const float g = 0.5f + 0.5f * std::sin(1.5f * std::sin(i));
            const float b = 0.5f + 0.5f * std::sin(1.5f * std::sin(i) + 0.5f);
            p.colors.emplace_back(r, g, b, 1);
        }
        p.iterationInterval = 300;
        p.offsetRatio = 0.7f;
        return p;
    }

    std::wstring ShdPalettePresets::Cinematic::getName() const {
        return L"Cinematic";
    }

    ShdPaletteSettings ShdPalettePresets::Cinematic::genPalette() const {
        ShdPaletteSettings p = {};
        for (uint8_t cnt = 0; cnt < 200; ++cnt) {
            const float i = PI * cnt / 100;
            float v = 0.5f + 0.5f * std::sin(i);
            glm::vec4 c{v, v, v, 1};
            c = ColorUtils::forEachExceptAlpha(c, glm::vec4{1.000000f, 0.647058f, 0.000000f, 1.000000f},
                                     [v](const float e, const float ta) { return e * (1 - v * 0.3f) + ta * v * 0.3f; });
            p.colors.push_back(c);
        }
        p.iterationInterval = 100;
        p.offsetRatio = 0.7f;
        return p;
    }

    std::wstring ShdPalettePresets::Desert::getName() const {
        return L"Desert";
    }

    ShdPaletteSettings ShdPalettePresets::Desert::genPalette() const {
        ShdPaletteSettings p = {};
        for (uint8_t cnt = 0; cnt < 200; ++cnt) {
            const float i = PI * cnt / 100;
            glm::vec4 c = {0.75f, 0.5f, 0.25f, 1};
            p.colors.emplace_back(ColorUtils::lerp(c, glm::vec4{1, 1, 1, 1}, -0.3f + 0.3f * std::cos(i)));
        }
        p.iterationInterval = 250;
        p.offsetRatio = 0.7f;
        return p;
    }

    std::wstring ShdPalettePresets::Flame::getName() const {
        return L"Flame";
    }

    ShdPaletteSettings ShdPalettePresets::Flame::genPalette() const {
        ShdPaletteSettings p = {};
        for (uint8_t cnt = 0; cnt < 200; ++cnt) {
            const float i = PI * cnt / 100;
            const float r = 0.5f + 0.5f * std::sin(1.5f * std::sin(i) + 0.5f);
            const float g = 0.5f + 0.5f * std::sin(1.5f * std::sin(i));
            const float b = 0.5f + 0.5f * std::sin(1.5f * std::sin(i) - 0.5f);
            p.colors.emplace_back(r, g, b, 1);
        }
        p.iterationInterval = 300;
        p.offsetRatio = 0.7f;
        return p;
    }


    std::wstring ShdPalettePresets::LongRandom64::getName() const {
        return L"Long Random 64";
    }

    ShdPaletteSettings ShdPalettePresets::LongRandom64::genPalette() const {
        auto p = ShdPaletteSettings();
        p.colors.reserve(64);

        for (int i = 0; i < 64; ++i) {
            p.colors.push_back(ColorUtils::random());
        }

        p.iterationInterval = 1;


        const ShdPaletteSettings p1 = p;
        constexpr uint64_t r1 = 100;

        p.colors.clear();
        p.colors.reserve(p1.colors.size() * r1);

        for (uint64_t i = 0; i < p1.colors.size() * r1; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r1);
            const glm::vec4 c2 = p1.getMidColor(irv / static_cast<float>(p1.colors.size()));
            const glm::vec4 cr = ColorUtils::random();

            p.colors.push_back(ColorUtils::forEachExceptAlpha(c2, cr, [](const float c, const float t) { return c + t / 6; }));
        }

        const ShdPaletteSettings p2 = p;
        constexpr uint64_t r2 = 100;
        p.colors.clear();
        p.colors.reserve(p2.colors.size() * r2);

        for (uint64_t i = 0; i < p2.colors.size() * r2; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r2);

            glm::vec4 c2 = p2.getMidColor(irv / static_cast<float>(p2.colors.size()));
            const float v = (c2.r + c2.g + c2.b) / 3;
            const float o = 0.5f + 0.5f * std::sin(std::fmod(irv, 1.0f) * 150.0f);

            p.colors.push_back(ColorUtils::forEachExceptAlpha(c2, [v, o](const float c) {
                return std::lerp(c, v / (1 + rff_math::random_f() * 2.0f), o);
            }));
        }


        p.iterationInterval = 18000000.0f;
        p.offsetRatio = 0;
        p.iterationColoring = ShdIterationColoringMethod::LINEAR;
        p.singleIterationColoring = ShdPalSingleIterationColoringMethod::NORMAL;
        return p;
    }

    std::wstring ShdPalettePresets::LongRainbow7::getName() const {
        return L"Long Rainbow 7";
    }

    ShdPaletteSettings ShdPalettePresets::LongRainbow7::genPalette() const {
        auto p = ShdPaletteSettings();
        p.colors.reserve(7);
        p.colors.push_back(glm::vec4{0.909803f, 0.078431f, 0.086274f, 1.000000f});
        p.colors.push_back(glm::vec4{1.000000f, 0.647058f, 0.000000f, 1.000000f});
        p.colors.push_back(glm::vec4{0.980392f, 0.921568f, 0.211764f, 1.000000f});
        p.colors.push_back(glm::vec4{0.474509f, 0.764705f, 0.078431f, 1.000000f});
        p.colors.push_back(glm::vec4{0.282352f, 0.490196f, 0.905882f, 1.000000f});
        p.colors.push_back(glm::vec4{0.294117f, 0.211764f, 0.615686f, 1.000000f});
        p.colors.push_back(glm::vec4{0.439215f, 0.211764f, 0.615686f, 1.000000f});

        p.iterationInterval = 1;


        const ShdPaletteSettings p1 = p;
        constexpr uint64_t r1 = 100;

        p.colors.clear();
        p.colors.reserve(p1.colors.size() * r1);

        for (uint64_t i = 0; i < p1.colors.size() * r1; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r1);
            const glm::vec4 c2 = p1.getMidColor(irv / static_cast<float>(p1.colors.size()));
            const glm::vec4 cr = ColorUtils::random();

            p.colors.push_back(ColorUtils::forEachExceptAlpha(c2, cr, [](const float c, const float t) { return c + t / 6; }));
        }

        const ShdPaletteSettings p2 = p;
        constexpr uint64_t r2 = 100;
        p.colors.clear();
        p.colors.reserve(p2.colors.size() * r2);

        for (uint64_t i = 0; i < p2.colors.size() * r2; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r2);

            glm::vec4 c2 = p2.getMidColor(irv / static_cast<float>(p2.colors.size()));
            const float v = (c2.r + c2.g + c2.b) / 3;
            const float o = 0.5f + 0.5f * std::sin(std::fmod(irv, 1.0f) * 150.0f);

            p.colors.push_back(ColorUtils::forEachExceptAlpha(c2, [v, o](const float c) {
                return std::lerp(c, v / (1 + rff_math::random_f() * 2.0f), o);
            }));
        }


        p.iterationInterval = 2000000.0f;
        p.offsetRatio = 0.55f;
        p.iterationColoring = ShdIterationColoringMethod::LINEAR;
        p.singleIterationColoring = ShdPalSingleIterationColoringMethod::NORMAL;
        return p;
    }

    std::wstring ShdPalettePresets::Rainbow::getName() const {
        return L"Rainbow";
    }

    ShdPaletteSettings ShdPalettePresets::Rainbow::genPalette() const {
        auto p = ShdPaletteSettings();
        p.colors.reserve(7);

        p.colors.push_back(glm::vec4{0.909803f, 0.078431f, 0.086274f, 1.000000f});
        p.colors.push_back(glm::vec4{1.000000f, 0.647058f, 0.000000f, 1.000000f});
        p.colors.push_back(glm::vec4{0.980392f, 0.921568f, 0.211764f, 1.000000f});
        p.colors.push_back(glm::vec4{0.474509f, 0.764705f, 0.078431f, 1.000000f});
        p.colors.push_back(glm::vec4{0.282352f, 0.490196f, 0.905882f, 1.000000f});
        p.colors.push_back(glm::vec4{0.294117f, 0.211764f, 0.615686f, 1.000000f});
        p.colors.push_back(glm::vec4{0.439215f, 0.211764f, 0.615686f, 1.000000f});
        p.iterationInterval = 300;
        p.offsetRatio = 0;
        p.iterationColoring = ShdIterationColoringMethod::LINEAR;
        p.singleIterationColoring = ShdPalSingleIterationColoringMethod::NORMAL;
        return p;
    }
}
