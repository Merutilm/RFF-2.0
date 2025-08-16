//
// Created by Merutilm on 2025-08-03.
//

#pragma once
#include <string>
#include <format>
#include <cstdint>
#include "../exception/exception.hpp"

namespace merutilm::vkh {
    struct SafeArrayChecker {
        SafeArrayChecker() = delete;

        static void checkSizeEqual(const size_t lhs, const size_t rhs, const std::string &mismatchTitle) {
            if (lhs != rhs) {
                throw exception_invalid_args(std::format("[{}] size mismatch : {} and {}", mismatchTitle, lhs, rhs));
            }
        }
        static void checkIndexEqual(const uint32_t lhs, const uint32_t rhs, const std::string &mismatchTitle) {
            if (lhs != rhs) {
                throw exception_invalid_args(std::format("[{}] index mismatch : {} and {}", mismatchTitle, lhs, rhs));
            }
        }
    };
}