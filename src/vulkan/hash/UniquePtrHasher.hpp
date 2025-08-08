//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <memory>

namespace merutilm::mvk {

    struct UniquePtrHasher {
        using is_transparent = void;

        template<typename T>
        size_t operator()(const std::unique_ptr<T> &key) const {
            return std::hash<T *>{}(key.get());
        }
    };
}
