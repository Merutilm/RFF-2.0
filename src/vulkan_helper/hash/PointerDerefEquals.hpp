//
// Created by Merutilm on 2025-07-18.
//

#pragma once
namespace merutilm::mvk {
    struct PointerDerefEquals {
        template<typename T>
        bool operator()(const T *lhs, const T *rhs) const {
            if (lhs == rhs) return true;
            if (lhs == nullptr || rhs == nullptr) return false;
            return *lhs == *rhs;
        }
    };
}