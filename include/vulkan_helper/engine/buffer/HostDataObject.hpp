//
// Created by Merutilm on 2025-07-15.
//

#pragma once

#include <cstring>
#include <vulkan_helper/base/vkh_base.hpp>

#include <vulkan_helper/engine/manage/HostDataObjectManager.hpp>

namespace merutilm::vkh {
    class HostDataObject final {
        std::vector<std::byte> data;
        std::vector<uint32_t> elements;
        std::vector<uint32_t> offsets;
        std::vector<uint32_t> aligns;
        std::vector<uint32_t> strides;

    public:
        explicit HostDataObject(HostDataObjectManager &&manager) :
            data(std::move(manager.data)), elements(std::move(manager.elements)),
            offsets(std::move(manager.offsets)), aligns(std::move(manager.aligns)),strides(std::move(manager.strides)) {}

        ~HostDataObject() = default;

        HostDataObject(const HostDataObject &) = delete;

        HostDataObject &operator=(const HostDataObject &) = delete;

        HostDataObject(HostDataObject &&) = delete;

        HostDataObject &operator=(HostDataObject &&) = delete;

        template<typename T>
            requires std::is_trivially_copyable_v<T>
        const T &get(uint32_t target) const;

        template<typename T>
            requires std::is_trivially_copyable_v<T>
        const T &get(uint32_t target, uint32_t index) const;

        template<typename T>
            requires std::is_trivially_copyable_v<T>
        void set(uint32_t target, const T &t);


        template<typename T>
            requires std::is_trivially_copyable_v<T>
        void set(uint32_t target, const std::vector<T> &arr);


        template<typename T>
            requires std::is_trivially_copyable_v<T>
        void set(uint32_t target, const T *rawArr);

        template<typename T>
            requires std::is_trivially_copyable_v<T>
        void set(uint32_t target, uint32_t arrIndex, T &t);

        void reset(uint32_t target);

        template<typename T>
            requires std::is_trivially_copyable_v<T>
        void resizeArray(uint32_t target, uint32_t elementCount);

        template<typename T>
            requires std::is_trivially_copyable_v<T>
        void resizeAndClear(uint32_t target, uint32_t elementCount);

        [[nodiscard]] std::vector<std::byte> &getData() { return data; }

        [[nodiscard]] const std::vector<std::byte> &getData() const { return data; }

        [[nodiscard]] uint32_t getOffset(const uint32_t target) const { return offsets[target]; }

        [[nodiscard]] uint32_t getAlign(const uint32_t target) const { return aligns[target]; }

        [[nodiscard]] uint32_t getStride(const uint32_t target) const { return strides[target]; }

        [[nodiscard]] uint32_t getTotalSizeByte() const { return static_cast<uint32_t>(data.size()); }

        [[nodiscard]] uint32_t getObjectCount() const { return static_cast<uint32_t>(elements.size()); }

        [[nodiscard]] uint32_t getElementCount(const uint32_t target) const { return elements[target]; }
    };


    template<typename T>
        requires std::is_trivially_copyable_v<T>
    const T &HostDataObject::get(const uint32_t target) const {
        safe_array::check_size_equal(strides[target], sizeof(T), "Buffer Object get");
        auto view = std::span(data.begin() + offsets[target], data.begin() + offsets[target] + strides[target]);
        return *reinterpret_cast<const T *>(view.data());
    }
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    const T &HostDataObject::get(const uint32_t target, const uint32_t index) const {
        safe_array::check_size_equal(strides[target], sizeof(T), "Buffer Object Vector get");
        auto view = std::span(data.begin() + offsets[target] + strides[target] * index,
                              data.begin() + offsets[target] + strides[target] * (index + 1));
        return *reinterpret_cast<const T *>(view.data());
    }
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    void HostDataObject::set(const uint32_t target, const T &t) {
        safe_array::check_size_equal(strides[target], sizeof(T), "Buffer Object set");
        const uint32_t offset = offsets[target];
        memcpy(&data[offset], &t, strides[target]);
    }
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    void HostDataObject::set(const uint32_t target, const std::vector<T> &arr) {
        safe_array::check_size_equal(strides[target], sizeof(T), "Buffer Object Vector set");
        const uint32_t offset = offsets[target];
        memcpy(&data[offset], arr.data(), strides[target] * arr.size());
    }
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    void HostDataObject::set(const uint32_t target, const T *rawArr) {
        const uint32_t offset = offsets[target];
        memcpy(&data[offset], rawArr, strides[target] * elements[target]);
    }

    template<typename T>
        requires std::is_trivially_copyable_v<T>
    void HostDataObject::set(const uint32_t target, const uint32_t arrIndex, T &t) {
        safe_array::check_size_equal(strides[target], sizeof(T), "Buffer Object Vector Element set");
        const uint32_t offset = offsets[target] + arrIndex * strides[target];
        memcpy(&data[offset], &t, strides[target]);
    }
    inline void HostDataObject::reset(const uint32_t target) {
        std::fill_n(data.begin() + offsets[target], strides[target] * elements[target], static_cast<std::byte>(0));
    }
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    void HostDataObject::resizeArray(const uint32_t target, const uint32_t elementCount) {
        safe_array::check_size_equal(strides[target], sizeof(T), "Buffer Object Vector resize");
        if (elementCount < elements[target]) {
            data.erase(data.begin() + offsets[target] + elementCount * strides[target],
                       data.begin() + offsets[target] + elements[target] * strides[target]);
        }
        if (elementCount > elements[target]) {
            const auto fill = std::vector<std::byte>((elementCount - elements[target]) * strides[target]);
            data.insert(data.begin() + offsets[target] + elements[target] * strides[target], fill.begin(), fill.end());
        }

        elements[target] = elementCount;
        uint32_t sizeSum = 0;

        for (uint32_t i = 0; i < static_cast<uint32_t>(strides.size()); ++i) {
            sizeSum = HostDataObjectManager::getAlignedOffset(sizeSum, aligns[i]);
            offsets[i] = sizeSum;
            sizeSum += strides[i] * elements[i];
        }
    }
    template<typename T>
        requires std::is_trivially_copyable_v<T>
    void HostDataObject::resizeAndClear(const uint32_t target, const uint32_t elementCount) {
        resizeArray<T>(target, elementCount);
        std::fill_n(data.begin() + offsets[target], strides[target] * elementCount, static_cast<std::byte>(0));
    }
} // namespace merutilm::vkh
