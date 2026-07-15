//
// Created by Merutilm on 7/15/26.
//

#pragma once
#include <vulkan/vulkan.h>

#include "../constants/Constants.hpp"
namespace merutilm::rff2 {
    struct RendererUtils {


        [[nodiscard]] static VkExtent2D getInternalImageExtent(const VkExtent2D &swapchainExtent,
                                                               const float clarityMultiplier) {
            const auto [width, height] = swapchainExtent;
            return {static_cast<uint32_t>(static_cast<float>(width) * clarityMultiplier),
                    static_cast<uint32_t>(static_cast<float>(height) * clarityMultiplier)};
        }

        [[nodiscard]] static VkExtent2D getBlurredImageExtent(const VkExtent2D &swapchainExtent,
                                                              const float clarityMultiplier) {
            const VkExtent2D blurredExtent = getInternalImageExtent(swapchainExtent, clarityMultiplier);
            if (const float rat = Constants::Fractal::GAUSSIAN_MAX_WIDTH / static_cast<float>(blurredExtent.width);
                rat < 1) {
                return {Constants::Fractal::GAUSSIAN_MAX_WIDTH,
                        static_cast<uint32_t>(static_cast<float>(blurredExtent.height) * rat)};
                }
            return blurredExtent;
        }
    };
}