//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <string>
#include "../util/SafeArrayChecker.hpp"

namespace merutilm::vkh {
    struct HostBufferObjectManager final {
        std::vector<std::byte> data = {};
        std::vector<uint32_t> elements = {};
        std::vector<uint32_t> sizes = {};
        std::vector<uint32_t> offsets = {};

        explicit HostBufferObjectManager() = default;

        ~HostBufferObjectManager() = default;

        HostBufferObjectManager(const HostBufferObjectManager &) = delete;

        HostBufferObjectManager &operator=(const HostBufferObjectManager &) = delete;

        HostBufferObjectManager(HostBufferObjectManager &&) noexcept = delete;

        HostBufferObjectManager &operator=(HostBufferObjectManager &&) noexcept = delete;


        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserve(uint32_t targetExpected);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserveArray(uint32_t targetExpected, uint32_t elementCount);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void add(uint32_t targetExpected, const T &t);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void addArray(uint32_t targetExpected, const std::vector<T> &t);

    };


    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostBufferObjectManager::reserve(const uint32_t targetExpected) {
        const size_t size = sizeof(T);
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Value Reserve");
        data.resize(data.size() + size);
        sizes.push_back(size);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostBufferObjectManager::reserveArray(const uint32_t targetExpected, const uint32_t elementCount) {
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(elementCount);
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Vector Reserve");
        data.resize(data.size() + sizeof(T) * elementCount);
        sizes.push_back(sizeof(T) * elementCount);
    }


    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostBufferObjectManager::add(const uint32_t targetExpected, const T &t) {
        const auto raw = reinterpret_cast<const std::byte *>(&t);
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Value Add");
        data.insert(data.end(), raw, raw + sizeof(T));
        sizes.push_back(sizeof(T));
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void HostBufferObjectManager::addArray(const uint32_t targetExpected, const std::vector<T> &t) {
        const auto raw = reinterpret_cast<const std::byte *>(t.data());
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(t.size());
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Vector Add");
        data.insert(data.end(), raw, raw + sizeof(T) * t.size());
        sizes.push_back(sizeof(T) * t.size());
    }

}
