//
// Created by Merutilm on 2025-05-28.
//
#include "ShdBloomPresets.hpp"

namespace merutilm::rff2::ShdBloomPresets {

    std::string Disabled::getName() const {
        return "Disabled";
    }

    ShdBloomSettings Disabled::genBloom() const {
        return ShdBloomSettings{0, 0.0f, 0, 0};
    }

    std::string Highlighted::getName() const {
        return "Highlighted";
    }

    ShdBloomSettings Highlighted::genBloom() const {
        return ShdBloomSettings{0, 0.05f, 0.2f, 1};
    }

    std::string HighlightedStrong::getName() const {
        return "Highlighted Strong";
    }

    ShdBloomSettings HighlightedStrong::genBloom() const {
        return ShdBloomSettings{0, 0.08f, 0.4f, 1.5f};
    }

    std::string Weak::getName() const {
        return "Weak";
    }

    ShdBloomSettings Weak::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 0.5f};

    }


    std::string Normal::getName() const {
        return "Normal";
    }

    ShdBloomSettings Normal::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 1};
    }

    std::string Strong::getName() const {
        return "Strong";
    }

    ShdBloomSettings Strong::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 1.5f};

    }
}