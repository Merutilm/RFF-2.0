//
// Created by Merutilm on 2025-07-15.
//

#pragma once


#include <memory>
#include <string>
#include <vector>
#include "ShaderObjectManager.hpp"

namespace merutilm::vkh {
    struct CompleteShaderObjectManager {
        std::vector<std::byte> data = {};
        std::vector<uint32_t> elements = {};
        std::vector<uint32_t> sizes = {};
        std::vector<uint32_t> offsets = {};
        std::vector<bool> initialized = {};

        explicit CompleteShaderObjectManager(std::unique_ptr<ShaderObjectManager> &&uploadManager) : data(std::move(
                uploadManager->data)),
            elements(std::move(uploadManager->elements)),
            sizes(std::move(uploadManager->sizes)),
            offsets(std::move(uploadManager->offsets)),
            initialized(std::move(uploadManager->initialized)) {
        }

        template<typename T> requires std::is_trivially_copyable_v<T>
        void set(const uint32_t target, const T &t) {
            if (sizes[target] != sizeof(T)) {
                throw exception_invalid_args(
                    std::format("size mismatch : {} and {} in target {}", sizes[target], sizeof(T), target));
            }
            const uint32_t offset = offsets[target];
            memcpy(&data[offset], &t, sizeof(T));
            initialized[target] = true;
        }


        template<typename T> requires std::is_trivially_copyable_v<T>
        void set(const uint32_t target, const std::vector<T> &arr) {
            const uint32_t size = sizeof(T) * arr.size();
            if (sizes[target] != size) {
                throw exception_invalid_args(
                    std::format("size mismatch : {} and {} in target {}", sizes[target], size, target));
            }
            const uint32_t offset = offsets[target];
            memcpy(&data[offset], arr.data(), size);
            initialized[target] = true;
        }

        template<typename T> requires std::is_trivially_copyable_v<T>
        void set(const uint32_t target, const uint32_t arrIndex, T &t) {
            if (!initialized[target]) {
                throw exception_invalid_state("initialize vector first using set(uint32_t target, const std::vector<T> &arr) before use set(uint32_t target, uint32_t arrIndex, T& t)");
            }
            const uint32_t offset = offsets[target] + arrIndex * sizeof(T);
            memcpy(&data[offset], &t, sizeof(T));
        }

        const std::vector<std::byte> &getData() const { return data; }

        uint32_t getOffset(const uint32_t target) const { return offsets[target]; }

        uint32_t getSizeByte(const uint32_t target) const { return sizes[target]; }

        uint32_t getTotalSizeByte() const { return static_cast<uint32_t>(data.size()); }

        uint32_t getObjectCount() const {
            return static_cast<uint32_t>(sizes.size());
        }

        uint32_t getElementCount(const uint32_t target) const {
            return elements[target];
        }
    };
}
