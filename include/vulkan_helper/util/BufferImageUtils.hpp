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


        static void initImage(VkDevice device, const VkPhysicalDeviceMemoryProperties &memProperties,
                              const ImageInitInfo &iii, VkImage *image,
                              VkDeviceMemory *imageMemory,
                              VkImageView *imageView, VkImageView *mipmappedImageView, VkDeviceSize *capacity);


        static void createImage(VkDevice device, const ImageInitInfo &iii, uint32_t mipLevels, VkImage *image);


        static void allocateImageMemory(VkDevice device, const VkPhysicalDeviceMemoryProperties &memProperties,
                                        const ImageInitInfo &iii, VkImage image, VkDeviceMemory *imageMemory, VkDeviceSize *capacity);

        static void createImageView(VkDevice device, VkImage image, VkImageViewType imageViewType, VkFormat imageFormat, VkImageView *writeImageView);

        static void createMipmappedImageView(VkDevice device, VkImage image, VkImageViewType imageViewType,
                                             VkFormat imageFormat, uint32_t mipLevels, VkImageView *mipmappedImageView);

        static VkImageAspectFlags getAspectMask(VkFormat format);

        static VkDeviceSize initBuffer(Core &core, const BufferInitInfo &bii, VkBuffer *buffer,
                                       VkDeviceMemory *bufferMemory);

        static VkDeviceSize initBuffer(VkDevice device, const VkPhysicalDeviceMemoryProperties &memProperties,
                                       const BufferInitInfo &bii, VkBuffer *buffer, VkDeviceMemory *bufferMemory);

        static void createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage,
                                 VkBuffer *buffer);


        static VkDeviceSize allocateBufferMemory(VkDevice device,
                                                 const VkPhysicalDeviceMemoryProperties &memProperties,
                                                 VkMemoryPropertyFlags properties, VkBuffer buffer,
                                                 VkDeviceMemory *bufferMemory);


        static uint32_t findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties &memProperties,
                                            uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);

        static uint32_t genMipLevels(const ImageInitInfo &iii);

        static uint32_t getAvailableMipLevels(const VkExtent2D &extent);
    };
}
