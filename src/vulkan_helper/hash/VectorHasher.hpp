//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "../core/vkh_base.hpp"

#include "BoostHasher.hpp"

namespace merutilm::vkh {

    template<typename T, typename Hasher = std::hash<T>>
    struct VectorHasher {
        using is_transparent = void;

        size_t operator()(const std::vector<T>& v) const {
            return operator()(std::span<const T>(v));
        }

        size_t operator()(const std::span<const T>& v) const {
            size_t seed = 0;
            for (size_t i = 0; i < v.size(); i++) {
                BoostHasher::hash(Hasher{}(v[i]), &seed);
            }
            return seed;
        }
    };
}
