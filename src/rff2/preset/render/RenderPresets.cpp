//
// Created by Merutilm on 2025-05-31.
//

#include "RenderPresets.h"

#include <thread>


namespace merutilm::rff2 {
    std::string RenderPresets::Potato::getName() const {
        return "Potato";
    }

    RenderSettings RenderPresets::Potato::genRender() const {
        return RenderSettings{0.1f, 60, true, std::thread::hardware_concurrency()};
    }


    std::string RenderPresets::Low::getName() const {
        return "Low";
    }

    RenderSettings RenderPresets::Low::genRender() const {
        return RenderSettings{0.3f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::Medium::getName() const {
        return "Medium";
    }

    RenderSettings RenderPresets::Medium::genRender() const {
        return RenderSettings{0.5f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::High::getName() const {
        return "High";
    }

    RenderSettings RenderPresets::High::genRender() const {
        return RenderSettings{1.0f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::Ultra::getName() const {
        return "Ultra";
    }

    RenderSettings RenderPresets::Ultra::genRender() const {
        return RenderSettings{2.0f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::Extreme::getName() const {
        return "Extreme (DANGER)";
    }

    RenderSettings RenderPresets::Extreme::genRender() const {
        return RenderSettings{4.0f,  60, true, std::thread::hardware_concurrency()};
    }
}
