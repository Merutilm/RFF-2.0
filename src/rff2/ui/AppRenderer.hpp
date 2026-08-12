//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../data/GraphicsMatrixStagingBuffer.h"
#include "../util/RendererUtils.hpp"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/RCCPresentPrepareImgui.hpp"
#include "../vulkan/RenderGraph0.hpp"
#include "../vulkan/RenderGraph3.hpp"
#include "../vulkan/RenderGraph4.hpp"
#include "../vulkan/RenderGraphDownsampleForBlur.hpp"
#include "vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp"
#include "vulkan_helper/engine/internal/RendererImGui.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"
#include "vulkan_helper/util/RenderContextUtils.hpp"

namespace merutilm::rff2 {
    struct AppRenderer final : public vkh::RendererImGui {


        Settings &settings;

        vkh::RenderContext *rc0 = nullptr;
        vkh::RenderContext *rcDownsample = nullptr;
        vkh::RenderContext *rc3 = nullptr;
        vkh::RenderContext *rc4 = nullptr;
        vkh::RenderContext *rcPresent = nullptr;

        RenderGraph0 *rg0 = nullptr;
        RenderGraphDownsampleForBlur *rccDownsample = nullptr;
        RenderGraph3 *rg3 = nullptr;
        RenderGraph4 *rg4 = nullptr;
        RCCPresentPrepareImgui *rccPresentPrepare = nullptr;

        CPCBoxBlur *computeBoxBlur = nullptr;


        std::unique_ptr<GraphicsMatrixBuffer<double>> iterationStagingBufferContext = nullptr;

        template<typename F>
            requires std::is_invocable_v<F>
        explicit AppRenderer(vkh::Engine &engine, vkh::WindowContext &wc, Settings &settings, F &&renderFunc) :
            RendererImGui(engine, wc, std::forward<F>(renderFunc)), settings(settings) {
            AppRenderer::init();
        }

        ~AppRenderer() override { AppRenderer::cleanup(); }

        AppRenderer(const AppRenderer &) = delete;

        AppRenderer &operator=(const AppRenderer &) = delete;

        AppRenderer(AppRenderer &&) = delete;

        AppRenderer &operator=(AppRenderer &&) = delete;


    protected:
        void init() override {

            const auto swapchainImageContextGetter = [this] {
                const auto &swapchain = wc.getSwapchain();
                return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
            };
            computeBoxBlur =
                    vkh::ComputePipelineConfigurator::createComputePipeline<CPCBoxBlur>(configurators, engine, wc);
            rc0 = vkh::RenderContextUtils::attachRenderContext<RenderGraph0>(
                    &rg0, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rcDownsample = vkh::RenderContextUtils::attachRenderContext<RenderGraphDownsampleForBlur>(
                    &rccDownsample, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getBlurredImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                     settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc3 = vkh::RenderContextUtils::attachRenderContext<RenderGraph3>(
                    &rg3, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc4 = vkh::RenderContextUtils::attachRenderContext<RenderGraph4>(
                    &rg4, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rcPresent = vkh::RenderContextUtils::attachRenderContext<RCCPresentPrepareImgui>(
                    &rccPresentPrepare, configurators, engine, wc, [this] { return wc.getSwapchain().getSwapchainExtent(); },
                    swapchainImageContextGetter);

            finishPipelineInitialization();
        }


        void beforeCmdRender() override {
            RendererImGui::beforeCmdRender();
        }


        void cmdRender(const uint32_t swapchainImageIndex) override {

            const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
            const auto mfg = [this](const uint32_t index) {
                return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
            };


            rg0->iterationPalette->cmdRefreshIterations(wc.getCommandBuffer().getCommandBufferHandle(frameIndex),
                                                         iterationStagingBufferContext->getContext());

            auto &ctx = rg0->iterationPalette->getResultIterationBuffer();
            vkh::BarrierUtils::cmdBufferMemoryBarrier(cbh, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                                      ctx.buffer, 0, ctx.bufferSize, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] Safe-copy iteration buffer

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc0, frameIndex);

            // [IN] EXTERNAL
            // [SUBPASS OUT] PRIMARY (color)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY

            rccDownsample->descIndexer = RenderGraphDownsampleForBlur::DescIndexer::FOG;
            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rcDownsample, frameIndex);

            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_GENERAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

            // [BARRIER] DOWNSAMPLED_PRIMARY

            computeBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG);

            // [IN] DOWNSAMPLED_PRIMARY
            // [OUT] DOWNSAMPLED_SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                    VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY
            // [BARRIER] DOWNSAMPLED_SECONDARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc3, frameIndex);

            // [IN] PRIMARY
            // [IN] DOWNSAMPLED_SECONDARY
            // [PRESERVED SUBPASS OUT] SECONDARY
            // [SUBPASS IN] SECONDARY
            // [OUT] PRIMARY (Threshold Masked)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] PRIMARY

            rccDownsample->descIndexer = RenderGraphDownsampleForBlur::DescIndexer::BLOOM;
            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rcDownsample, frameIndex);
            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_GENERAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            // [BARRIER] DOWNSAMPLED_PRIMARY

            computeBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM);

            // [IN] DOWNSAMPLED_PRIMARY
            // [OUT] DOWNSAMPLED_SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                    VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY
            // [BARRIER] DOWNSAMPLED_SECONDARY


            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc4, frameIndex);

            // [IN] SECONDARY
            // [IN] DOWNSAMPLED_SECONDARY
            // [OUT] SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenForSwapchainRenderPass(wc, *rcPresent, frameIndex,
                                                                                   swapchainImageIndex);

            // [IN] SECONDARY
            // [OUT] EXTERNAL

            vkh::BarrierUtils::cmdOverlaySwapchain(wc.getCommandBuffer().getCommandBufferHandle(frameIndex),
                                                   wc.getSwapchain().getSwapchainImages()[swapchainImageIndex]);

            RendererImGui::cmdRender(swapchainImageIndex);
        }

        void cleanup() override { iterationStagingBufferContext = nullptr; }
    };
} // namespace merutilm::rff2
