//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../constants/VulkanWindowConstants.hpp"
#include "../vulkan/CPCBoxBlur.hpp"
#include "../vulkan/GPCBloom.hpp"
#include "../vulkan/GPCBloomThreshold.hpp"
#include "../vulkan/GPCColor.hpp"
#include "../vulkan/GPCFog.hpp"
#include "../vulkan/GPCIterationPalette.hpp"
#include "../vulkan/GPCLinearInterpolation.hpp"
#include "../vulkan/GPCDownsampleForBlur.hpp"
#include "../vulkan/GPCPresent.hpp"
#include "../vulkan/GPCSlope.hpp"
#include "../vulkan/GPCStripe.hpp"
#include "../vulkan/RCC1.hpp"
#include "../vulkan/RCC2.hpp"
#include "../vulkan/RCC3.hpp"
#include "../vulkan/RCC4.hpp"
#include "../vulkan/RCCDownsampleForBlur.hpp"
#include "../vulkan/RCCPresent.hpp"

namespace merutilm::rff2 {
    struct RenderSceneShaderPrograms final : public vkh::WindowContextHandler {
        std::vector<vkh::PipelineConfigurator> configurator = {};
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

        explicit RenderSceneShaderPrograms(vkh::WindowContextRef wc) : WindowContextHandler(wc) {
            RenderSceneShaderPrograms::init();
        }

        ~RenderSceneShaderPrograms() override {
            RenderSceneShaderPrograms::destroy();
        }

        RenderSceneShaderPrograms(const RenderSceneShaderPrograms &) = delete;

        RenderSceneShaderPrograms &operator=(const RenderSceneShaderPrograms &) = delete;

        RenderSceneShaderPrograms(RenderSceneShaderPrograms &&) = delete;

        RenderSceneShaderPrograms &operator=(RenderSceneShaderPrograms &&) = delete;


        void init() override {
            rendererIteration = vkh::PipelineConfiguratorAbstract::createShaderProgram<
                GPCIterationPalette>(
                configurator, wc,
                RCC1::CONTEXT_INDEX,
                RCC1::SUBPASS_ITERATION_INDEX);

            rendererStripe = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCStripe>(
                configurator, wc,
                RCC1::CONTEXT_INDEX,
                RCC1::SUBPASS_STRIPE_INDEX);

            rendererSlope = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCSlope>(
                configurator, wc,
                RCC1::CONTEXT_INDEX,
                RCC1::SUBPASS_SLOPE_INDEX);

            rendererColor = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCColor>(
                configurator, wc,
                RCC1::CONTEXT_INDEX,
                RCC1::SUBPASS_COLOR_INDEX);


            rendererDownsampleForBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCDownsampleForBlur>(
                configurator, wc,
                RCCDownsampleForBlur::CONTEXT_INDEX,
                RCCDownsampleForBlur::SUBPASS_DOWNSAMPLE_INDEX
            );

            rendererBoxBlur = vkh::PipelineConfiguratorAbstract::createShaderProgram<CPCBoxBlur>(
                configurator, wc
            );

            rendererFog = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCFog>(
                configurator, wc,
                RCC2::CONTEXT_INDEX,
                RCC2::SUBPASS_FOG_INDEX
            );

            rendererBloomThreshold = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloomThreshold>(
                configurator, wc,
                RCC2::CONTEXT_INDEX,
                RCC2::SUBPASS_BLOOM_THRESHOLD_INDEX
            );

            rendererBloom = vkh::PipelineConfiguratorAbstract::createShaderProgram<GPCBloom>(
                configurator, wc,
                RCC3::CONTEXT_INDEX,
                RCC3::SUBPASS_BLOOM_INDEX
            );

            rendererLinearInterpolation = vkh::PipelineConfiguratorAbstract::createShaderProgram<
                GPCLinearInterpolation>(
                configurator, wc,
                RCC4::CONTEXT_INDEX,
                RCC4::SUBPASS_LINEAR_INTERPOLATION_INDEX
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
