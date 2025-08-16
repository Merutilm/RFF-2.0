//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "BoostHasher.hpp"
#include "../impl/CombinedImageSampler.hpp"

namespace merutilm::vkh {

    struct SamplerCreateInfoHasher {
        using is_transparent = void;

        size_t operator()(const VkSamplerCreateInfo &key) const {
            size_t seed = 0;
            BoostHasher::hash(key.flags, &seed);
            BoostHasher::hash(key.magFilter, &seed);
            BoostHasher::hash(key.minFilter, &seed);
            BoostHasher::hash(key.mipmapMode, &seed);
            BoostHasher::hash(key.addressModeU, &seed);
            BoostHasher::hash(key.addressModeV, &seed);
            BoostHasher::hash(key.addressModeW, &seed);
            BoostHasher::hash(std::bit_cast<uint32_t>(key.mipLodBias), &seed);
            BoostHasher::hash(key.anisotropyEnable, &seed);
            BoostHasher::hash(std::bit_cast<uint32_t>(key.maxAnisotropy), &seed);
            BoostHasher::hash(key.compareEnable, &seed);
            BoostHasher::hash(key.compareOp, &seed);
            BoostHasher::hash(std::bit_cast<uint32_t>(key.minLod), &seed);
            BoostHasher::hash(std::bit_cast<uint32_t>(key.maxLod), &seed);
            BoostHasher::hash(key.borderColor, &seed);
            BoostHasher::hash(key.unnormalizedCoordinates, &seed);
            return seed;
        }
    };
}
