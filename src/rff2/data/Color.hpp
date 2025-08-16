//
// Created by Merutilm on 2025-08-15.
//


#pragma once
#include "NormalizedColor.h"

namespace merutilm::rff2 {

    struct Color {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;

        NormalizedColor normalize() const {
            return {
                .r = r / 255.0f,
                .g = g / 255.0f,
                .b = b / 255.0f,
                .a = a / 255.0f,
            };
        }

        static Color from(const NormalizedColor &normalized) {
            return {
                .r = static_cast<unsigned char>(normalized.r * 255.0f),
                .g = static_cast<unsigned char>(normalized.g * 255.0f),
                .b = static_cast<unsigned char>(normalized.b * 255.0f),
                .a = static_cast<unsigned char>(normalized.a * 255.0f)
            };
        }
    };
}
