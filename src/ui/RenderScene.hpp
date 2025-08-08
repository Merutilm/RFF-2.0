//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <windows.h>

#include "../vulkan/handle/Handler.hpp"

namespace merutilm::rff2 {
    class RenderScene final : public mvk::Handler {
        HWND window;

    public:
        explicit RenderScene(HWND window);

        ~RenderScene() override;

        RenderScene(const RenderScene &) = delete;

        RenderScene &operator=(const RenderScene &) = delete;

        RenderScene(RenderScene &&) = delete;

        RenderScene &operator=(RenderScene &&) = delete;

        HWND getWindowHandle() const { return window; };

    private:
        void init() override;

        void destroy() override;
    };
}
