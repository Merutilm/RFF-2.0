//
// Created by Merutilm on 2025-05-28.
//
#include "ShdFogPresets.hpp"

namespace merutilm::rff2::ShdFogPresets {

    std::string Disabled::getName() const {
        return "Disabled";
    }

    ShdFogSettings Disabled::genFog() const {
        return ShdFogSettings{0.0, 0.0};
    }

    std::string Low::getName() const {
        return "Low";
    }

    ShdFogSettings Low::genFog() const {
        return ShdFogSettings{0.1f, 0.2f};
    }

    std::string Medium::getName() const {
        return "Medium";
    }

    ShdFogSettings Medium::genFog() const {
        return ShdFogSettings{0.1f, 0.5f};
    }

    std::string High::getName() const {
        return "High";
    }

    ShdFogSettings High::genFog() const {
        return ShdFogSettings{0.15f, 0.8f};

    }

    std::string Ultra::getName() const {
        return "Ultra";
    }

    ShdFogSettings Ultra::genFog() const {
        return ShdFogSettings{0.15f, 1};
    }
}
