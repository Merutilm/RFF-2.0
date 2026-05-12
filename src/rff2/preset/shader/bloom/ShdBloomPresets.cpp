//
// Created by Merutilm on 2025-05-28.
//
#include "ShdBloomPresets.h"

namespace merutilm::rff2 {

    std::wstring BloomPresets::Disabled::getName() const {
        return L"Disabled";
    }

    ShdBloomSettings BloomPresets::Disabled::genBloom() const {
        return ShdBloomSettings{0, 0.0f, 0, 0};
    }

    std::wstring BloomPresets::Highlighted::getName() const {
        return L"Highlighted";
    }

    ShdBloomSettings BloomPresets::Highlighted::genBloom() const {
        return ShdBloomSettings{0, 0.05f, 0.2f, 1};
    }

    std::wstring BloomPresets::HighlightedStrong::getName() const {
        return L"Highlighted Strong";
    }

    ShdBloomSettings BloomPresets::HighlightedStrong::genBloom() const {
        return ShdBloomSettings{0, 0.08f, 0.4f, 1.5f};
    }

    std::wstring BloomPresets::Weak::getName() const {
        return L"Weak";
    }

    ShdBloomSettings BloomPresets::Weak::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 0.5f};

    }


    std::wstring BloomPresets::Normal::getName() const {
        return L"Normal";
    }

    ShdBloomSettings BloomPresets::Normal::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 1};
    }

    std::wstring BloomPresets::Strong::getName() const {
        return L"Strong";
    }

    ShdBloomSettings BloomPresets::Strong::genBloom() const {
        return ShdBloomSettings{0, 0.1f, 0, 1.5f};

    }
}