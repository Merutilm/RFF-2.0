//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include <vulkan_helper/util/RenderContextUtils.hpp>
#include "../util/RendererUtils.hpp"
#include "../vulkan/CPC2MapIterationStripe.hpp"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/CPCImageRGBA2BGR.hpp"
#include "../vulkan/RenderGraph2.hpp"
#include "../vulkan/RenderGraph3.hpp"
#include "../vulkan/RenderGraph4.hpp"
#include "../vulkan/RenderGraphDownsampleForBlur.hpp"
#include "../vulkan/RenderGraphPresent.hpp"
#include "../vulkan/RenderGraphStatic2Image.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"
#include "vulkan_helper/base/vkh.hpp"
#include "vulkan_helper/engine/configurator/PipelineConfigurator.hpp"
#include "vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp"
#include "vulkan_helper/engine/graphics/Renderer.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"

namespace merutilm::rff2 {
    struct VideoWindowRenderer final : public vkh::Renderer {

        const Settings &settings;
        vkh::RenderContext *rcStatic2 = nullptr;
        vkh::RenderContext *rc2 = nullptr;
        vkh::RenderContext *rcDownsample = nullptr;
        vkh::RenderContext *rc3 = nullptr;
        vkh::RenderContext *rc4 = nullptr;
        vkh::RenderContext *rcPresent = nullptr;

        RenderGraphStatic2Image *rgStatic2 = nullptr;
        RenderGraph2 *rg2 = nullptr;
        RenderGraphDownsampleForBlur *rgDownsample = nullptr;
        RenderGraph3 *rg3 = nullptr;
        RenderGraph4 *rg4 = nullptr;
        RenderGraphPresent *rgPresent = nullptr;

        CPC2MapIterationStripe *compute2MapIterationStripe = nullptr;
        CPCBoxBlur *computeBoxBlur = nullptr;
        CPCImageRGBA2BGR *computeImageRGBA2BGR = nullptr;

        const VkExtent2D &videoExtent;

        bool isStaticImages = false;
        float currentSec = 0.0f;
        float currentFrame = 0.0f;

        explicit VideoWindowRenderer(vkh::Engine &engine, vkh::WindowContext &wc, const Settings &settings,
                                     const VkExtent2D &videoExtent) :
            Renderer(engine, wc), settings(settings), videoExtent(videoExtent) {
            VideoWindowRenderer::init();
        }

        ~VideoWindowRenderer() override { VideoWindowRenderer::cleanup(); }

        VideoWindowRenderer(const VideoWindowRenderer &) = delete;

        VideoWindowRenderer &operator=(const VideoWindowRenderer &) = delete;

        VideoWindowRenderer(VideoWindowRenderer &&) = delete;

        VideoWindowRenderer &operator=(VideoWindowRenderer &&) = delete;

    protected:
        void init() override {
            const auto swapchainImageContextGetter = [this] {
                const auto &swapchain = wc.getSwapchain();
                return vkh::ImageContext::fromSwapchain(wc.core, swapchain);
            };
            compute2MapIterationStripe =
                    vkh::ComputePipelineConfigurator::createComputePipeline<CPC2MapIterationStripe>(configurators,
                                                                                                    engine, wc);

            computeImageRGBA2BGR = vkh::ComputePipelineConfigurator::createComputePipeline<CPCImageRGBA2BGR>(
                    configurators, engine, wc);
            computeBoxBlur =
                    vkh::ComputePipelineConfigurator::createComputePipeline<CPCBoxBlur>(configurators, engine, wc);

            rcStatic2 = vkh::RenderContextUtils::attachRenderContext<RenderGraphStatic2Image>(
                    &rgStatic2, configurators, engine, wc, [this] { return videoExtent; }, swapchainImageContextGetter);
            rc2 = vkh::RenderContextUtils::attachRenderContext<RenderGraph2>(
                    &rg2, configurators, engine, wc, [this] { return videoExtent; }, swapchainImageContextGetter);
            rcDownsample = vkh::RenderContextUtils::attachRenderContext<RenderGraphDownsampleForBlur>(
                    &rgDownsample, configurators, engine, wc,
                    [this] { return RendererUtils::getBlurredImageExtent(videoExtent, 1); },
                    swapchainImageContextGetter);
            rc3 = vkh::RenderContextUtils::attachRenderContext<RenderGraph3>(
                    &rg3, configurators, engine, wc, [this] { return videoExtent; }, swapchainImageContextGetter);
            rc4 = vkh::RenderContextUtils::attachRenderContext<RenderGraph4>(
                    &rg4, configurators, engine, wc, [this] { return videoExtent; }, swapchainImageContextGetter);
            rcPresent = vkh::RenderContextUtils::attachRenderContext<RenderGraphPresent>(
                    &rgPresent, configurators, engine, wc, [this] { return wc.getSwapchain().getSwapchainExtent(); },
                    swapchainImageContextGetter);

            finishPipelineInitialization();
        }

