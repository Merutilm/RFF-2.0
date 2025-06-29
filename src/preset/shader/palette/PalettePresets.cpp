//
// Created by Merutilm on 2025-05-28.
//

#include "PalettePresets.h"

#include <algorithm>

namespace merutilm::rff2 {
    std::string PalettePresets::LongRandom64::getName() const {
        return "Long Random 64";
    }

    PaletteSettings PalettePresets::LongRandom64::paletteSettings() const {
        auto p = PaletteSettings();
        p.colors.reserve(64);

        for (int i = 0; i < 64; ++i) {
            p.colors.push_back(ColorFloat::random());
        }

        p.iterationInterval = 1;


        const PaletteSettings p1 = p;
        constexpr uint64_t r1 = 100;

        p.colors.clear();
        p.colors.reserve(p1.colors.size() * r1);

        for (uint64_t i = 0; i < p1.colors.size() * r1; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r1);
            const ColorFloat c2 = p1.getMidColor(irv / static_cast<float>(p1.colors.size()));
            const ColorFloat cr = ColorFloat::random();

            p.colors.push_back(c2.forEachExceptAlpha(cr, [](const float c, const float t) { return c + t / 6; }));
        }

        const PaletteSettings p2 = p;
        constexpr uint64_t r2 = 100;
        p.colors.clear();
        p.colors.reserve(p2.colors.size() * r2);

        for (uint64_t i = 0; i < p2.colors.size() * r2; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r2);

            ColorFloat c2 = p2.getMidColor(irv / static_cast<float>(p2.colors.size()));
            const float v = (c2.r + c2.g + c2.b) / 3;
            const float o = 0.5f + 0.5f * std::sin(std::fmod(irv, 1.0f) * 150.0f);

            p.colors.push_back(c2.forEachExceptAlpha([v, o](const float c) {
                return std::lerp(c, v / (1 + rff_math::random_f() * 2.0f), o);
            }));
        }


        p.iterationInterval = 18000000.0f;
        p.offsetRatio = 0;
        p.colorSmoothing = ColorSmoothingSettings::NORMAL;
        return p;
    }

    std::string PalettePresets::LongRainbow7::getName() const {
        return "Long Rainbow 7";
    }

    PaletteSettings PalettePresets::LongRainbow7::paletteSettings() const {
        auto p = PaletteSettings();
        p.colors.reserve(7);
        p.colors.push_back(ColorFloat{0.909803f, 0.078431f, 0.086274f, 1.000000f});
        p.colors.push_back(ColorFloat{1.000000f, 0.647058f, 0.000000f, 1.000000f});
        p.colors.push_back(ColorFloat{0.980392f, 0.921568f, 0.211764f, 1.000000f});
        p.colors.push_back(ColorFloat{0.474509f, 0.764705f, 0.078431f, 1.000000f});
        p.colors.push_back(ColorFloat{0.282352f, 0.490196f, 0.905882f, 1.000000f});
        p.colors.push_back(ColorFloat{0.294117f, 0.211764f, 0.615686f, 1.000000f});
        p.colors.push_back(ColorFloat{0.439215f, 0.211764f, 0.615686f, 1.000000f});

        p.iterationInterval = 1;


        const PaletteSettings p1 = p;
        constexpr uint64_t r1 = 100;

        p.colors.clear();
        p.colors.reserve(p1.colors.size() * r1);

        for (uint64_t i = 0; i < p1.colors.size() * r1; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r1);
            const ColorFloat c2 = p1.getMidColor(irv / static_cast<float>(p1.colors.size()));
            const ColorFloat cr = ColorFloat::random();

            p.colors.push_back(c2.forEachExceptAlpha(cr, [](const float c, const float t) { return c + t / 6; }));
        }

        const PaletteSettings p2 = p;
        constexpr uint64_t r2 = 100;
        p.colors.clear();
        p.colors.reserve(p2.colors.size() * r2);

        for (uint64_t i = 0; i < p2.colors.size() * r2; i++) {
            const float irv = static_cast<float>(i) / static_cast<float>(r2);

            ColorFloat c2 = p2.getMidColor(irv / static_cast<float>(p2.colors.size()));
            const float v = (c2.r + c2.g + c2.b) / 3;
            const float o = 0.5f + 0.5f * std::sin(std::fmod(irv, 1.0f) * 150.0f);

            p.colors.push_back(c2.forEachExceptAlpha([v, o](const float c) {
                return std::lerp(c, v / (1 + rff_math::random_f() * 2.0f), o);
            }));
        }


        p.iterationInterval = 2000000.0f;
        p.offsetRatio = 0.55f;
        p.colorSmoothing = ColorSmoothingSettings::NORMAL;
        return p;
    }
    std::string PalettePresets::Rainbow::getName() const {
        return "Rainbow";
    }

    PaletteSettings PalettePresets::Rainbow::paletteSettings() const {
        auto p = PaletteSettings();
        p.colors.reserve(7);

        p.colors.push_back(ColorFloat{0.909803f, 0.078431f, 0.086274f, 1.000000f});
        p.colors.push_back(ColorFloat{1.000000f, 0.647058f, 0.000000f, 1.000000f});
        p.colors.push_back(ColorFloat{0.980392f, 0.921568f, 0.211764f, 1.000000f});
        p.colors.push_back(ColorFloat{0.474509f, 0.764705f, 0.078431f, 1.000000f});
        p.colors.push_back(ColorFloat{0.282352f, 0.490196f, 0.905882f, 1.000000f});
        p.colors.push_back(ColorFloat{0.294117f, 0.211764f, 0.615686f, 1.000000f});
        p.colors.push_back(ColorFloat{0.439215f, 0.211764f, 0.615686f, 1.000000f});
        p.iterationInterval = 300;
        p.offsetRatio = 0;
        p.colorSmoothing = ColorSmoothingSettings::NORMAL;
        return p;
    }
}