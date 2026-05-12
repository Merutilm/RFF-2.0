//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../data/GraphicsMatrixStagingBuffer.h"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/GPCBloom.hpp"
#include "../vulkan/GPCBloomThreshold.hpp"
#include "../vulkan/GPCColor.hpp"
#include "../vulkan/GPCDownsampleForBlur.hpp"
#include "../vulkan/GPCFog.hpp"
#include "../vulkan/GPCIterationPalette.hpp"
#include "../vulkan/GPCLinearInterpolation.hpp"
#include "../vulkan/GPCPresent.hpp"
#include "../vulkan/GPCSlope.hpp"
#include "../vulkan/GPCStripe.hpp"
#include "../vulkan/RCC0.hpp"
#include "../vulkan/RCC1.hpp"
#include "../vulkan/RCC2.hpp"
#include "../vulkan/RCC3.hpp"
#include "../vulkan/RCC4.hpp"
#include "../vulkan/RCC5.hpp"
#include "../vulkan/RCCDownsampleForBlur.hpp"
#include "../vulkan/RCCPresent.hpp"
#include "vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp"
#include "vulkan_helper/engine/graphics/Renderer.hpp"
#include "vulkan_helper/util/BarrierUtils.hpp"

namespace merutilm::rff2 {
    struct AppRenderer final : public vkh::Renderer {

        GPCIterationPalette *rendererIteration = nullptr;
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

        std::unique_ptr<GraphicsMatrixBuffer<double> > iterationStagingBufferContext = nullptr;

        explicit AppRenderer(vkh::Engine &engine, vkh::WindowContext &wc, vkh::VertexBuffer &vertexBufferPP, vkh::IndexBuffer &indexBufferPP) : Renderer(engine, wc, vertexBufferPP, indexBufferPP) {
            AppRenderer::init();
        }

        ~AppRenderer() override {
            AppRenderer::cleanup();
        }

        AppRenderer(const AppRenderer &) = delete;

        AppRenderer &operator=(const AppRenderer &) = delete;

        AppRenderer(AppRenderer &&) = delete;

        AppRenderer &operator=(AppRenderer &&) = delete;


    private:
        void init() override {

            rendererIteration = vkh::PipelineConfigurator::create<
                GPCIterationPalette>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC0::CONTEXT_INDEX,
                RCC0::SUBPASS_ITERATION_INDEX, vertexBufferPP, indexBufferPP);

            rendererStripe = vkh::PipelineConfigurator::create<GPCStripe>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC1::CONTEXT_INDEX,
                RCC1::SUBPASS_STRIPE_INDEX, vertexBufferPP, indexBufferPP);

