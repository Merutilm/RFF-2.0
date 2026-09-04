//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../data/GraphicsMatrixBuffer.h"
#include "../util/RendererUtils.hpp"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/CPCInterpolateIsolated.hpp"
#include "../vulkan/CPCIterate.hpp"
#include "../vulkan/RenderGraph0.hpp"
#include "../vulkan/RenderGraph1.hpp"
#include "../vulkan/RenderGraph3.hpp"
#include "../vulkan/RenderGraph4.hpp"
#include "../vulkan/RenderGraphDownsampleForBlur.hpp"
#include "../vulkan/RenderGraphPresentPrepareImgui.hpp"
#include "ZoomAnimationInfo.hpp"
#include "vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp"
#include "vulkan_helper/engine/internal/RendererImGui.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"
#include "vulkan_helper/util/RenderContextUtils.hpp"

namespace merutilm::rff2 {
    struct RFF2Renderer final : public vkh::RendererImGui {


        const Settings &settings;
        const ZoomAnimationInfo &zoomAnimationInfo;

        vkh::RenderContext *rc0 = nullptr;
        vkh::RenderContext *rc1 = nullptr;
        vkh::RenderContext *rcDownsample = nullptr;
        vkh::RenderContext *rc3 = nullptr;
        vkh::RenderContext *rc4 = nullptr;
        vkh::RenderContext *rcPresent = nullptr;

        RenderGraph0 *rg0 = nullptr;
        RenderGraph1 *rg1 = nullptr;
        RenderGraphDownsampleForBlur *rccDownsample = nullptr;
        RenderGraph3 *rg3 = nullptr;
        RenderGraph4 *rg4 = nullptr;
        RenderGraphPresentPrepareImgui *rccPresentPrepare = nullptr;

        CPCIterate<float> *computeIterate = nullptr;
        CPCIterate<fex> *computeIterateFex = nullptr;
        CPCInterpolateIsolated *computeIgnoreIsolated = nullptr;
        CPCBoxBlur *computeBoxBlur = nullptr;


        std::unique_ptr<SharedDescriptorStorage> descriptorStorage = nullptr;
        std::unique_ptr<GraphicsMatrixBuffer<double>> visibleIterationBufferContext = nullptr;
        bool updateStagingBuffer;

        template<typename F>
            requires std::is_invocable_v<F>
        explicit RFF2Renderer(vkh::Engine &engine, vkh::WindowContext &wc, Settings &settings,
                             ZoomAnimationInfo &zoomAnimationInfo, F &&renderFunc) :
            RendererImGui(engine, wc, std::forward<F>(renderFunc)), settings(settings),
            zoomAnimationInfo(zoomAnimationInfo) {
            RFF2Renderer::init();
        }

        ~RFF2Renderer() override { RFF2Renderer::cleanup(); }

        RFF2Renderer(const RFF2Renderer &) = delete;

        RFF2Renderer &operator=(const RFF2Renderer &) = delete;

        RFF2Renderer(RFF2Renderer &&) = delete;

        RFF2Renderer &operator=(RFF2Renderer &&) = delete;


