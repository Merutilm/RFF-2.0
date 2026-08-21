//
// Created by Merutilm on 7/15/26.
//

#pragma once
#include <vulkan/vulkan.h>

#include "../constants/Constants.hpp"
#include "vulkan_helper/engine/descriptor/Descriptor.hpp"
#include "vulkan_helper/engine/wrapped/RenderPassAttachmentReference.hpp"
#include "vulkan_helper/engine/wrapped/Subpassdependency.hpp"
namespace merutilm::rff2 {
    struct RendererUtils {

        inline static const vkh::RenderPassAttachmentReference COLOR_REF_INFO{vkh::RenderPassAttachmentType::COLOR,
                                                                              VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
        inline static const vkh::RenderPassAttachmentReference DEPTH_REF_INFO{vkh::RenderPassAttachmentType::DEPTH_STENCIL,
                                                                              VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
        inline static const vkh::RenderPassAttachmentReference INPUT_REF_INFO{vkh::RenderPassAttachmentType::INPUT,
                                                                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};
        inline static const vkh::SubpassDependency SAMPLER_READ_DEPENDENCY{
                .srcPipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstPipelineStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessFlags = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                .dependencyFlags = 0

        };
        inline static const vkh::SubpassDependency INPUT_READ_DEPENDENCY{
                .srcPipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstPipelineStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessFlags = VK_ACCESS_SHADER_READ_BIT,
                .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT};

        inline static const std::function<vkh::DescIndexPicker()> DEFAULT_DESC_PICKER = []{ return vkh::DescIndexPicker{}; };

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
} // namespace merutilm::rff2
