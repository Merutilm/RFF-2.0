//
// Created by Merutilm on 2025-05-31.
//

#include "RenderPresets.h"

#include <thread>


namespace merutilm::rff2 {
    std::wstring RenderPresets::Potato::getName() const {
        return L"Potato";
    }

    RenderSettings RenderPresets::Potato::genRender() const {
        return RenderSettings{0.1f, 60, true, std::thread::hardware_concurrency()};
    }


    std::wstring RenderPresets::Low::getName() const {
        return L"Low";
    }

    RenderSettings RenderPresets::Low::genRender() const {
        return RenderSettings{0.3f, 60, true, std::thread::hardware_concurrency()};
    }

    std::wstring RenderPresets::Medium::getName() const {
        return L"Medium";
    }

    RenderSettings RenderPresets::Medium::genRender() const {
        return RenderSettings{0.5f, 60, true, std::thread::hardware_concurrency()};
    }

    std::wstring RenderPresets::High::getName() const {
        return L"High";
    }

    RenderSettings RenderPresets::High::genRender() const {
        return RenderSettings{1.0f, 60, true, std::thread::hardware_concurrency()};
    }

    std::wstring RenderPresets::Ultra::getName() const {
        return L"Ultra";
    }

    RenderSettings RenderPresets::Ultra::genRender() const {
        return RenderSettings{2.0f, 60, true, std::thread::hardware_concurrency()};
    }

    std::wstring RenderPresets::Extreme::getName() const {
        return L"Extreme (DANGER)";
    }

    RenderSettings RenderPresets::Extreme::genRender() const {
        return RenderSettings{4.0f,  60, true, std::thread::hardware_concurrency()};
    }
}
