//
// Created by Merutilm on 2025-09-08.
//

#pragma once
#include <vulkan_helper/engine/configurator/PipelineConfigurator.hpp>

#include "vulkan_helper/engine/buffer/IndexBuffer.hpp"
#include "vulkan_helper/engine/buffer/VertexBuffer.hpp"
#include "vulkan_helper/handle/EngineHandler.hpp"

namespace merutilm::vkh {
    struct Renderer : EngineHandler {

        WindowContext & wc;
        VertexBuffer & vertexBufferPP;
        IndexBuffer & indexBufferPP;

        uint32_t frameIndex = 0;
        bool pipelineInitialized = false;
        std::vector<std::unique_ptr<PipelineConfigurator>> configurators = {};

        explicit Renderer(Engine & engine, WindowContext &wc, VertexBuffer &vertexBufferPP, IndexBuffer &indexBufferPP);

        ~Renderer() override;

        Renderer(const Renderer &windowContext) = delete;

        Renderer(Renderer &&) = delete;

        Renderer &operator=(Renderer &&) = delete;

        Renderer &operator=(const Renderer &) = delete;


        [[nodiscard]] uint32_t getFrameIndex() const;

        void finishPipelineInitialization() const;

        void execute();

    private:
        virtual void beforeCmdRender() = 0;

        virtual void cmdRender(uint32_t swapchainImageIndex) = 0;
    };


}
