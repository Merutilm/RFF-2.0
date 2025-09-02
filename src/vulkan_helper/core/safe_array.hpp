//
// Created by Merutilm on 2025-08-03.
//

#pragma once
#include "vkh_base.hpp"

namespace merutilm::vkh {
    struct safe_array {
        safe_array() = delete;

        static void check_index(const uint32_t index, const uint32_t size, const std::string &mismatchTitle) {
            if (index >= size) {
                throw exception_invalid_args(std::format("[{}] index out of bounds : {} is larger than {}", mismatchTitle, index, size));
            }
        }
        static void check_size_equal(const size_t lhs, const size_t rhs, const std::string &mismatchTitle) {
            if (lhs != rhs) {
                throw exception_invalid_args(std::format("[{}] size mismatch : {} and {}", mismatchTitle, lhs, rhs));
            }
        }
        static void check_index_equal(const uint32_t lhs, const uint32_t rhs, const std::string &mismatchTitle) {
            if (lhs != rhs) {
                throw exception_invalid_args(std::format("[{}] index mismatch : {} and {}", mismatchTitle, lhs, rhs));
            }
        }
    };
}