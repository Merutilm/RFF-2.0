//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "../../vulkan_helper/configurator/PipelineConfigurator.hpp"
#include "../../vulkan_helper/core/vkh.hpp"
#include "../../vulkan_helper/executor/RenderPassFullscreenRecorder.hpp"
#include "../../vulkan_helper/impl/Renderer.hpp"
#include "../../vulkan_helper/util/BarrierUtils.hpp"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/CPCIterationPalette2Map.hpp"
#include "../vulkan/GPCBloom.hpp"
#include "../vulkan/GPCBloomThreshold.hpp"
#include "../vulkan/GPCColor.hpp"
#include "../vulkan/GPCDownsampleForBlur.hpp"
#include "../vulkan/GPCFog.hpp"
#include "../vulkan/GPCLinearInterpolation.hpp"
#include "../vulkan/GPCPresent.hpp"
#include "../vulkan/GPCSlope.hpp"
#include "../vulkan/GPCStripe.hpp"
#include "../vulkan/RCC1Vid.hpp"
#include "../vulkan/RCC2Vid.hpp"
#include "../vulkan/RCC3Vid.hpp"
#include "../vulkan/RCC4Vid.hpp"
#include "../vulkan/RCCDownsampleForBlurVid.hpp"
#include "../vulkan/RCCPresentVid.hpp"
#include "../vulkan/SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    struct VideoRenderSceneRenderer final : public vkh::RendererAbstract {
        CPCIterationPalette2Map *rendererIteration2Map = nullptr;
        GPCStripe *rendererStripe = nullptr;
        GPCSlope *rendererSlope = nullptr;
        GPCColor *rendererColor = nullptr;
        GPCDownsampleForBlur *rendererDownsampleForBlur = nullptr;
        CPCBoxBlur *rendererBoxBlur = nullptr;
        GPCFog *rendererFog = nullptr;
        GPCBloomThreshold *rendererBloomThreshold = nullptr;
        GPCBloom *rendererBloom = nullptr;
        GPCLinearInterpolation *rendererLinearInterpolation = nullptr;
        GPCPresent *rendererPresent = nullptr;

        explicit VideoRenderSceneRenderer(vkh::EngineRef engine, const uint32_t windowContextIndex) : RendererAbstract(
            engine, windowContextIndex) {
            VideoRenderSceneRenderer::init();
        }

        ~VideoRenderSceneRenderer() override {
            VideoRenderSceneRenderer::destroy();
        }

        VideoRenderSceneRenderer(const VideoRenderSceneRenderer &) = delete;

        VideoRenderSceneRenderer &operator=(const VideoRenderSceneRenderer &) = delete;

        VideoRenderSceneRenderer(VideoRenderSceneRenderer &&) = delete;

        VideoRenderSceneRenderer &operator=(VideoRenderSceneRenderer &&) = delete;

    private:
        void init() override {
            rendererIteration2Map = vkh::PipelineConfiguratorAbstract::createShaderProgram<CPCIterationPalette2Map>(
                configurators, engine, wc.getAttachmentIndex());

            rendererStripe = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCStripe>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_STRIPE_INDEX);

            rendererSlope = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCSlope>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_SLOPE_INDEX);

            rendererColor = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCColor>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_COLOR_INDEX);

            rendererDownsampleForBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCDownsampleForBlur>(
                configurators, engine, wc.getAttachmentIndex(),
                RCCDownsampleForBlurVid::CONTEXT_INDEX,
                RCCDownsampleForBlurVid::SUBPASS_DOWNSAMPLE_INDEX
            );

            rendererBoxBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<CPCBoxBlur>(
                configurators, engine, wc.getAttachmentIndex()
            );

            rendererFog = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCFog>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC2Vid::CONTEXT_INDEX,
                RCC2Vid::SUBPASS_FOG_INDEX
            );

            rendererBloomThreshold = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloomThreshold>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC2Vid::CONTEXT_INDEX,
                RCC2Vid::SUBPASS_BLOOM_THRESHOLD_INDEX
            );

            rendererBloom = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloom>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC3Vid::CONTEXT_INDEX,
                RCC3Vid::SUBPASS_BLOOM_INDEX
            );

            rendererLinearInterpolation = vkh::PipelineConfiguratorAbstract::createShaderProgram<
                GPCLinearInterpolation>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC4Vid::CONTEXT_INDEX,
                RCC4Vid::SUBPASS_LINEAR_INTERPOLATION_INDEX
            );
            rendererPresent = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCPresent>(
                configurators, engine, wc.getAttachmentIndex(),
                RCCPresentVid::CONTEXT_INDEX,
                RCCPresentVid::SUBPASS_PRESENT_INDEX
            );
            finishPipelineInitialization();
        }


        void beforeCmdRender() override {
        }


        void cmdRender(const uint32_t frameIndex, const uint32_t swapchainImageIndex) override {
            const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
            const auto mfg = [this, &frameIndex](const uint32_t index) {
                return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
            };

            vkh::BarrierUtils::cmdImageMemoryBarrier(
               cbh, mfg(SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_SECONDARY),
               0, VK_ACCESS_SHADER_READ_BIT,
               VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
               0, 1,
               VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
               VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] Init image

            rendererIteration2Map->cmdRender(cbh, frameIndex, {});

            // [IN] EXTERNAL
            // [OUT] SSBO (Iteration Buffer)
            // [OUT] SECONDARY

            const auto &outputBuffer = rendererIteration2Map->getDescriptor(CPCIterationPalette2Map::SET_OUTPUT_ITERATION).get
                    <vkh::ShaderStorage>(0, SharedDescriptorTemplate::DescIteration::BINDING_SSBO_ITERATION_MATRIX)->
                    getBufferContext();

            vkh::BarrierUtils::cmdBufferMemoryBarrier(cbh, VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                                      outputBuffer.buffer, 0, outputBuffer.bufferSize,
                                                      VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                cbh, mfg(SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_SECONDARY),
                VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                0, 1,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SSBO (Result Iteration Buffer)
            // [BARRIER] SECONDARY (Result Image)

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC1Vid>(
                wc, frameIndex, {
                    rendererStripe, rendererSlope,
                    rendererColor
                }, {{}, {}, {}});

            // [IN] SSBO (Iteration Buffer)
            // [IN] SECONDARY
            // [SUBPASS OUT] PRIMARY (stripe)
            // [SUBPASS IN] PRIMARY
            // [SUBPASS OUT] SECONDARY (slope)
            // [SUBPASS IN] SECONDARY
            // [OUT] PRIMARY (color)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<
                RCCDownsampleForBlurVid>(
                wc, frameIndex, {rendererDownsampleForBlur}, {
                    {GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG}
                });

            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_GENERAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

            // [BARRIER] DOWNSAMPLED_PRIMARY

            rendererBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_FOG);

            // [IN] DOWNSAMPLED_PRIMARY
            // [OUT] DOWNSAMPLED_SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                cbh, mfg(SharedImageContextIndices::MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY
            // [BARRIER] DOWNSAMPLED_SECONDARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC2Vid>(
                wc, frameIndex, {rendererFog, rendererBloomThreshold}, {{}, {}});

            // [IN] PRIMARY
            // [IN] DOWNSAMPLED_SECONDARY
            // [PRESERVED SUBPASS OUT] SECONDARY
            // [SUBPASS IN] SECONDARY
            // [OUT] PRIMARY (Threshold Masked)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] PRIMARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCCDownsampleForBlurVid>(
                wc, frameIndex, {rendererDownsampleForBlur}, {
                    {GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM}
                });
            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_GENERAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
            // [BARRIER] DOWNSAMPLED_PRIMARY

            rendererBoxBlur->cmdGaussianBlur(frameIndex, CPCBoxBlur::DESC_INDEX_BLUR_TARGET_BLOOM);

            // [IN] DOWNSAMPLED_PRIMARY
            // [OUT] DOWNSAMPLED_SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_SECONDARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                cbh, mfg(SharedImageContextIndices::MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY
            // [BARRIER] DOWNSAMPLED_SECONDARY


            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC3Vid>(
                wc, frameIndex, {rendererBloom}, {{}});

            // [IN] SECONDARY
            // [IN] DOWNSAMPLED_SECONDARY
            // [OUT] PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC4Vid>(
                wc, frameIndex, {rendererLinearInterpolation}, {{}});

            // [IN] PRIMARY
            // [OUT] SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_SECONDARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenPresentOnlyRenderPass<RCCPresentVid>(
                wc, frameIndex, swapchainImageIndex, {rendererPresent}, {{}});

            // [IN] SECONDARY
            // [OUT] EXTERNAL
        }

        void destroy() override {
            //noop
        }
    };
}
