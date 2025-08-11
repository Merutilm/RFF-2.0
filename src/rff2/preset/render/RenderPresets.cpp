//
// Created by Merutilm on 2025-05-31.
//

#include "RenderPresets.h"


namespace merutilm::rff2 {
    std::string RenderPresets::Potato::getName() const {
        return "Potato";
    }

    RenderSettings RenderPresets::Potato::renderSettings() const {
        return RenderSettings{0.1f, true};
    }


    std::string RenderPresets::Low::getName() const {
        return "Low";
    }

    RenderSettings RenderPresets::Low::renderSettings() const {
        return RenderSettings{0.3f, true};
    }

    std::string RenderPresets::Medium::getName() const {
        return "Medium";
    }

    RenderSettings RenderPresets::Medium::renderSettings() const {
        return RenderSettings{0.5f, true};
    }

    std::string RenderPresets::High::getName() const {
        return "High";
    }

    RenderSettings RenderPresets::High::renderSettings() const {
        return RenderSettings{1.0f, true};
    }

    std::string RenderPresets::Ultra::getName() const {
        return "Ultra";
    }

    RenderSettings RenderPresets::Ultra::renderSettings() const {
        return RenderSettings{2.0f, true};
    }

    std::string RenderPresets::Extreme::getName() const {
        return "Extreme";
    }

    RenderSettings RenderPresets::Extreme::renderSettings() const {
        return RenderSettings{4.0f, true};
    }
}