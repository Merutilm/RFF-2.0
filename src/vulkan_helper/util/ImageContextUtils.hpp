//
// Created by Merutilm on 2025-08-04.
//

#pragma once
#include "../executor/ScopedCommandExecutor.hpp"
#include "../context/ImageContext.hpp"
#include <stb_image.h>
#include "../impl/CommandPool.hpp"
#include "BufferImageUtils.hpp"

namespace merutilm::vkh {
    struct ImageContextUtils {
        static ImageContext imageFromByteColorArray(CoreRef core, CommandPoolRef commandPool,
                                                    const uint32_t width, const uint32_t height,
                                                    const uint32_t texChannels, const bool useMipmap,
                                                    const std::byte *const data) {
            VkBuffer stagingBuffer = VK_NULL_HANDLE;
            VkDeviceMemory stagingMemory = VK_NULL_HANDLE;
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            const VkDeviceSize size = static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * static_cast<
                                          uint64_t>(
                                          texChannels);

            BufferImageUtils::initBuffer(core, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                         &stagingBuffer, &stagingMemory);
            void *mapped = nullptr;
            vkMapMemory(device, stagingMemory, 0, size, 0, &mapped);
            memcpy(mapped, data, size);
            vkUnmapMemory(device, stagingMemory);


            const ImageInitInfo initInfo = {
                .imageType = VK_IMAGE_TYPE_2D,
                .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
                .extent = {width, height, 1},
                .useMipmap = useMipmap,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };

            ImageContext context = ImageContext::createContext(core, initInfo);

            //COMMAND START
            {
                const auto sce = ScopedCommandExecutor(core, commandPool);
                const VkBufferImageCopy copyRegion = {
                    .bufferOffset = 0,
                    .bufferRowLength = 0,
                    .bufferImageHeight = 0,
                    .imageSubresource = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .mipLevel = 0,
                        .baseArrayLayer = 0,
                        .layerCount = 1

                    },
                    .imageOffset = {0, 0, 0},
                    .imageExtent = {width, height, 1}
                }; // copy original mip level
                transformImageLayout(sce.getCommandBufferHandle(), context, VK_ACCESS_HOST_WRITE_BIT,
                                     VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                     0, VK_PIPELINE_STAGE_HOST_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT);

                vkCmdCopyBufferToImage(sce.getCommandBufferHandle(), stagingBuffer, context.image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
                transformImageLayout(sce.getCommandBufferHandle(), context, VK_ACCESS_TRANSFER_WRITE_BIT,
                                     VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                     0, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                if (useMipmap) {
                    generateMipmaps(sce.getCommandBufferHandle(), context);
                }
            }
            //COMMAND END
            vkFreeMemory(device, stagingMemory, nullptr);
            vkDestroyBuffer(device, stagingBuffer, nullptr);

            return context;
        }

        static ImageContext imageFromPath(CoreRef core, CommandPoolRef commandPool, const bool useMipmap,
                                          const std::string_view path) {
            stbi_uc *data = nullptr;
            int width = 0;
            int height = 0;
            int texChannels = 0;
            data = stbi_load(path.data(), &width, &height, &texChannels, STBI_rgb_alpha);
            if (data == nullptr) {
                throw exception_init("Failed to load texture");
            }
            const ImageContext result = imageFromByteColorArray(core, commandPool, width, height, texChannels,
                                                                useMipmap,
                                                                reinterpret_cast<std::byte *>(data));
            stbi_image_free(data);
            return result;
        }

        static void generateMipmaps(const VkCommandBuffer commandBuffer, ImageContext &imageContext) {

            uint32_t mipWidth = imageContext.extent.width;
            uint32_t mipHeight = imageContext.extent.height;
            const auto mipLevels = static_cast<uint32_t>(imageContext.mipImageLayout.size());

            transformImageLayout(commandBuffer, imageContext, 0, VK_ACCESS_TRANSFER_WRITE_BIT,
                                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 0, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT);
            for (uint32_t i = 1; i < mipLevels; ++i) {

                transformImageLayout(commandBuffer, imageContext, 0, VK_ACCESS_TRANSFER_WRITE_BIT,
                                     VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT);

                blitImage(commandBuffer, imageContext, i, mipWidth, mipHeight);

                transformImageLayout(commandBuffer, imageContext, VK_ACCESS_TRANSFER_WRITE_BIT,
                                     VK_ACCESS_SHADER_READ_BIT,
                                     VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i - 1, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

                transformImageLayout(commandBuffer, imageContext, VK_ACCESS_TRANSFER_WRITE_BIT,
                                     VK_ACCESS_TRANSFER_READ_BIT,
                                     VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                     VK_PIPELINE_STAGE_TRANSFER_BIT);
                mipWidth = std::max(mipWidth / 2, static_cast<uint32_t>(1));
                mipHeight = std::max(mipHeight / 2, static_cast<uint32_t>(1));
            }
            transformImageLayout(commandBuffer, imageContext, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels - 1, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                 VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        }


        static void blitImage(const VkCommandBuffer commandBuffer, ImageContext &imageContext,
                              const uint32_t mipLevel, const uint32_t mipWidth, const uint32_t mipHeight) {
            const VkImageBlit blit = {
                .srcSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = mipLevel - 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .srcOffsets = {
                    VkOffset3D{0, 0, 0},
                    VkOffset3D{static_cast<int32_t>(mipWidth), static_cast<int32_t>(mipHeight), 1}
                },
                .dstSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = mipLevel,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .dstOffsets = {
                    VkOffset3D{0, 0, 0},
                    VkOffset3D{
                        std::max(static_cast<int32_t>(mipWidth / 2), 1),
                        std::max(static_cast<int32_t>(mipHeight / 2), 1), 1
                    }
                },
            };

            vkCmdBlitImage(commandBuffer, imageContext.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageContext.image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
            imageContext.mipImageLayout[mipLevel] = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        }

        static void transformImageLayout(const VkCommandBuffer commandBuffer, ImageContext &imageContext,
                                         const VkAccessFlags srcAccessMask,
                                         const VkAccessFlags dstAccessMask,
                                         const VkImageLayout newLayout,
                                         const uint32_t mipLevel,
                                         const VkPipelineStageFlags srcStageMask,
                                         const VkPipelineStageFlags dstStageMask) {
            const VkImageMemoryBarrier barrier = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = srcAccessMask,
                .dstAccessMask = dstAccessMask,
                .oldLayout = imageContext.mipImageLayout[mipLevel],
                .newLayout = newLayout,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .image = imageContext.image,
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = mipLevel,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                }
            };
            vkCmdPipelineBarrier(commandBuffer, srcStageMask,
                                 dstStageMask, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);
            imageContext.mipImageLayout[mipLevel] = newLayout;
        }
    };
}
