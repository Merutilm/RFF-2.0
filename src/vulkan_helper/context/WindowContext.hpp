//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <memory>

#include "../def/GraphicsContextWindow.hpp"
#include "../def/Surface.hpp"
#include "../def/Swapchain.hpp"

namespace merutilm::mvk {
    struct WindowContext final {
        std::unique_ptr<GraphicsContextWindow> window = nullptr;
        std::unique_ptr<Surface> surface = nullptr;
        std::unique_ptr<Swapchain> swapchain = nullptr;
    };
}