        void beforeCmdRender() override {
            compute2MapIterationStripe->setTime(currentSec, frameIndex);
            compute2MapIterationStripe->setCurrentFrame(currentFrame, frameIndex);
            rg2->slope->setSlope(settings.shader.slope, 1, frameIndex);
            computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG, settings.shader.fog.radius,
                                        frameIndex);
            computeBoxBlur->setBlurInfo(CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM, settings.shader.bloom.radius,
                                        frameIndex);
        }


        void cmdRender(const uint32_t swapchainImageIndex) override {
            const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
            const auto mfg = [this](const uint32_t index) {
                return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
            };
            if (isStaticImages) {
                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rcStatic2, frameIndex);

                vkh::BarrierUtils::cmdImageMemoryBarrier(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY), VK_ACCESS_SHADER_WRITE_BIT,
                        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0,
                        1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            } else {
                vkh::BarrierUtils::cmdImageMemoryBarrier(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY), 0, VK_ACCESS_SHADER_READ_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 0, 1, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                // [BARRIER] Init image

                compute2MapIterationStripe->cmdRender(cbh, frameIndex, {});

                // [IN] EXTERNAL
                // [OUT] SSBO (Iteration Buffer)
                // [OUT] PRIMARY

                const auto &outputBuffer =
                        compute2MapIterationStripe->getDescriptor(CPC2MapIterationStripe::SET_OUTPUT_ITERATION)
                                .get<vkh::ShaderStorage>(
                                        0, SharedDescriptorTemplate::DescIteration::BINDING_SSBO_ITERATION_MATRIX)
                                .getBufferContext();

                vkh::BarrierUtils::cmdBufferMemoryBarrier(cbh, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                                          outputBuffer.buffer, 0, outputBuffer.bufferSize,
                                                          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                vkh::BarrierUtils::cmdImageMemoryBarrier(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY), VK_ACCESS_SHADER_WRITE_BIT,
                        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0,
                        1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

                // [BARRIER] SSBO (Result Iteration Buffer)
                // [BARRIER] PRIMARY (Result Image)

                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass(wc, *rc2, frameIndex);

                // [IN] SSBO (Iteration Buffer)
                // [IN] SECONDARY
                // [SUBPASS OUT] PRIMARY (stripe)
                // [SUBPASS IN] PRIMARY
                // [SUBPASS OUT] SECONDARY (slope)
                // [SUBPASS IN] SECONDARY
                // [OUT] PRIMARY (color)

                vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                // [BARRIER] PRIMARY

                rgDownsample->descIndexer = RenderGraphDownsampleForBlur::DescIndexer::FOG;
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

                rgDownsample->descIndexer = RenderGraphDownsampleForBlur::DescIndexer::BLOOM;
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
            }


            // [BARRIER] SECONDARY

            computeImageRGBA2BGR->cmdRender(cbh, frameIndex, {});

            vkh::RenderPassFullscreenRecorder::cmdFullscreenForSwapchainRenderPass(wc, *rcPresent, frameIndex,
                                                                                   swapchainImageIndex);

            // [IN] SECONDARY
            // [OUT] EXTERNAL
        }

        void cleanup() override {
            // noop
        }
    };
} // namespace merutilm::rff2
