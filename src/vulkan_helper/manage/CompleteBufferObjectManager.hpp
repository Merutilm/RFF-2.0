//
// Created by Merutilm on 2025-07-15.
//

#pragma once


#include <memory>
#include <string>
#include <vector>
#include "BufferObjectManager.hpp"

namespace merutilm::vkh {
    struct CompleteBufferObjectManager {
        std::vector<std::byte> data = {};
        std::vector<uint32_t> elements = {};
        std::vector<uint32_t> sizes = {};
        std::vector<uint32_t> offsets = {};
        std::vector<bool> initialized = {};

        explicit CompleteBufferObjectManager(std::unique_ptr<BufferObjectManager> &&uploadManager) : data(std::move(
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
                throw exception_invalid_state(
                    "initialize vector first using set(uint32_t target, const std::vector<T> &arr) before use set(uint32_t target, uint32_t arrIndex, T& t)");
            }
            const uint32_t offset = offsets[target] + arrIndex * sizeof(T);
            memcpy(&data[offset], &t, sizeof(T));
        }


        template<typename T> requires std::is_trivially_copyable_v<T>
        void resize(const uint32_t target, const uint32_t elementCount) {
            if (elementCount < elements[target]) {
                data.erase(
                    data.begin() + offsets[target] + elementCount * sizeof(T),
                    data.begin() + offsets[target] + elements[target] * sizeof(T)
                );
            }
            if (elementCount > elements[target]) {
                const auto fill = std::vector<std::byte>((elementCount - elements[target]) * sizeof(T));
                data.insert(data.begin() + offsets[target] + elements[target] * sizeof(T),
                            fill.begin(), fill.end());
            }

            sizes[target] = sizeof(T) * elementCount;
            elements[target] = elementCount;
            uint32_t sizeSum = 0;

            for (uint32_t i = 0; i < static_cast<uint32_t>(sizes.size()); ++i) {
                offsets[i] = sizeSum;
                sizeSum += sizes[i];
            }
        }

        template<typename T> requires std::is_trivially_copyable_v<T>
        void resizeAndClear(const uint32_t target, const uint32_t elementCount) {
            resize<T>(target, elementCount);
            std::fill_n(data.begin() + offsets[target], sizes[target], 0);
        }

        [[nodiscard]] const std::vector<std::byte> &getData() const { return data; }

        [[nodiscard]] uint32_t getOffset(const uint32_t target) const { return offsets[target]; }

        [[nodiscard]] uint32_t getSizeByte(const uint32_t target) const { return sizes[target]; }

        [[nodiscard]] uint32_t getTotalSizeByte() const { return static_cast<uint32_t>(data.size()); }

        [[nodiscard]] uint32_t getObjectCount() const {
            return static_cast<uint32_t>(sizes.size());
        }

        [[nodiscard]] uint32_t getElementCount(const uint32_t target) const {
            return elements[target];
        }
    };
}
