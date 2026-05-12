//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/core/Core.hpp>
#include <vulkan_helper/engine/wrapped/BufferInitInfo.hpp>
#include <vulkan_helper/engine/wrapped/ImageInitInfo.hpp>

namespace merutilm::vkh {
    struct BufferImageUtils {
        BufferImageUtils() = delete;


        static void initImage(Core & core,
                              const ImageInitInfo &iii, VkImage *image,
                              VkDeviceMemory *imageMemory,
                              VkImageView *imageView, VkImageView *mipmappedImageView, VkDeviceSize *capacity);


        static void initImage(const VkDevice device, const VkPhysicalDeviceMemoryProperties &memProperties,
                              const ImageInitInfo &iii, VkImage *image,
                              VkDeviceMemory *imageMemory,
                              VkImageView *imageView, VkImageView *mipmappedImageView, VkDeviceSize *capacity);


        static void createImage(const VkDevice device, const ImageInitInfo &iii, const uint32_t mipLevels, VkImage *image);


        static void allocateImageMemory(const VkDevice device, const VkPhysicalDeviceMemoryProperties &memProperties,
                                        const ImageInitInfo &iii,
                                        const VkImage image, VkDeviceMemory *imageMemory, VkDeviceSize *capacity);

        static void createImageView(const VkDevice device, const VkImage image,
                                    const VkImageViewType imageViewType,
                                    const VkFormat imageFormat, VkImageView *writeImageView);

        static void createMipmappedImageView(const VkDevice device, const VkImage image,
                                             const VkImageViewType imageViewType,
                                             const VkFormat imageFormat, const uint32_t mipLevels, VkImageView *mipmappedImageView);

        static VkImageAspectFlags getAspectMask(const VkFormat format);

        static void initBuffer(Core & core, const BufferInitInfo &bii, VkBuffer *buffer, VkDeviceMemory *bufferMemory);

        static void initBuffer(const VkDevice device, const VkPhysicalDeviceMemoryProperties &memProperties,
                               const BufferInitInfo &bii, VkBuffer *buffer, VkDeviceMemory *bufferMemory);

        static void createBuffer(const VkDevice device, const VkDeviceSize size, const VkBufferUsageFlags usage,
                                 VkBuffer *buffer);


        static void allocateBufferMemory(const VkDevice device, const VkPhysicalDeviceMemoryProperties &memProperties,
                                         const VkMemoryPropertyFlags properties,
                                         const VkBuffer buffer, VkDeviceMemory *bufferMemory);


        static uint32_t findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties &memProperties,
                                            const uint32_t memoryTypeBits,
                                            const VkMemoryPropertyFlags properties);

        static uint32_t genMipLevels(const ImageInitInfo &iii);

        static uint32_t getAvailableMipLevels(const VkExtent2D &extent);
    };
}
