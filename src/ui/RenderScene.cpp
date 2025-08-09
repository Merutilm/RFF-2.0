//
// Created by Merutilm on 2025-08-08.
//

#include "RenderScene.hpp"

#include "../constants/VulkanWindowConstants.hpp"
#include "../vulkan/configurator/BasicRenderContextConfigurator.hpp"
#include "../vulkan/configurator/Template1PipelineConfigurator.hpp"
#include "../vulkan/configurator/Template2PipelineConfigurator.hpp"


namespace merutilm::rff2 {
    RenderScene::RenderScene(mvk::Engine &engine, const HWND window) : engine(engine), window(window) {
        init();
    }

    RenderScene::~RenderScene() {
        destroy();
    }

    void RenderScene::init() {
        initRenderContext();
        initShaderPrograms();
    }


    void RenderScene::initRenderContext() const {
        const auto &swapchain = *engine.getCore().getWindowContext(
            Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX).swapchain;
        auto configurator = std::make_unique<mvk::BasicRenderContextConfigurator>(engine.getCore(), swapchain);

        engine.attachRenderContext(std::make_unique<mvk::RenderContext>(engine.getCore(),
                                                                        swapchain.populateSwapchainExtent(),
                                                                        std::move(configurator)));
    }

    void RenderScene::initShaderPrograms() {
        shaderPrograms.emplace_back(
            mvk::PipelineConfigurator::createShaderProgram<mvk::Template1PipelineConfigurator>(
                engine, mvk::BasicRenderContextConfigurator::SUBPASS_PRIMARY_INDEX));
        shaderPrograms.emplace_back(
            mvk::PipelineConfigurator::createShaderProgram<mvk::Template2PipelineConfigurator>(
                engine, mvk::BasicRenderContextConfigurator::SUBPASS_SECONDARY_INDEX));
    }


    void RenderScene::destroy() {
        shaderPrograms.clear();
    }
}
