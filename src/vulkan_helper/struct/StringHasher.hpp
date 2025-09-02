//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "../core/vkh_base.hpp"


namespace merutilm::vkh {

    struct StringHasher {
        using is_transparent = void;

        size_t operator()(const std::string &key) const {
            return std::hash<std::string>{}(key);
        }
    };
}