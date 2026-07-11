//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <glm/vec4.hpp>

#include "../settings/ShdPaletteSettings.h"

namespace merutilm::rff2 {
    struct KFRColorLoader {
        KFRColorLoader() = delete;

        static std::vector<glm::vec4>  loadPaletteSettings();
    };
}
