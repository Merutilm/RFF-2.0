//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "Instance.hpp"
#include "GraphicsContextWindow.hpp"
#include "../handle/Handler.hpp"

namespace merutilm::vkh {
    class SurfaceImpl final : public Handler {

        InstanceRef instance;
        GraphicsContextWindowRef window;

        VkSurfaceKHR surface = nullptr;

    public:
        explicit SurfaceImpl(InstanceRef instance, GraphicsContextWindowRef window);

        ~SurfaceImpl() override;

        SurfaceImpl(const SurfaceImpl &) = delete;

        SurfaceImpl &operator=(const SurfaceImpl &) = delete;

        SurfaceImpl(SurfaceImpl &&) = delete;

        SurfaceImpl &operator=(SurfaceImpl &&) = delete;

        [[nodiscard]] GraphicsContextWindowRef getTargetWindow() const { return window; }

        [[nodiscard]] VkSurfaceKHR getSurfaceHandle() const { return surface; }

    private:
        void init() override;

        void destroy() override;
    };

    using Surface = std::unique_ptr<SurfaceImpl>;
    using SurfacePtr = SurfaceImpl *;
    using SurfaceRef = SurfaceImpl &;
}
