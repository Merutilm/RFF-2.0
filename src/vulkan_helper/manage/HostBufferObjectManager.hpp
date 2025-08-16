//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <string>
#include "../util/SafeArrayChecker.hpp"

namespace merutilm::vkh {
    struct HostBufferObjectManagerImpl final {
        std::vector<std::byte> data = {};
        std::vector<uint32_t> elements = {};
        std::vector<uint32_t> sizes = {};
        std::vector<uint32_t> offsets = {};

        explicit HostBufferObjectManagerImpl() = default;

        ~HostBufferObjectManagerImpl() = default;

        HostBufferObjectManagerImpl(const HostBufferObjectManagerImpl &) = delete;

        HostBufferObjectManagerImpl &operator=(const HostBufferObjectManagerImpl &) = delete;

        HostBufferObjectManagerImpl(HostBufferObjectManagerImpl &&) noexcept = delete;

        HostBufferObjectManagerImpl &operator=(HostBufferObjectManagerImpl &&) noexcept = delete;


        
    template<typename T> requires std::is_trivially_copyable_v<T>
    void reserve(const uint32_t targetExpected) {
        const size_t size = sizeof(T);
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Value Reserve");
        data.resize(data.size() + size);
        sizes.push_back(size);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void reserveArray(const uint32_t targetExpected, const uint32_t elementCount) {
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(elementCount);
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()),
                                      "Shader Object Vector Reserve");
        data.resize(data.size() + sizeof(T) * elementCount);
        sizes.push_back(sizeof(T) * elementCount);
    }


    template<typename T> requires std::is_trivially_copyable_v<T>
    void add(const uint32_t targetExpected, const T &t) {
        const auto raw = reinterpret_cast<const std::byte *>(&t);
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Value Add");
        data.insert(data.end(), raw, raw + sizeof(T));
        sizes.push_back(sizeof(T));
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void addArray(const uint32_t targetExpected, const std::vector<T> &t) {
        const auto raw = reinterpret_cast<const std::byte *>(t.data());
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(t.size());
        SafeArrayChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Vector Add");
        data.insert(data.end(), raw, raw + sizeof(T) * t.size());
        sizes.push_back(sizeof(T) * t.size());
    }

    };

    using HostBufferObjectManager = std::unique_ptr<HostBufferObjectManagerImpl>;
    using HostBufferObjectManagerPtr = HostBufferObjectManagerImpl *;
    using HostBufferObjectManagerRef = HostBufferObjectManagerImpl &;


}
