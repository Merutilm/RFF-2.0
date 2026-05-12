//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {

    struct EnumHasher {
        using is_transparent = void;

        template<typename T> requires std::is_enum_v<T>
        size_t operator()(const T &key) const {
            return static_cast<std::size_t>(key);
        }
    };
}