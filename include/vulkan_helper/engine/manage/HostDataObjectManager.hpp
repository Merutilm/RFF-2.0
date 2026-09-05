//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    struct HostDataObjectManager final {
        std::vector<std::byte> data = {};
        std::vector<uint64_t> elements = {};
        std::vector<uint64_t> offsets = {};
        std::vector<uint32_t> aligns = {};
        std::vector<uint32_t> strides = {};

        explicit HostDataObjectManager();

        ~HostDataObjectManager();

        HostDataObjectManager(const HostDataObjectManager &) = delete;

        HostDataObjectManager &operator=(const HostDataObjectManager &) = delete;

        HostDataObjectManager(HostDataObjectManager &&) noexcept = default;

        HostDataObjectManager &operator=(HostDataObjectManager &&) noexcept = delete;

        static uint64_t getAlignedOffset(const uint64_t currentDataLen, const uint32_t alignment) {
            return (currentDataLen + alignment - 1) / alignment * alignment;
        }

        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserve(uint32_t targetExpected, uint32_t align = alignof(T));

        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserveArray(uint32_t targetExpected, uint32_t elementCount, uint32_t align = alignof(T), uint32_t stride = sizeof(T));


        template<typename T> requires std::is_trivially_copyable_v<T>
        void add(uint32_t targetExpected, const T &t, uint32_t align = alignof(T));

        template<typename T> requires std::is_trivially_copyable_v<T>
        void addArray(uint32_t targetExpected, const std::vector<T> &t, uint32_t align = alignof(T), uint32_t stride = sizeof(T));
    };


    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::reserve(const uint32_t targetExpected, const uint32_t align) {
        safe_array::check_index_equal(targetExpected, static_cast<uint32_t>(elements.size()),
                                      "Shader Object Value Reserve");

        elements.push_back(1);
        offsets.push_back(getAlignedOffset(data.size(), align));
        data.resize(offsets.back() + sizeof(T));
        aligns.push_back(align);
        strides.push_back(sizeof(T));
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::reserveArray(const uint32_t targetExpected, const uint32_t elementCount, const uint32_t align, const uint32_t stride) {

        safe_array::check_index_equal(targetExpected, static_cast<uint32_t>(elements.size()),
                                      "Shader Object Vector Reserve");
        elements.push_back(elementCount);
        offsets.push_back(getAlignedOffset(data.size(), align));
        data.resize(offsets.back() + stride * elementCount);
        aligns.push_back(align);
        strides.push_back(stride);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::add(const uint32_t targetExpected, const T &t, const uint32_t align) {
        reserve<T>(targetExpected, align);
        const auto raw = reinterpret_cast<const std::byte *>(&t);
        memcpy(data.data() + offsets.back(), raw, sizeof(T));
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::addArray(const uint32_t targetExpected, const std::vector<T> &t, const uint32_t align, const uint32_t stride) {
        reserveArray<T>(targetExpected, static_cast<uint32_t>(t.size()), align, stride);
        const auto raw = reinterpret_cast<const std::byte *>(t.data());
        memcpy(data.data() + offsets.back(), raw, sizeof(T) * t.size());
    }


}
