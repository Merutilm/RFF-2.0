//
// Created by Merutilm on 2025-05-31.
//

#include "RenderPresets.h"

#include <thread>


namespace merutilm::rff2 {
    std::string RenderPresets::Potato::getName() const {
        return "Potato";
    }

    RenderAttribute RenderPresets::Potato::genRender() const {
        return RenderAttribute{0.1f, 60, true, std::thread::hardware_concurrency()};
    }


    std::string RenderPresets::Low::getName() const {
        return "Low";
    }

    RenderAttribute RenderPresets::Low::genRender() const {
        return RenderAttribute{0.3f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::Medium::getName() const {
        return "Medium";
    }

    RenderAttribute RenderPresets::Medium::genRender() const {
        return RenderAttribute{0.5f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::High::getName() const {
        return "High";
    }

    RenderAttribute RenderPresets::High::genRender() const {
        return RenderAttribute{1.0f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::Ultra::getName() const {
        return "Ultra";
    }

    RenderAttribute RenderPresets::Ultra::genRender() const {
        return RenderAttribute{2.0f, 60, true, std::thread::hardware_concurrency()};
    }

    std::string RenderPresets::Extreme::getName() const {
        return "Extreme (DANGER)";
    }

    RenderAttribute RenderPresets::Extreme::genRender() const {
        return RenderAttribute{4.0f,  60, true, std::thread::hardware_concurrency()};
    }
}