            rendererSlope = vkh::PipelineConfigurator::create<GPCSlope>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC2::CONTEXT_INDEX,
                RCC2::SUBPASS_SLOPE_INDEX, vertexBufferPP, indexBufferPP);

            rendererColor = vkh::PipelineConfigurator::create<GPCColor>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC2::CONTEXT_INDEX,
                RCC2::SUBPASS_COLOR_INDEX, vertexBufferPP, indexBufferPP);


            rendererDownsampleForBlur = vkh::PipelineConfigurator::create<GPCDownsampleForBlur>(
                configurators, engine, wc.getAttachmentIndex(),
                RCCDownsampleForBlur::CONTEXT_INDEX,
                RCCDownsampleForBlur::SUBPASS_DOWNSAMPLE_INDEX, vertexBufferPP, indexBufferPP
            );

            rendererBoxBlur = vkh::PipelineConfigurator::create<CPCBoxBlur>(
                configurators, engine, wc.getAttachmentIndex()
            );

            rendererFog = vkh::PipelineConfigurator::create<GPCFog>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC3::CONTEXT_INDEX,
                RCC3::SUBPASS_FOG_INDEX, vertexBufferPP, indexBufferPP
            );

            rendererBloomThreshold = vkh::PipelineConfigurator::create<GPCBloomThreshold>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC3::CONTEXT_INDEX,
                RCC3::SUBPASS_BLOOM_THRESHOLD_INDEX, vertexBufferPP, indexBufferPP
            );

            rendererBloom = vkh::PipelineConfigurator::create<GPCBloom>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC4::CONTEXT_INDEX,
                RCC4::SUBPASS_BLOOM_INDEX, vertexBufferPP, indexBufferPP
            );

            rendererLinearInterpolation = vkh::PipelineConfigurator::create<
                GPCLinearInterpolation>(
                configurators, engine, wc.getAttachmentIndex(),
                RCC5::CONTEXT_INDEX,
                RCC5::SUBPASS_LINEAR_INTERPOLATION_INDEX, vertexBufferPP, indexBufferPP
            );
            rendererPresent = vkh::PipelineConfigurator::create<GPCPresent>(
                configurators, engine, wc.getAttachmentIndex(),
                RCCPresent::CONTEXT_INDEX,
                RCCPresent::SUBPASS_PRESENT_INDEX, vertexBufferPP, indexBufferPP
            );

            finishPipelineInitialization();

        }

        void beforeCmdRender() override {
            vkh::BufferContext::flush(wc.core.getLogicalDevice().getLogicalDeviceHandle(), iterationStagingBufferContext->getContext());
        }


        void cmdRender(const uint32_t swapchainImageIndex) override {

            const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
            const auto mfg = [this](const uint32_t index) {
                return wc.getSharedImageContext().getImageContextMF(index)[frameIndex].image;
            };



            rendererIteration->cmdRefreshIterations(
                wc.getCommandBuffer().getCommandBufferHandle(frameIndex), iterationStagingBufferContext->getContext());

            auto &ctx = rendererIteration->getResultIterationBuffer();
            vkh::BarrierUtils::cmdBufferMemoryBarrier(cbh, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, ctx.buffer, 0, ctx.bufferSize,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] Safe-copy iteration buffer

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC0>(
                wc, frameIndex, {rendererIteration}, {{}});

            // [IN] EXTERNAL
            // [SUBPASS OUT] SECONDARY (iteration)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] SECONDARY
            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC1>(
                           wc, frameIndex, {
                               rendererStripe
                           }, {{}});


            // [IN] SECONDARY
            // [OUT] PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                                          mfg(
                                                                              SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                                          0, 1,
                                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                                          VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC2>(
                wc, frameIndex, {
                    rendererSlope, rendererColor
                }, {{}, {}});

            // [IN] PRIMARY
            // [SUBPASS OUT] SECONDARY (slope)
            // [SUBPASS IN] SECONDARY
            // [OUT] PRIMARY (color)

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            // [BARRIER] PRIMARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<
                RCCDownsampleForBlur>(
                wc, frameIndex, {rendererDownsampleForBlur}, {
                    {GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG}
                });

            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
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
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
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

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] PRIMARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCCDownsampleForBlur>(
                wc, frameIndex, {rendererDownsampleForBlur}, {
                    {GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM}
                });
            // [IN] PRIMARY
            // [OUT] DOWNSAMPLED_PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY),
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
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
            vkh::BarrierUtils::cmdImageMemoryBarrier(
                cbh, mfg(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY),
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_GENERAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY
            // [BARRIER] DOWNSAMPLED_SECONDARY


            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC4>(
                wc, frameIndex, {rendererBloom}, {{}});

            // [IN] SECONDARY
            // [IN] DOWNSAMPLED_SECONDARY
            // [OUT] PRIMARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            vkh::RenderPassFullscreenRecorder::cmdFullscreenInternalRenderPass<RCC5>(
                wc, frameIndex, {rendererLinearInterpolation}, {{}});

            // [IN] PRIMARY
            // [OUT] SECONDARY

            vkh::BarrierUtils::cmdSynchronizeImageWriteToRead(cbh,
                                                              mfg(
                                                                  SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY),
                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                              0, 1,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

            // [BARRIER] SECONDARY

            vkh::RenderPassFullscreenRecorder::cmdFullscreenPresentOnlyRenderPass<RCCPresent>(
                wc, frameIndex, swapchainImageIndex, {rendererPresent}, {{}});

            // [IN] SECONDARY
            // [OUT] EXTERNAL
        }

        void cleanup() override {
            iterationStagingBufferContext = nullptr;
        }
    };
}
