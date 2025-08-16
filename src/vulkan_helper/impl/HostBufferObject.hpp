//
// Created by Merutilm on 2025-07-15.
//

#pragma once


#include <memory>
#include <string>
#include <vector>

#include "../manage/HostBufferObjectManager.hpp"

namespace merutilm::vkh {
    struct HostBufferObject {
        std::vector<std::byte> data = {};
        std::vector<uint32_t> elements = {};
        std::vector<uint32_t> sizes = {};
        std::vector<uint32_t> offsets = {};

        explicit HostBufferObject(HostBufferObjectManager &&uploadManager) : data(std::move(
                uploadManager->data)),
            elements(std::move(uploadManager->elements)),
            sizes(std::move(uploadManager->sizes)),
            offsets(std::move(uploadManager->offsets)) {
        }

        ~HostBufferObject() = default;

        HostBufferObject(const HostBufferObject &) = delete;

        HostBufferObject &operator=(const HostBufferObject &) = delete;

        HostBufferObject(HostBufferObject &&) = delete;

        HostBufferObject &operator=(HostBufferObject &&) = delete;

        template<typename T> requires std::is_trivially_copyable_v<T>
        const T &get(const uint32_t target) const {
            SafeArrayChecker::checkSizeEqual(sizes[target], sizeof(T), "Buffer Object get");
            auto view = std::span(data.begin() + offsets[target], data.begin() + offsets[target] + sizes[target]);
            return *reinterpret_cast<const T *>(view.data());
        }

        template<typename T> requires std::is_trivially_copyable_v<T>
        const T &get(const uint32_t target, const uint32_t index) const {
            const uint32_t size = sizeof(T) * elements.size();
            SafeArrayChecker::checkSizeEqual(sizes[target], size, "Buffer Object Vector get");
            auto view = std::span(data.begin() + offsets[target] + sizeof(T) * index, data.begin() + offsets[target] + sizeof(T) * (index + 1));
            return *reinterpret_cast<const T *>(view.data());
        }

        template<typename T> requires std::is_trivially_copyable_v<T>
        void set(const uint32_t target, const T &t) {
            SafeArrayChecker::checkSizeEqual(sizes[target], sizeof(T), "Buffer Object set");
            const uint32_t offset = offsets[target];
            memcpy(&data[offset], &t, sizeof(T));
        }


        template<typename T> requires std::is_trivially_copyable_v<T>
        void set(const uint32_t target, const std::vector<T> &arr) {
            const uint32_t size = sizeof(T) * arr.size();
            SafeArrayChecker::checkSizeEqual(sizes[target], size, "Buffer Object Vector set");
            const uint32_t offset = offsets[target];
            memcpy(&data[offset], arr.data(), size);
        }

        template<typename T> requires std::is_trivially_copyable_v<T>
        void set(const uint32_t target, const uint32_t arrIndex, T &t) {
            const uint32_t offset = offsets[target] + arrIndex * sizeof(T);
            memcpy(&data[offset], &t, sizeof(T));
        }

        void reset(const uint32_t target) {
            std::fill_n(data.begin() + offsets[target], sizes[target], static_cast<std::byte>(0));
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
