//
// Created by Merutilm on 2025-08-04.
//

#pragma once
#include <vulkan_helper/engine/cmd/CommandPool.hpp>
#include <vulkan_helper/engine/context/BufferContext.hpp>
#include <vulkan_helper/engine/context/ImageContext.hpp>
#include <vulkan_helper/util/BufferImageUtils.hpp>

namespace merutilm::vkh {
    struct BufferImageContextUtils {
        static std::vector<VkImage> enumerateImages(const MultiframeImageContext &context);

        /**
        * Creates image from byte color array. result layout is <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.</b>
        */
        static ImageContext imageFromByteColorArray(Core & core, CommandPool & commandPool, VkFormat format,
                                                    uint32_t width, uint32_t height, uint32_t texChannels,
                                                    uint32_t channelBits, bool useMipmap,
                                                    const std::byte *data);

        /**
        * Creates image from file path. result layout is <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.</b>
        */
        static ImageContext imageFromPath(Core & core, CommandPool & commandPool, VkFormat format, bool useMipmap,
                                          std::string_view path);

        /**
         * Copies whole buffer.
         * @param commandBuffer The command buffer to record
         * @param srcBuffer The source buffer to copy.
         * @param dstBuffer The destination buffer to copy.
         */
        static void cmdCopyBuffer(VkCommandBuffer commandBuffer, const BufferContext &srcBuffer,
                                         const BufferContext &dstBuffer);

        /**
         * Copies whole image to buffer.
         * @param commandBuffer The command buffer to record
         * @param image The source image to copy. its layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL</b>
         * @param buffer The destination buffer to copy.
         */
        static void cmdCopyImageToBuffer(VkCommandBuffer commandBuffer, const ImageContext &image,
                                         const BufferContext &buffer);


        /**
         * Generates the Mipmap. Input layout of image Context must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_BIT</b>.
         */
        static void cmdGenerateMipmaps(VkCommandBuffer commandBuffer, const ImageContext &imageContext,
                                       VkImageLayout dstLayout);

        /**
        * Copy source image to destination image. srcContext layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_BIT,</b> and dstContext layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_DST_BIT.</b>
        */
        static void cmdCopyEqualSizeImage(VkCommandBuffer commandBuffer, uint32_t mipLevel,
                                          const ImageContext &srcContext, const ImageContext &dstContext);

        /**
        * Blit image to destination image. srcMipLevel layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_BIT,</b> and dstMipLevel layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_DST_BIT.</b>
        */
        static void cmdBlitImage(VkCommandBuffer commandBuffer, const ImageContext &imageContext, uint32_t srcMipLevel,
                                 uint32_t dstMipLevel);
    };
}
