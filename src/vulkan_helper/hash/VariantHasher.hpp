//
// Created by Merutilm on 2025-09-01.
//

#pragma once
#include "../core/vkh_base.hpp"

namespace merutilm::vkh {
    struct VariantHasher {
        using is_transparent = void;

        template<typename... Args>
        size_t operator()(const std::variant<Args...> &v) const {
            auto visitor = []<typename T>(const T &type) {
                return std::hash<T>{}(type);
            };
            return std::visit(visitor, v);
        }
    };
}
