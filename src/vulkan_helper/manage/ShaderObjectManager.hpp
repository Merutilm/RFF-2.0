//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <string>
#include "../util/IndexChecker.hpp"

namespace merutilm::vkh {
    struct ShaderObjectManager final {
        std::vector<std::byte> data = {};
        std::vector<uint32_t> elements = {};
        std::vector<uint32_t> sizes = {};
        std::vector<uint32_t> offsets = {};
        std::vector<bool> initialized = {};

        explicit ShaderObjectManager() = default;

        ~ShaderObjectManager() = default;

        ShaderObjectManager(const ShaderObjectManager &) = delete;

        ShaderObjectManager &operator=(const ShaderObjectManager &) = delete;

        ShaderObjectManager(ShaderObjectManager &&) noexcept = delete;

        ShaderObjectManager &operator=(ShaderObjectManager &&) noexcept = delete;


        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserve(uint32_t targetExpected, uint32_t padding = 0);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void reserveArray(uint32_t targetExpected, uint32_t elementCount, uint32_t padding = 0);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void add(uint32_t targetExpected, const T &t, uint32_t padding = 0);

        template<typename T> requires std::is_trivially_copyable_v<T>
        void addArray(uint32_t targetExpected, const std::vector<T> &t, uint32_t padding = 0);
    };


    template<typename T> requires std::is_trivially_copyable_v<T>
    void ShaderObjectManager::reserve(const uint32_t targetExpected, const uint32_t padding) {
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        IndexChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Value Reserve");
        data.resize(data.size() + sizeof(T) + padding);
        sizes.push_back(sizeof(T));
        initialized.push_back(false);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void ShaderObjectManager::reserveArray(const uint32_t targetExpected, const uint32_t elementCount, const uint32_t padding) {
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(elementCount);
        IndexChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Vector Reserve");
        data.resize(data.size() + sizeof(T) * elementCount + padding);
        sizes.push_back(sizeof(T) * elementCount);
        initialized.push_back(false);
    }


    template<typename T> requires std::is_trivially_copyable_v<T>
    void ShaderObjectManager::add(const uint32_t targetExpected, const T &t, const uint32_t padding) {
        const auto raw = reinterpret_cast<const std::byte *>(&t);
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(1);
        IndexChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Value Add");
        data.insert(data.end(), raw, raw + sizeof(T));
        data.resize(data.size() + padding);
        sizes.push_back(sizeof(T));
        initialized.push_back(true);
    }

    template<typename T> requires std::is_trivially_copyable_v<T>
    void ShaderObjectManager::addArray(const uint32_t targetExpected, const std::vector<T> &t, const uint32_t padding) {
        const auto raw = reinterpret_cast<const std::byte *>(t.data());
        offsets.push_back(static_cast<uint32_t>(data.size()));
        elements.push_back(t.size());
        IndexChecker::checkIndexEqual(targetExpected, static_cast<uint32_t>(sizes.size()), "Shader Object Vector Add");
        data.insert(data.end(), raw, raw + sizeof(T) * t.size());
        data.resize(data.size() + padding);
        sizes.push_back(sizeof(T) * t.size());
        initialized.push_back(true);
    }
}
