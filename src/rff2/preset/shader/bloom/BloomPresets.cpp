//
// Created by Merutilm on 2025-05-28.
//
#include "BloomPresets.h"

namespace merutilm::rff2 {

    std::string BloomPresets::Disabled::getName() const {
        return "Disabled";
    }

    BloomSettings BloomPresets::Disabled::bloomSettings() const {
        return BloomSettings{0, 0.0f, 0, 0};
    }

    std::string BloomPresets::Highlighted::getName() const {
        return "Highlighted";
    }

    BloomSettings BloomPresets::Highlighted::bloomSettings() const {
        return BloomSettings{0, 0.05f, 0.2f, 1};
    }

    std::string BloomPresets::HighlightedStrong::getName() const {
        return "Highlighted Strong";
    }

    BloomSettings BloomPresets::HighlightedStrong::bloomSettings() const {
        return BloomSettings{0, 0.08f, 0.4f, 1.5f};
    }

    std::string BloomPresets::Weak::getName() const {
        return "Weak";
    }

    BloomSettings BloomPresets::Weak::bloomSettings() const {
        return BloomSettings{0, 0.1f, 0, 0.5f};

    }


    std::string BloomPresets::Normal::getName() const {
        return "Normal";
    }

    BloomSettings BloomPresets::Normal::bloomSettings() const {
        return BloomSettings{0, 0.1f, 0, 1};
    }

    std::string BloomPresets::Strong::getName() const {
        return "Strong";
    }

    BloomSettings BloomPresets::Strong::bloomSettings() const {
        return BloomSettings{0, 0.1f, 0, 1.5f};

    }
}