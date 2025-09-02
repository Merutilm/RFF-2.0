//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "../core/vkh_base.hpp"

namespace merutilm::vkh {
    struct UniquePtrEquals {

        using is_transparent = void;

        template<typename T>
        bool operator()(const std::unique_ptr<T> &lhs, const std::unique_ptr<T> &rhs) const {
            if (lhs == rhs) return true;
            if (lhs == nullptr || rhs == nullptr) return false;
            return *lhs == *rhs;
        }
    };
}
