//
// Created by Merutilm on 2025-05-31.
//

#include "RndDisplayPresets.hpp"

#include <thread>


namespace merutilm::rff2::RndDisplayPresets {
    std::string Potato::getName() const { return "Potato"; }

    RndDisplaySettings Potato::genDisplay() const {
        return RndDisplaySettings{0.125f,
                              60,
                              RndPixelRenderPriority::SEQUENTIAL};
    }


    std::string Low::getName() const { return "Low"; }

    RndDisplaySettings Low::genDisplay() const {
        return RndDisplaySettings{0.25f,
                              60,
                              RndPixelRenderPriority::SEQUENTIAL};
    }

    std::string Medium::getName() const { return "Medium"; }

    RndDisplaySettings Medium::genDisplay() const {
        return RndDisplaySettings{0.5f,
                              60,
                              RndPixelRenderPriority::SEQUENTIAL};
    }

    std::string High::getName() const { return "High"; }

    RndDisplaySettings High::genDisplay() const {
        return RndDisplaySettings{1.0f,
                              60,
                              RndPixelRenderPriority::SEQUENTIAL};
    }

    std::string Ultra::getName() const { return "Ultra"; }

    RndDisplaySettings Ultra::genDisplay() const {
        return RndDisplaySettings{2.0f,
                              60,
                              RndPixelRenderPriority::SEQUENTIAL};
    }

    std::string Extreme::getName() const { return "Extreme (DANGER)"; }

    RndDisplaySettings Extreme::genDisplay() const {
        return RndDisplaySettings{4.0f,
                              60,
                              RndPixelRenderPriority::SEQUENTIAL};
    }
} // namespace merutilm::rff2
