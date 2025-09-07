//
// Created by Merutilm on 2025-09-06.
//

#pragma once
#include "../../vulkan_helper/configurator/PipelineConfigurator.hpp"
#include "../../vulkan_helper/core/vkh.hpp"
#include "../constants/VulkanWindowConstants.hpp"
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
#include "../vulkan/RCCPresent.hpp"

namespace merutilm::rff2 {
    struct VideoRenderSceneShaderPrograms final : public vkh::WindowContextHandler {
        std::vector<vkh::PipelineConfigurator> configurator = {};
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

        explicit VideoRenderSceneShaderPrograms(vkh::WindowContextRef wc) : WindowContextHandler(wc) {
            VideoRenderSceneShaderPrograms::init();
        }

        ~VideoRenderSceneShaderPrograms() override {
            VideoRenderSceneShaderPrograms::destroy();
        }

        VideoRenderSceneShaderPrograms(const VideoRenderSceneShaderPrograms &) = delete;

        VideoRenderSceneShaderPrograms &operator=(const VideoRenderSceneShaderPrograms &) = delete;

        VideoRenderSceneShaderPrograms(VideoRenderSceneShaderPrograms &&) = delete;

        VideoRenderSceneShaderPrograms &operator=(VideoRenderSceneShaderPrograms &&) = delete;


        void init() override {
            rendererIteration2Map = vkh::PipelineConfiguratorAbstract::createShaderProgram<CPCIterationPalette2Map>(
                configurator, wc);

            rendererStripe = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCStripe>(
                configurator, wc,
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_STRIPE_INDEX);

            rendererSlope = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCSlope>(
                configurator, wc,
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_SLOPE_INDEX);

            rendererColor = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCColor>(
                configurator, wc,
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_COLOR_INDEX);

            rendererDownsampleForBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCDownsampleForBlur>(
                configurator, wc,
                RCCDownsampleForBlurVid::CONTEXT_INDEX,
                RCCDownsampleForBlurVid::SUBPASS_DOWNSAMPLE_INDEX
            );

            rendererBoxBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<CPCBoxBlur>(
                configurator, wc
            );

            rendererFog = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCFog>(
                configurator, wc,
                RCC2Vid::CONTEXT_INDEX,
                RCC2Vid::SUBPASS_FOG_INDEX
            );

            rendererBloomThreshold = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloomThreshold>(
                configurator, wc,
                RCC2Vid::CONTEXT_INDEX,
                RCC2Vid::SUBPASS_BLOOM_THRESHOLD_INDEX
            );

            rendererBloom = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloom>(
                configurator, wc,
                RCC3Vid::CONTEXT_INDEX,
                RCC3Vid::SUBPASS_BLOOM_INDEX
            );

            rendererLinearInterpolation = vkh::PipelineConfiguratorAbstract::createShaderProgram<
                GPCLinearInterpolation>(
                configurator, wc,
                RCC4Vid::CONTEXT_INDEX,
                RCC4Vid::SUBPASS_LINEAR_INTERPOLATION_INDEX
            );
            rendererPresent = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCPresent>(
                configurator, wc,
                RCCPresent::CONTEXT_INDEX,
                RCCPresent::SUBPASS_PRESENT_INDEX
            );
        }

        void destroy() override {
            //noop
        }
    };
}
