//
// Created by Merutilm on 2025-08-04.
//

#pragma once
#include "../executor/ScopedNewCommandBufferExecutor.hpp"
#include "../context/ImageContext.hpp"
#include "../impl/CommandPool.hpp"
#include "BufferImageUtils.hpp"
#include "BarrierUtils.hpp"
#include "../context/BufferContext.hpp"

namespace merutilm::vkh {
    struct BufferImageContextUtils {
        static std::vector<VkImage> enumerateImages(const MultiframeImageContext &context) {
            std::vector<VkImage> images(context.size());
            std::ranges::transform(context, images.begin(), [](const ImageContext &image) { return image.image; });
            return images;
        }

        /**
        * Creates image from byte color array. result layout is <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.</b>
        */
        static ImageContext imageFromByteColorArray(CoreRef core, CommandPoolRef commandPool, const VkFormat format,
                                                    const uint32_t width, const uint32_t height,
                                                    const uint32_t texChannels, const uint32_t channelBits, const bool useMipmap,
                                                    const std::byte *const data) {
            const VkDeviceSize size = static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * static_cast<
                                          uint64_t>(
                                          texChannels) * channelBits / 8;

            auto staging = BufferContext::createContext(core, {
                                             .size = size,
                                             .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                             .properties =
                                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                         });

            BufferContext::mapMemory(core, staging);
            memcpy(staging.mappedMemory, data, size);
            BufferContext::unmapMemory(core, staging);


            const ImageInitInfo initInfo = {
                .imageType = VK_IMAGE_TYPE_2D,
                .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                .imageFormat = format,
                .extent = {width, height, 1},
                .useMipmap = useMipmap,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            };

            const ImageContext context = ImageContext::createContext(core, initInfo);

            //COMMAND START
            {
                const auto sce = ScopedNewCommandBufferExecutor(core, commandPool);
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
                BarrierUtils::cmdImageMemoryBarrier(sce.getCommandBufferHandle(), context.image,
                                                    VK_ACCESS_HOST_WRITE_BIT,
                                                    VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED,
                                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                    0, 1, VK_PIPELINE_STAGE_HOST_BIT,
                                                    VK_PIPELINE_STAGE_TRANSFER_BIT);

                vkCmdCopyBufferToImage(sce.getCommandBufferHandle(), staging.buffer, context.image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

                if (useMipmap) {
                    BarrierUtils::cmdImageMemoryBarrier(sce.getCommandBufferHandle(), context.image,
                                                        VK_ACCESS_TRANSFER_WRITE_BIT,
                                                        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                        0, 1, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                    cmdGenerateMipmaps(sce.getCommandBufferHandle(), context, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
                } else {
                    BarrierUtils::cmdImageMemoryBarrier(sce.getCommandBufferHandle(), context.image,
                                                        VK_ACCESS_TRANSFER_WRITE_BIT,
                                                        VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                        0, 1, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
                }
            }
            //COMMAND END
            BufferContext::destroyContext(core, staging);
            return context;
        }

        /**
        * Creates image from file path. result layout is <b>VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.</b>
        */
        static ImageContext imageFromPath(CoreRef core, CommandPoolRef commandPool, const VkFormat format,
                                          const bool useMipmap,
                                          const std::string_view path) {
            stbi_uc *data = nullptr;
            int width = 0;
            int height = 0;
            int texChannels = 0;
            data = stbi_load(path.data(), &width, &height, &texChannels, STBI_rgb_alpha);
            if (data == nullptr) {
                throw exception_init("Failed to load texture");
            }
            const ImageContext result = imageFromByteColorArray(core, commandPool, format, width, height, texChannels,
                                                                8, useMipmap,
                                                                reinterpret_cast<std::byte *>(data));
            stbi_image_free(data);
            return result;
        }

        /**
         * Copies whole buffer.
         * @param commandBuffer The command buffer to record
         * @param srcBuffer The source buffer to copy.
         * @param dstBuffer The destination buffer to copy.
         */
        static void cmdCopyBuffer(const VkCommandBuffer commandBuffer, const BufferContext &srcBuffer,
                                         const BufferContext &dstBuffer) {
            const VkBufferCopy copyRegion = {
                .srcOffset = 0,
                .dstOffset = 0,
                .size = srcBuffer.bufferSize
            };
            vkCmdCopyBuffer(commandBuffer, srcBuffer.buffer, dstBuffer.buffer, 1,
                                   &copyRegion);
        }
        /**
         * Copies whole image to buffer.
         * @param commandBuffer The command buffer to record
         * @param image The source image to copy. its layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL</b>
         * @param buffer The destination buffer to copy.
         */
        static void cmdCopyImageToBuffer(const VkCommandBuffer commandBuffer, const ImageContext &image,
                                         const BufferContext &buffer) {
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
                .imageExtent = {image.extent.width, image.extent.height, 1}
            };
            vkCmdCopyImageToBuffer(commandBuffer, image.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer.buffer, 1,
                                   &copyRegion);
        }


        /**
         * Generates the Mipmap. Input layout of image Context must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_BIT</b>.
         */
        static void cmdGenerateMipmaps(const VkCommandBuffer commandBuffer, const ImageContext &imageContext,
                                       const VkImageLayout dstLayout) {
            uint32_t mipWidth = imageContext.extent.width;
            uint32_t mipHeight = imageContext.extent.height;
            const auto mipLevels = BufferImageUtils::getAvailableMipLevels(imageContext.extent);

            for (uint32_t i = 1; i < mipLevels; ++i) {
                BarrierUtils::cmdImageMemoryBarrier(commandBuffer, imageContext.image, 0, VK_ACCESS_TRANSFER_WRITE_BIT,
                                                    VK_IMAGE_LAYOUT_UNDEFINED,
                                                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, i, 1,
                                                    VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                    VK_PIPELINE_STAGE_TRANSFER_BIT);

                cmdBlitImage(commandBuffer, imageContext, i - 1, i);

                BarrierUtils::cmdImageMemoryBarrier(commandBuffer, imageContext.image, VK_ACCESS_TRANSFER_WRITE_BIT,
                                                    VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                    dstLayout, i - 1,  1, VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);

                if (i < mipLevels - 1) {
                    BarrierUtils::cmdImageMemoryBarrier(commandBuffer, imageContext.image, VK_ACCESS_TRANSFER_WRITE_BIT,
                                                        VK_ACCESS_TRANSFER_READ_BIT,
                                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i,  1,
                                                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                        VK_PIPELINE_STAGE_TRANSFER_BIT);
                }

                mipWidth = std::max(mipWidth / 2, static_cast<uint32_t>(1));
                mipHeight = std::max(mipHeight / 2, static_cast<uint32_t>(1));
            }
            BarrierUtils::cmdImageMemoryBarrier(commandBuffer, imageContext.image, VK_ACCESS_TRANSFER_WRITE_BIT,
                                                VK_ACCESS_SHADER_READ_BIT,
                                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                dstLayout, mipLevels - 1, 1,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
        }

        /**
        * Copy source image to destination image. srcContext layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_BIT,</b> and dstContext layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_DST_BIT.</b>
        */
        static void cmdCopyEqualSizeImage(const VkCommandBuffer commandBuffer, const uint32_t mipLevel,
                                          const ImageContext &srcContext, const ImageContext &dstContext) {
            const VkImageCopy copyRegion = {
                .srcSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = mipLevel,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .srcOffset = {0, 0, 0},
                .dstSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = mipLevel,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .dstOffset = {0, 0, 0},
                .extent = {
                    .width = srcContext.extent.width,
                    .height = srcContext.extent.height,
                    .depth = 1
                },
            };
            vkCmdCopyImage(commandBuffer, srcContext.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstContext.image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
        }

        /**
        * Blit image to destination image. srcMipLevel layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_SRC_BIT,</b> and dstMipLevel layout must be <b>VK_IMAGE_LAYOUT_TRANSFER_DST_BIT.</b>
        */
        static void cmdBlitImage(const VkCommandBuffer commandBuffer, const ImageContext &imageContext,
                                 const uint32_t srcMipLevel, const uint32_t dstMipLevel) {
            const int32_t srcWidth = static_cast<int32_t>(imageContext.extent.width) >> srcMipLevel;
            const int32_t srcHeight = static_cast<int32_t>(imageContext.extent.height) >> srcMipLevel;

            const int32_t dstWidth = static_cast<int32_t>(imageContext.extent.width) >> dstMipLevel;
            const int32_t dstHeight = static_cast<int32_t>(imageContext.extent.height) >> dstMipLevel;

            const VkImageBlit blit = {
                .srcSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = srcMipLevel,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .srcOffsets = {
                    VkOffset3D{0, 0, 0},
                    VkOffset3D{srcWidth, srcHeight, 1}
                },
                .dstSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = dstMipLevel,
                    .baseArrayLayer = 0,
                    .layerCount = 1
                },
                .dstOffsets = {
                    VkOffset3D{0, 0, 0},
                    VkOffset3D{dstWidth, dstHeight, 1}
                }
            };


            vkCmdBlitImage(commandBuffer, imageContext.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, imageContext.image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        }
    };
}
