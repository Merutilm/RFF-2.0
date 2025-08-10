//
// Created by Merutilm on 2025-08-08.
//

#pragma once
#include <vector>
#include <windows.h>

#include "../vulkan_helper/configurator/PipelineConfigurator.hpp"
#include "../vulkan_helper/handle/EngineHandler.hpp"

namespace merutilm::rff2 {
    class RenderScene final {
        mvk::Engine &engine;
        HWND window;
        std::vector<std::unique_ptr<mvk::PipelineConfigurator>> shaderPrograms = {};

    public:
        explicit RenderScene(mvk::Engine& engine, HWND window);

        ~RenderScene();

        RenderScene(const RenderScene &) = delete;

        RenderScene &operator=(const RenderScene &) = delete;

        RenderScene(RenderScene &&) = delete;

        RenderScene &operator=(RenderScene &&) = delete;

        HWND getWindowHandle() const { return window; }

        std::vector<std::unique_ptr<mvk::PipelineConfigurator>> &getShaderPrograms() { return shaderPrograms; }

    private:
        void init();

        void initRenderContext() const;

        void initShaderPrograms();

        void destroy();
    };
}
