//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <memory>

#include "../settings/PaletteSettings.h"

namespace merutilm::rff2 {
    struct KFRColorLoader {
        KFRColorLoader() = delete;

        static std::vector<ColorFloat>  loadPaletteSettings();
    };
}
