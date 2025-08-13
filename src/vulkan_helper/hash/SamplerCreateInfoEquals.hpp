//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "SamplerCreateInfoHasher.hpp"

namespace merutilm::vkh {

    struct SamplerCreateInfoEquals {
        using is_transparent = void;

        bool operator()(const VkSamplerCreateInfo &lhs, const VkSamplerCreateInfo &rhs) const {
            return lhs.pNext == rhs.pNext
            && lhs.flags == rhs.flags
            && lhs.magFilter == rhs.magFilter
            && lhs.minFilter == rhs.minFilter
            && lhs.mipmapMode == rhs.mipmapMode
            && lhs.addressModeU  == rhs.addressModeU
            && lhs.addressModeV == rhs.addressModeV
            && lhs.addressModeW == rhs.addressModeW
            && std::bit_cast<uint32_t>(lhs.mipLodBias) == std::bit_cast<uint32_t>(rhs.mipLodBias)
            && lhs.anisotropyEnable == rhs.anisotropyEnable
            && std::bit_cast<uint32_t>(lhs.maxAnisotropy) == std::bit_cast<uint32_t>(rhs.maxAnisotropy)
            && lhs.compareEnable == rhs.compareEnable
            && lhs.compareOp == rhs.compareOp
            && std::bit_cast<uint32_t>(lhs.minLod) == std::bit_cast<uint32_t>(rhs.minLod)
            && std::bit_cast<uint32_t>(lhs.maxLod) == std::bit_cast<uint32_t>(rhs.maxLod)
            && lhs.borderColor == rhs.borderColor
            && lhs.unnormalizedCoordinates == rhs.unnormalizedCoordinates;
        }
    };
}
