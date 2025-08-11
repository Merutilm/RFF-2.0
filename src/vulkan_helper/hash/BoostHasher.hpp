//
// Created by Merutilm on 2025-07-18.
//

#pragma once
namespace merutilm::vkh {
    struct BoostHasher {
        BoostHasher() = delete;

        static void hash(const size_t currHashValue, size_t *seed) {
            *seed ^= currHashValue + 0x9e3779b9 + (*seed << 6) + (*seed >> 2);
        }
    };
}