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

    struct VideoRenderSceneShaderPrograms {
        vkh::EngineRef engine;
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

        explicit VideoRenderSceneShaderPrograms(vkh::EngineRef engine) : engine(engine) {
            init();
        }


        void init() {
            rendererIteration2Map = vkh::PipelineConfiguratorAbstract::createShaderProgram<CPCIterationPalette2Map>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX);

            rendererStripe = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCStripe>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_STRIPE_INDEX);

            rendererSlope = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCSlope>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_SLOPE_INDEX);

            rendererColor = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCColor>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCC1Vid::CONTEXT_INDEX,
                RCC1Vid::SUBPASS_COLOR_INDEX);
            
            rendererDownsampleForBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCDownsampleForBlur>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCCDownsampleForBlurVid::CONTEXT_INDEX,
                RCCDownsampleForBlurVid::SUBPASS_DOWNSAMPLE_INDEX
            );

            rendererBoxBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<CPCBoxBlur>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX
            );

            rendererFog = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCFog>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCC2Vid::CONTEXT_INDEX,
                RCC2Vid::SUBPASS_FOG_INDEX
            );

            rendererBloomThreshold = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloomThreshold>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCC2Vid::CONTEXT_INDEX,
                RCC2Vid::SUBPASS_BLOOM_THRESHOLD_INDEX
            );

            rendererBloom = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloom>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCC3Vid::CONTEXT_INDEX,
                RCC3Vid::SUBPASS_BLOOM_INDEX
            );

            rendererLinearInterpolation = vkh::PipelineConfiguratorAbstract::createShaderProgram<
                GPCLinearInterpolation>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCC4Vid::CONTEXT_INDEX,
                RCC4Vid::SUBPASS_LINEAR_INTERPOLATION_INDEX
            );
            rendererPresent = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCPresent>(
                configurator, engine, Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX,
                RCCPresent::CONTEXT_INDEX,
                RCCPresent::SUBPASS_PRESENT_INDEX
            );
        }
    };
}
