//
// Created by Merutilm on 2025-05-28.
//
#include "ShdBloomPresets.h"

namespace merutilm::rff2 {

    std::string BloomPresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdBloomSettings BloomPresets::Disabled::genBloom() const {
        return ShdBloomSettings{0, 0.0f, 0, 0};
    }

    std::string BloomPresets::Highlighted::getName() const {
        return "Highlighted";
    }

    ShdBloomSettings BloomPresets::Highlighted::genBloom() const {
        return ShdBloomSettings{0, 0.05f, 0.2f, 1};
    }

    std::string BloomPresets::HighlightedStrong::getName() const {
        return "Highlighted Strong";
    }

    ShdBloomSettings BloomPresets::HighlightedStrong::genBloom() const {
        return ShdBloomSettings{0, 0.08f, 0.4f, 1.5f};
    }

    std::string BloomPresets::Weak::getName() const {
        return "Weak";
    }

    ShdBloomSettings BloomPresets::Weak::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 0.5f};

    }


    std::string BloomPresets::Normal::getName() const {
        return "Normal";
    }

    ShdBloomSettings BloomPresets::Normal::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 1};
    }

    std::string BloomPresets::Strong::getName() const {
        return "Strong";
    }

    ShdBloomSettings BloomPresets::Strong::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 1.5f};

    }
}