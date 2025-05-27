//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include <string>

namespace Presets {
    struct Preset {
        virtual ~Preset() = default;
        virtual std::string getName() = 0;
    };
}
