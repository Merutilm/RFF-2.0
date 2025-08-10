//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <memory>

namespace merutilm::mvk {
    struct UniquePtrEquals {
        template<typename T>
        bool operator()(const std::unique_ptr<T> &lhs, const std::unique_ptr<T> &rhs) const {
            if (lhs == rhs) return true;
            if (lhs == nullptr || rhs == nullptr) return false;
            return *lhs == *rhs;
        }
    };
}
