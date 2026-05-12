//
// Created by Merutilm on 2026-02-03.
//

#include <vulkan_helper/util/BufferImageUtils.hpp>
namespace merutilm::vkh {
    void BufferImageUtils::initImage(Core & core, const ImageInitInfo &iii, VkImage*image,
        VkDeviceMemory*imageMemory, VkImageView*imageView, VkImageView*mipmappedImageView, VkDeviceSize *capacity) {
        initImage(core.getLogicalDevice().getLogicalDeviceHandle(),
                  core.getPhysicalDevice().getPhysicalDeviceMemoryProperties(), iii, image, imageMemory, imageView, mipmappedImageView, capacity);
    }

    void BufferImageUtils::initImage(const VkDevice device,
        const VkPhysicalDeviceMemoryProperties &memProperties, const ImageInitInfo &iii, VkImage*image,
        VkDeviceMemory*imageMemory, VkImageView*imageView, VkImageView*mipmappedImageView, VkDeviceSize *capacity) {
        const uint32_t mipLevels = genMipLevels(iii);
        createImage(device, iii, mipLevels, image);
        allocateImageMemory(device, memProperties, iii, *image,
                            imageMemory, capacity);
        vkBindImageMemory(device, *image, *imageMemory, 0);
        createImageView(device, *image, iii.imageViewType, iii.imageFormat, imageView);
        if (mipLevels == 1) {
            *mipmappedImageView = *imageView;
        }else {
            createMipmappedImageView(device, *image, iii.imageViewType, iii.imageFormat, mipLevels, mipmappedImageView);
        }
    }

    void BufferImageUtils::createImage(const VkDevice device, const ImageInitInfo &iii,
        const uint32_t mipLevels, VkImage*image) {
        const VkImageCreateInfo imageInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = iii.imageType,
            .format = iii.imageFormat,
            .extent = iii.extent,
            .mipLevels = mipLevels,
            .arrayLayers = iii.arrayLayers,
            .samples = iii.samples,
            .tiling = iii.imageTiling,
            .usage = iii.usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = iii.initialLayout,
        };
        if (vkCreateImage(device, &imageInfo, nullptr, image)) {
            throw exception_init("Failed to create image!");
        }
    }

    void BufferImageUtils::allocateImageMemory(const VkDevice device,
        const VkPhysicalDeviceMemoryProperties &memProperties, const ImageInitInfo &iii, const VkImage image,
        VkDeviceMemory*imageMemory, VkDeviceSize *capacity) {
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);
        const VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryTypeIndex(memProperties, memRequirements.memoryTypeBits, iii.properties),
        };
        *capacity = memRequirements.size;
        if (vkAllocateMemory(device, &allocInfo, nullptr,
                              imageMemory)) {
            throw exception_init("Failed to allocate memory!");
                              }
    }

    void BufferImageUtils::createImageView(const VkDevice device, const VkImage image,
        const VkImageViewType imageViewType, const VkFormat imageFormat, VkImageView*writeImageView) {
        const VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = imageViewType,
            .format = imageFormat,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = getAspectMask(imageFormat),
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        if (vkCreateImageView(device, &viewInfo, nullptr, writeImageView)) {
            throw exception_init("Failed to create image view!");
        }
    }

    void BufferImageUtils::createMipmappedImageView(const VkDevice device, const VkImage image,
        const VkImageViewType imageViewType, const VkFormat imageFormat, const uint32_t mipLevels,
        VkImageView*mipmappedImageView) {
        const VkImageViewCreateInfo viewInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = imageViewType,
            .format = imageFormat,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = getAspectMask(imageFormat),
                .baseMipLevel = 0,
                .levelCount = mipLevels,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        if (vkCreateImageView(device, &viewInfo, nullptr, mipmappedImageView)) {
            throw exception_init("Failed to create image view!");
        }
    }

    VkImageAspectFlags BufferImageUtils::getAspectMask(const VkFormat format) {
        if (format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D16_UNORM) {
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT) {
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        return VK_IMAGE_ASPECT_COLOR_BIT;
    }

    void BufferImageUtils::initBuffer(Core & core, const BufferInitInfo &bii, VkBuffer*buffer,
        VkDeviceMemory*bufferMemory) {
        initBuffer(core.getLogicalDevice().getLogicalDeviceHandle(),
                   core.getPhysicalDevice().getPhysicalDeviceMemoryProperties(), bii, buffer, bufferMemory);
    }

    void BufferImageUtils::initBuffer(const VkDevice device,
        const VkPhysicalDeviceMemoryProperties &memProperties, const BufferInitInfo &bii, VkBuffer*buffer,
        VkDeviceMemory*bufferMemory) {
        createBuffer(device, bii.size, bii.usage, buffer);
        allocateBufferMemory(device, memProperties, bii.properties, *buffer,
                             bufferMemory);
        vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
    }

    void BufferImageUtils::createBuffer(const VkDevice device, const VkDeviceSize size,
        const VkBufferUsageFlags usage, VkBuffer*buffer) {
        const VkBufferCreateInfo bufferInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = size,
            .usage = usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr
        };
        if (vkCreateBuffer(device, &bufferInfo, nullptr, buffer) !=
            VK_SUCCESS) {
            throw exception_init("Failed to create buffer!");
            }
    }

    void BufferImageUtils::allocateBufferMemory(const VkDevice device,
        const VkPhysicalDeviceMemoryProperties &memProperties, const VkMemoryPropertyFlags properties,
        const VkBuffer buffer, VkDeviceMemory*bufferMemory) {
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
        const VkMemoryAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memRequirements.size,
            .memoryTypeIndex = findMemoryTypeIndex(memProperties, memRequirements.memoryTypeBits,
                                                   properties)
        };
        if (vkAllocateMemory(device, &allocInfo, nullptr,
                              bufferMemory) != VK_SUCCESS) {
            throw exception_init("failed to allocate memory!");
                              }
    }

    uint32_t BufferImageUtils::findMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties &memProperties,
        const uint32_t memoryTypeBits, const VkMemoryPropertyFlags properties) {
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((memoryTypeBits & 1 << i) != 0 && //check memory type is equal
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                //check the "propertyFlags" is completely contains "properties"
                return i;
                }
        }

        throw exception_init("failed to find suitable memory type!");
    }

    uint32_t BufferImageUtils::genMipLevels(const ImageInitInfo &iii) {
        if (!iii.useMipmap) return 1;
        return getAvailableMipLevels({iii.extent.width, iii.extent.height});
    }

    uint32_t BufferImageUtils::getAvailableMipLevels(const VkExtent2D &extent) {
        return static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height))) + 1);
    }
}