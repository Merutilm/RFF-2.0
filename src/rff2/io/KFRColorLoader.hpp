//
// Created by Merutilm on 2025-07-16.
//

#pragma once
#include <memory>

#include "../attr/ShdPaletteAttribute.h"

namespace merutilm::rff2 {
    struct KFRColorLoader {
        KFRColorLoader() = delete;

        static std::vector<NormalizedColor>  loadPaletteSettings();
    };
}
