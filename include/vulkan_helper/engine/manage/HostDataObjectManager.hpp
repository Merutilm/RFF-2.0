//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    struct HostDataObjectManager final {
        std::vector<std::byte> data = {};
        std::vector<uint32_t> elements = {};
        std::vector<uint32_t> sizes = {};
        std::vector<uint32_t> paddingsPerElem = {};
        std::vector<uint32_t> offsets = {};

        explicit HostDataObjectManager();

        ~HostDataObjectManager();

        HostDataObjectManager(const HostDataObjectManager &) = delete;

        HostDataObjectManager &operator=(const HostDataObjectManager &) = delete;

        HostDataObjectManager(HostDataObjectManager &&) noexcept = default;

        HostDataObjectManager &operator=(HostDataObjectManager &&) noexcept = delete;


        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserve(uint32_t targetExpected, uint32_t padding = 0);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserveArray(uint32_t targetExpected, uint32_t elementCount, uint32_t paddingPerElem = 0);


        template<typename T> requires std::is_trivially_copyable_v<T>
        void add(uint32_t targetExpected, const T &t, uint32_t padding = 0);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void addArray(uint32_t targetExpected, const std::vector<T> &t, uint32_t paddingPerElem = 0);
    };


    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::reserve(const uint32_t targetExpected, const uint32_t padding) {
        const size_t size = sizeof(T);
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        safe_array::check_index_equal(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Value Reserve");
        data.resize(data.size() + size + padding);
        sizes.push_back(size);
        paddingsPerElem.push_back(padding);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::reserveArray(const uint32_t targetExpected, const uint32_t elementCount,
        const uint32_t paddingPerElem) {
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(elementCount);
        safe_array::check_index_equal(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Vector Reserve");
        data.resize(data.size() + (sizeof(T) + paddingPerElem) * elementCount);
        sizes.push_back(sizeof(T) * elementCount);
        paddingsPerElem.push_back(paddingPerElem);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::add(const uint32_t targetExpected, const T &t, const uint32_t padding) {
        const auto raw = reinterpret_cast<const std::byte *>(&t);
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        safe_array::check_index_equal(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Value Add");
        data.insert(data.end(), raw, raw + sizeof(T));
        data.resize(data.size() + padding);
        sizes.push_back(sizeof(T));
        paddingsPerElem.push_back(padding);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostDataObjectManager::addArray(const uint32_t targetExpected, const std::vector<T> &t,
        const uint32_t paddingPerElem) {
        const auto raw = reinterpret_cast<const std::byte *>(t.data());
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(t.size());
        safe_array::check_index_equal(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Vector Add");
        data.insert(data.end(), raw, raw + sizeof(T) * t.size());
        data.resize(data.size() + paddingPerElem * t.size());
        sizes.push_back(sizeof(T) * t.size());
        paddingsPerElem.push_back(paddingPerElem);
    }


}
