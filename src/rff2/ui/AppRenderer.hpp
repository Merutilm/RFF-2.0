//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../data/GraphicsMatrixStagingBuffer.h"
#include "../util/RendererUtils.hpp"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/RCC0.hpp"
#include "../vulkan/RCC1.hpp"
#include "../vulkan/RCC2.hpp"
#include "../vulkan/RCC3.hpp"
#include "../vulkan/RCC4.hpp"
#include "../vulkan/RCC5.hpp"
#include "../vulkan/RCCDownsampleForBlur.hpp"
#include "../vulkan/RCCPresentPrepareImgui.hpp"
#include "vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp"
#include "vulkan_helper/engine/internal/RendererImGui.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"
#include "vulkan_helper/util/RenderContextUtils.hpp"

namespace merutilm::rff2 {
    struct AppRenderer final : public vkh::RendererImGui {


        Settings &settings;

        vkh::RenderContext *rc0;
        vkh::RenderContext *rc1;
        vkh::RenderContext *rc2;
        vkh::RenderContext *rcDownsample;
        vkh::RenderContext *rc3;
        vkh::RenderContext *rc4;
        vkh::RenderContext *rc5;
        vkh::RenderContext *rcPresent;

        RCC0 *rcc0;
        RCC1 *rcc1;
        RCC2 *rcc2;
        RCCDownsampleForBlur *rccDownsample;
        RCC3 *rcc3;
        RCC4 *rcc4;
        RCC5 *rcc5;
        RCCPresentPrepareImgui *rccPresentPrepare;

        CPCBoxBlur *computeBoxBlur;

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
                auto &swapchain = wc.getSwapchain();
                return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
            };
            computeBoxBlur =
                    vkh::ComputePipelineConfigurator::createComputePipeline<CPCBoxBlur>(configurators, engine, wc);
            rc0 = vkh::RenderContextUtils::attachRenderContext<RCC0>(
                    &rcc0, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc1 = vkh::RenderContextUtils::attachRenderContext<RCC1>(
                    &rcc1, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc2 = vkh::RenderContextUtils::attachRenderContext<RCC2>(
                    &rcc2, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rcDownsample = vkh::RenderContextUtils::attachRenderContext<RCCDownsampleForBlur>(
                    &rccDownsample, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getBlurredImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                     settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc3 = vkh::RenderContextUtils::attachRenderContext<RCC3>(
                    &rcc3, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc4 = vkh::RenderContextUtils::attachRenderContext<RCC4>(
                    &rcc4, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                      settings.render.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc5 = vkh::RenderContextUtils::attachRenderContext<RCC5>(
                    &rcc5, configurators, engine, wc,
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
            vkh::BufferContext::flush(wc.core.getLogicalDevice().getLogicalDeviceHandle(),
                                      iterationStagingBufferContext->getContext());
        }


        void cmdRender(const uint32_t swapchainImageIndex) override {

            const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
            const auto mfg = [this](const uint32_t index) {
                return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
            };


            rcc0->iterationPalette->cmdRefreshIterations(wc.getCommandBuffer().getCommandBufferHandle(frameIndex),
                                                         iterationStagingBufferContext->getContext());

            auto &ctx = rcc0->iterationPalette->getResultIterationBuffer();
            vkh::BarrierUtils::cmdBufferMemoryBarrier(cbh, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                                      ctx.buffer, 0, ctx.bufferSize, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] Safe-copy iteration buffer

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc0, frameIndex);

            // [IN] EXTERNAL
            // [SUBPASS OUT] SECONDARY (iteration)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] SECONDARY
            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc1, frameIndex);


            // [IN] SECONDARY
            // [OUT] PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(
                    wc, *rc2, frameIndex);

            // [IN] PRIMARY
            // [SUBPASS OUT] SECONDARY (slope)
            // [SUBPASS IN] SECONDARY
            // [OUT] PRIMARY (color)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY

            rccDownsample->descIndexer = RCCDownsampleForBlur::DescIndexer::FOG;
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

            rccDownsample->descIndexer = RCCDownsampleForBlur::DescIndexer::BLOOM;
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
            // [OUT] PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc5, frameIndex);

            // [IN] PRIMARY
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
