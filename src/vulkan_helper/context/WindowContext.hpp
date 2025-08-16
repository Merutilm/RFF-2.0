//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <memory>

#include "../impl/GraphicsContextWindow.hpp"
#include "../impl/Surface.hpp"
#include "../impl/Swapchain.hpp"

namespace merutilm::vkh {
    struct WindowContext final {
        std::unique_ptr<GraphicsContextWindow> window = nullptr;
        std::unique_ptr<Surface> surface = nullptr;
        std::unique_ptr<Swapchain> swapchain = nullptr;
    };
}
