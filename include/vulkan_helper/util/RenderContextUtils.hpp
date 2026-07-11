//
// Created by Merutilm on 7/11/26.
//

#pragma once
#include "vulkan_helper/engine/Engine.hpp"
#include "vulkan_helper/engine/context/RenderContext.hpp"
#include "vulkan_helper/engine/context/WindowContext.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGeneratorBase.hpp"
#include <vulkan_helper/engine/configurator/PipelineConfigurator.hpp>
namespace merutilm::vkh {
    struct RenderContextUtils {
        RenderContextUtils() = delete;


        template<typename T, typename ExtentImgGetter, typename SwapchainImgGetter>
            requires(std::is_base_of_v<RenderPassGraphGeneratorBase, T> &&
                     std::is_invocable_r_v<VkExtent2D, ExtentImgGetter> &&
                     std::is_invocable_r_v<MultiframeImageContext, SwapchainImgGetter>)
        static RenderContext *attachRenderContext(T **result, std::vector<std::unique_ptr<PipelineConfigurator>> &configurators, Engine &engine,
                                           WindowContext &wc, ExtentImgGetter &&extentGetter,
                                           SwapchainImgGetter &&swapchainImageGetter) {

            std::unique_ptr<T> ptr =
                    std::make_unique<T>(engine, wc, std::forward<SwapchainImgGetter>(swapchainImageGetter));
            if (result)
                *result = ptr.get();

            std::unique_ptr<RenderContext> rc =
                    std::make_unique<RenderContext>(engine.getCore(), std::forward<ExtentImgGetter>(extentGetter), std::move(ptr));

            std::vector<std::unique_ptr<PipelineConfigurator>> taken = rc->getGenerator()->takePipelineConfigurators();
            configurators.insert(configurators.end(), std::make_move_iterator(taken.begin()),
                                 std::make_move_iterator(taken.end()));

            return wc.getRenderContexts().emplace_back(std::move(rc)).get();
        }
    };
} // namespace merutilm::vkh
