//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "../core/vkh_base.hpp"

#include "../impl/GraphicsContextWindow.hpp"
#include "../impl/Surface.hpp"
#include "../impl/Swapchain.hpp"

namespace merutilm::vkh {
    struct WindowContext final {
        GraphicsContextWindow window = nullptr;
        Surface surface = nullptr;
        Swapchain swapchain = nullptr;
        SharedImageContext sharedImageContext = nullptr;
    };
}
