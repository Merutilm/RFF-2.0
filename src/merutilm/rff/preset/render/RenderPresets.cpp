//
// Created by Merutilm on 2025-05-31.
//

#include "RenderPresets.h"


std::string merutilm::rff::RenderPresets::Potato::getName() const {
    return "Potato";
}

merutilm::rff::RenderSettings merutilm::rff::RenderPresets::Potato::renderSettings() const {
    return RenderSettings{0.1f, true};
}


std::string merutilm::rff::RenderPresets::Low::getName() const {
    return "Low";
}

merutilm::rff::RenderSettings merutilm::rff::RenderPresets::Low::renderSettings() const {
    return RenderSettings{0.3f, true};
}

std::string merutilm::rff::RenderPresets::Medium::getName() const {
    return "Medium";
}

merutilm::rff::RenderSettings merutilm::rff::RenderPresets::Medium::renderSettings() const {
    return RenderSettings{0.5f, true};
}

std::string merutilm::rff::RenderPresets::High::getName() const {
    return "High";
}

merutilm::rff::RenderSettings merutilm::rff::RenderPresets::High::renderSettings() const {
    return RenderSettings{1.0f, true};
}

std::string merutilm::rff::RenderPresets::Ultra::getName() const {
    return "Ultra";
}

merutilm::rff::RenderSettings merutilm::rff::RenderPresets::Ultra::renderSettings() const {
    return RenderSettings{2.0f, true};
}

std::string merutilm::rff::RenderPresets::Extreme::getName() const {
    return "Extreme";
}

merutilm::rff::RenderSettings merutilm::rff::RenderPresets::Extreme::renderSettings() const {
    return RenderSettings{4.0f, true};
}