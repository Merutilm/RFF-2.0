//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#ifdef _WIN32
#include "../vulkan/CPC2MapIterationStripe.hpp"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/CPCImageRGBA2BGR.hpp"
#include "../vulkan/GPCBloom.hpp"
#include "../vulkan/GPCBloomThreshold.hpp"
#include "../vulkan/GPCColor.hpp"
#include "../vulkan/GPCDownsampleForBlur.hpp"
#include "../vulkan/GPCFog.hpp"
#include "../vulkan/GPCLinearInterpolation.hpp"
#include "../vulkan/GPCPresent.hpp"
#include "../vulkan/GPCSlope.hpp"
#include "../vulkan/GPCStaticImage2Map.hpp"
#include "../vulkan/RCCStatic2Image.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"
#include "../vulkan/RCC2.hpp"
#include "../vulkan/RCC3.hpp"
#include "../vulkan/RCC4.hpp"
#include "../vulkan/RCC5.hpp"
#include "../vulkan/RCCDownsampleForBlur.hpp"
#include "../vulkan/RCCPresent.hpp"
#include "vulkan_helper/base/vkh.hpp"
#include "vulkan_helper/engine/configurator/PipelineConfigurator.hpp"
#include "vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp"
#include "vulkan_helper/engine/graphics/Renderer.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"

namespace merutilm::rff2 {
    struct VideoWindowRenderer final : public vkh::Renderer {
        GPCStaticImage2Map *rendererStaticImage = nullptr;
        CPC2MapIterationStripe *renderer2MapIterationStripe = nullptr;
        GPCSlope *rendererSlope = nullptr;
        GPCColor *rendererColor = nullptr;
        GPCDownsampleForBlur *rendererDownsampleForBlur = nullptr;
        CPCBoxBlur *rendererBoxBlur = nullptr;
        GPCFog *rendererFog = nullptr;
        GPCBloomThreshold *rendererBloomThreshold = nullptr;
        GPCBloom *rendererBloom = nullptr;
        GPCLinearInterpolation *rendererLinearInterpolation = nullptr;
        CPCImageRGBA2BGR *rendererImageRGBA2BGR = nullptr;
        GPCPresent *rendererPresent = nullptr;
        bool isStaticImages = false;
        float currentSec = 0.0f;
        float currentFrame = 0.0f;

        explicit VideoWindowRenderer(vkh::Engine &engine, vkh::WindowContext &wc) :
            Renderer(engine, wc) {
            VideoWindowRenderer::init();
        }

        ~VideoWindowRenderer() override { VideoWindowRenderer::cleanup(); }

        VideoWindowRenderer(const VideoWindowRenderer &) = delete;

        VideoWindowRenderer &operator=(const VideoWindowRenderer &) = delete;

        VideoWindowRenderer(VideoWindowRenderer &&) = delete;

        VideoWindowRenderer &operator=(VideoWindowRenderer &&) = delete;

    private:
        void init() override {
            rendererStaticImage = vkh::PipelineConfigurator::create<GPCStaticImage2Map>(
                    configurators, engine, wc.getAttachmentIndex(), RCCStatic2Image::CONTEXT_INDEX,
                    RCCStatic2Image::SUBPASS_STATIC_IMAGE_INDEX);

            renderer2MapIterationStripe = vkh::PipelineConfigurator::create<CPC2MapIterationStripe>(
                    configurators, engine, wc.getAttachmentIndex());

            rendererSlope = vkh::PipelineConfigurator::create<GPCSlope>(
                    configurators, engine, wc.getAttachmentIndex(), RCC2::CONTEXT_INDEX,
                    RCC2::SUBPASS_SLOPE_INDEX);

            rendererColor = vkh::PipelineConfigurator::create<GPCColor>(
                    configurators, engine, wc.getAttachmentIndex(), RCC2::CONTEXT_INDEX,
                    RCC2::SUBPASS_COLOR_INDEX);

            rendererDownsampleForBlur = vkh::PipelineConfigurator::create<GPCDownsampleForBlur>(
                    configurators, engine, wc.getAttachmentIndex(), RCCDownsampleForBlur::CONTEXT_INDEX,
                    RCCDownsampleForBlur::SUBPASS_DOWNSAMPLE_INDEX);

            rendererBoxBlur =
                    vkh::PipelineConfigurator::create<CPCBoxBlur>(configurators, engine, wc.getAttachmentIndex());

            rendererFog = vkh::PipelineConfigurator::create<GPCFog>(configurators, engine, wc.getAttachmentIndex(),
                                                                    RCC3::CONTEXT_INDEX, RCC3::SUBPASS_FOG_INDEX,
                                                                    vertexBufferPP, indexBufferPP);

            rendererBloomThreshold = vkh::PipelineConfigurator::create<GPCBloomThreshold>(
                    configurators, engine, wc.getAttachmentIndex(), RCC3::CONTEXT_INDEX,
                    RCC3::SUBPASS_BLOOM_THRESHOLD_INDEX);

            rendererBloom = vkh::PipelineConfigurator::create<GPCBloom>(
                    configurators, engine, wc.getAttachmentIndex(), RCC4::CONTEXT_INDEX,
                    RCC4::SUBPASS_BLOOM_INDEX);

            rendererLinearInterpolation = vkh::PipelineConfigurator::create<GPCLinearInterpolation>(
                    configurators, engine, wc.getAttachmentIndex(), RCC5::CONTEXT_INDEX,
                    RCC5::SUBPASS_LINEAR_INTERPOLATION_INDEX);
            rendererImageRGBA2BGR =
                    vkh::PipelineConfigurator::create<CPCImageRGBA2BGR>(configurators, engine, wc.getAttachmentIndex());
            rendererPresent = vkh::PipelineConfigurator::create<GPCPresent>(
                    configurators, engine, wc.getAttachmentIndex(), RCCPresent::CONTEXT_INDEX,
                    RCCPresent::SUBPASS_PRESENT_INDEX);
            finishPipelineInitialization();
        }