    protected:
        void init() override {

            const auto swapchainImageContextGetter = [this] {
                const auto &swapchain = wc.getSwapchain();
                return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
            };
            descriptorStorage = std::make_unique<SharedDescriptorStorage>(engine, wc);

            computeIterate = vkh::ComputePipelineConfigurator::createComputePipeline<CPCIterate<float>>(configurators, engine, wc);
            computeIterateFex = vkh::ComputePipelineConfigurator::createComputePipeline<CPCIterate<fex>>(configurators, engine, wc);
            computeIgnoreIsolated = vkh::ComputePipelineConfigurator::createComputePipeline<CPCInterpolateIsolated>(configurators, engine, wc);
            computeBoxBlur = vkh::ComputePipelineConfigurator::createComputePipeline<CPCBoxBlur>(configurators, engine, wc);
            rc0 = vkh::RenderContextUtils::attachRenderContext<RenderGraph0>(
                    &rg0, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                                     settings.render.display.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc1 = vkh::RenderContextUtils::attachRenderContext<RenderGraph1>(
                    &rg1, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                                     settings.render.display.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rcDownsample = vkh::RenderContextUtils::attachRenderContext<RenderGraphDownsampleForBlur>(
                    &rccDownsample, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getBlurredImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                                    settings.render.display.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc3 = vkh::RenderContextUtils::attachRenderContext<RenderGraph3>(
                    &rg3, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                                     settings.render.display.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rc4 = vkh::RenderContextUtils::attachRenderContext<RenderGraph4>(
                    &rg4, configurators, engine, wc,
                    [this] {
                        return RendererUtils::getInternalImageExtent(wc.getSwapchain().getSwapchainExtent(),
                                                                     settings.render.display.clarityMultiplier);
                    },
                    swapchainImageContextGetter);
            rcPresent = vkh::RenderContextUtils::attachRenderContext<RenderGraphPresentPrepareImgui>(
                    &rccPresentPrepare, configurators, engine, wc,
                    [this] { return wc.getSwapchain().getSwapchainExtent(); }, swapchainImageContextGetter);

            finishPipelineInitialization();
        }


        void beforeCmdRender() override {
            RendererImGui::beforeCmdRender();
            const float mul = std::pow(10.0f, -zoomAnimationInfo.targetLogZoomOffset);

            descriptorStorage->time->setToCurrentTime(frameIndex);
            descriptorStorage->slope->set(settings.shader.slope, mul, frameIndex);

            computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG,
                                        std::min(1.0f, settings.shader.fog.radius * mul), frameIndex);
            computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM,
                                        std::min(1.0f, settings.shader.bloom.radius * mul), frameIndex);
        }


        void cmdRender(const uint32_t swapchainImageIndex) override {

            const auto &commandBuffer = wc.getCommandBufferGroup().getCommandBuffer(frameIndex);
            const auto mfg = [this](const uint32_t index) {
                return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
            };


            if (updateStagingBuffer) {
                updateStagingBuffer = false;

                descriptorStorage->iteration->cmdRefreshIterations(commandBuffer, visibleIterationBufferContext->getContext());
                auto &ctx = descriptorStorage->iteration->getResultIterationBuffer();
                vkh::BarrierUtils::cmdBufferMemoryBarrier(commandBuffer.getCommandBufferHandle(), VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                                          ctx.buffer, 0, ctx.bufferSize, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            }


            // [BARRIER] Safe-copy iteration buffer


            if (settings.shader.fractal3D.use) {
                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc1, frameIndex);
            } else {
                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc0, frameIndex);
            }


            // [IN] EXTERNAL
            // [SUBPASS OUT] PRIMARY (color)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY

            rccDownsample->descIndexer = RenderGraphDownsampleForBlur::DescIndexer::FOG;
            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rcDownsample, frameIndex);

            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

            // [BARRIER] DOWNSAMPLED_PRIMARY

            computeBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG);

            // [IN] DOWNSAMPLED_PRIMARY
            // [OUT] DOWNSAMPLED_SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                    VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
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
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] PRIMARY

            rccDownsample->descIndexer = RenderGraphDownsampleForBlur::DescIndexer::BLOOM;
            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rcDownsample, frameIndex);
            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                    VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            // [BARRIER] DOWNSAMPLED_PRIMARY

            computeBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM);

            // [IN] DOWNSAMPLED_PRIMARY
            // [OUT] DOWNSAMPLED_SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                    VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY
            // [BARRIER] DOWNSAMPLED_SECONDARY


            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc4, frameIndex);

            // [IN] SECONDARY
            // [IN] DOWNSAMPLED_SECONDARY
            // [OUT] SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                    commandBuffer.getCommandBufferHandle(), mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenForSwapchainRenderPass(wc, *rcPresent, frameIndex,
                                                                                   swapchainImageIndex);

            // [IN] SECONDARY
            // [OUT] EXTERNAL

            vkh::BarrierUtils::cmdOverlaySwapchain(wc.getCommandBufferGroup().getCommandBuffer(frameIndex).getCommandBufferHandle(),
                                                   wc.getSwapchain().getSwapchainImages()[swapchainImageIndex]);

            RendererImGui::cmdRender(swapchainImageIndex);
        }

        void cleanup() override { visibleIterationBufferContext = nullptr; }
    };
} // namespace merutilm::rff2
