//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <algorithm>
#include <array>
#include <vector>

#include "vulkan_helper/engine/context/BufferContext.hpp"
#include "vulkan_helper/handle/CoreHandler.hpp"


namespace merutilm::rff2 {
    template<typename T>
    class GraphicsMatrixBuffer final : vkh::CoreHandler {
        bool updated = false;
        uint16_t width;
        uint16_t height;
        std::vector<T> data;
        vkh::BufferContext context = {};
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;

    public:
        explicit GraphicsMatrixBuffer(vkh::Core &core, const uint16_t width, const uint16_t height,
                                      const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) :
            CoreHandler(core), width(width), height(height), usage(usage), properties(properties) {
            GraphicsMatrixBuffer::init();
        }

        ~GraphicsMatrixBuffer() override { GraphicsMatrixBuffer::cleanup(); }

        GraphicsMatrixBuffer(const GraphicsMatrixBuffer &) = delete;

        GraphicsMatrixBuffer &operator=(const GraphicsMatrixBuffer &) = delete;

        GraphicsMatrixBuffer(GraphicsMatrixBuffer &&) = delete;

        GraphicsMatrixBuffer &operator=(GraphicsMatrixBuffer &&) = delete;

        [[nodiscard]] T operator[](const uint32_t i) const { return vkh::BufferContext::get<T>(context, i); }

        [[nodiscard]] T operator()(const uint32_t x, const uint32_t y) const {
            return vkh::BufferContext::get<T>(context, getIndex(x, y));
        }

        void set(const uint32_t i, const T &value) {
            updated = true;
            data[i] = value;
        }

        void set(const uint32_t x, const uint32_t y, const T &value) {
            updated = true;
            data[getIndex(x, y)] = value;
        }

        bool fill() {
            if (!updated) return false;

            updated = false;
            vkh::BufferContext::fill(context, data);
            return true;
        }

        void markUpdate() {
            updated = true;
        }

        std::vector<T> &getData() { return data; }

        void fill(const std::vector<T> &data) {
            memcpy(this->data.data(), data.data(), data.size() * sizeof(T));
            vkh::BufferContext::fill(context, data);
        }

        void fillZero() const { vkh::BufferContext::fillZero(context); }

        [[nodiscard]] uint32_t getIndex(uint16_t x, uint16_t y) const {
            x = std::clamp(x, static_cast<uint16_t>(0), static_cast<uint16_t>(width - 1));
            y = std::clamp(y, static_cast<uint16_t>(0), static_cast<uint16_t>(height - 1));
            return static_cast<uint32_t>(width) * y + x;
        }

        [[nodiscard]] std::array<uint32_t, 2> getLocation(const uint32_t i) const {
            const uint32_t px = i % width;
            const uint32_t py = i / width;
            return {px, py};
        }

        [[nodiscard]] const vkh::BufferContext &getContext() const { return context; }

        [[nodiscard]] uint16_t getWidth() const { return width; }

        [[nodiscard]] uint16_t getHeight() const { return height; }

        [[nodiscard]] uint32_t getLength() const { return static_cast<uint32_t>(width) * height; }

    protected:
        void init() override {
            updated = false;
            context = vkh::BufferContext::createContext(core, {
                                                                      .size = width * height * sizeof(T),
                                                                      .usage = usage,
                                                                      .properties = properties,
                                                              });
            data.resize(width * height);
            vkh::BufferContext::mapMemory(core, context);
        }

        void cleanup() override {
            vkh::BufferContext::unmapMemory(core, context);
            vkh::BufferContext::destroyContext(core, context);
        }
    };
} // namespace merutilm::rff2