        void beforeCmdRender() override {
            renderer2MapIterationStripe->setTime(currentSec, frameIndex);
            renderer2MapIterationStripe->setCurrentFrame(currentFrame, frameIndex);
        }


        void cmdRender(const uint32_t swapchainImageIndex) override {
            const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
            const auto mfg = [this](const uint32_t index) {
                return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
            };
            if (isStaticImages) {
                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCCStatic2Image>(
                        wc, frameIndex, {rendererStaticImage}, {{}});

                vkh::BarrierUtils::cmdImageMemoryBarrier(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                        VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            } else {
                vkh::BarrierUtils::cmdImageMemoryBarrier(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY), 0,
                        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 0, 1,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                // [BARRIER] Init image

                renderer2MapIterationStripe->cmdRender(cbh, frameIndex, {});

                // [IN] EXTERNAL
                // [OUT] SSBO (Iteration Buffer)
                // [OUT] PRIMARY

                const auto &outputBuffer =
                        renderer2MapIterationStripe->getDescriptor(CPC2MapIterationStripe::SET_OUTPUT_ITERATION)
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

                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC2>(
                        wc, frameIndex, {rendererSlope, rendererColor}, {{}, {}});

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

                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCCDownsampleForBlur>(
                        wc, frameIndex, {rendererDownsampleForBlur},
                        {{GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG}});

                // [IN] PRIMARY
                // [OUT] DOWNSAMPLED_PRIMARY

                vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                        VK_IMAGE_LAYOUT_GENERAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

                // [BARRIER] DOWNSAMPLED_PRIMARY

                rendererBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG);

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

                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC3>(
                        wc, frameIndex, {rendererFog, rendererBloomThreshold}, {{}, {}});

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

                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCCDownsampleForBlur>(
                        wc, frameIndex, {rendererDownsampleForBlur},
                        {{GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM}});
                // [IN] PRIMARY
                // [OUT] DOWNSAMPLED_PRIMARY

                vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                        VK_IMAGE_LAYOUT_GENERAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
                // [BARRIER] DOWNSAMPLED_PRIMARY

                rendererBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM);

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


                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC4>(wc, frameIndex,
                                                                                            {rendererBloom}, {{}});

                // [IN] SECONDARY
                // [IN] DOWNSAMPLED_SECONDARY
                // [OUT] PRIMARY

                vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

                vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC5>(
                        wc, frameIndex, {rendererLinearInterpolation}, {{}});

                // [IN] PRIMARY
                // [OUT] SECONDARY

                vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(
                        cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            }


            // [BARRIER] SECONDARY

            rendererImageRGBA2BGR->cmdRender(cbh, frameIndex, {});

            vkh::RenderPassFullscreenRecorder::cmdFullscreenPresentOnlyRenderPass<RCCPresent>(
                    wc, frameIndex, swapchainImageIndex, {rendererPresent}, {{}});


            // [IN] SECONDARY
            // [OUT] EXTERNAL
        }

        void cleanup() override {
            // noop
        }
    };
} // namespace merutilm::rff2
#endif