//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include <algorithm>
#include <vector>
#include <array>

#include "../../vulkan_helper/context/BufferContext.hpp"
#include "../../vulkan_helper/handle/CoreHandler.hpp"


namespace merutilm::rff2 {
    template<typename T>
    class GraphicsMatrixBuffer final : vkh::CoreHandler {
        uint32_t width;
        uint32_t height;
        vkh::BufferContext context = {};
        VkBufferUsageFlags usage;
        VkMemoryPropertyFlags properties;

    public:
        explicit GraphicsMatrixBuffer(vkh::CoreRef core, const uint32_t width,
                                             const uint32_t height, const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties) : CoreHandler(core), width(width), height(height), usage(usage), properties(properties) {
            GraphicsMatrixBuffer::init();
        }

        ~GraphicsMatrixBuffer() override {
            GraphicsMatrixBuffer::destroy();
        }

        GraphicsMatrixBuffer(const GraphicsMatrixBuffer &) = delete;

        GraphicsMatrixBuffer &operator=(const GraphicsMatrixBuffer &) = delete;

        GraphicsMatrixBuffer(GraphicsMatrixBuffer &&) = delete;

        GraphicsMatrixBuffer &operator=(GraphicsMatrixBuffer &&) = delete;

        T operator[](const uint32_t i) const {
            return vkh::BufferContext::get<T>(context, i);
        }

        T operator()(const uint32_t x, const uint32_t y) const {
            return vkh::BufferContext::get<T>(context, getIndex(x, y));
        }

        void set(const uint32_t i, const T &value) {
            vkh::BufferContext::set(context, i, value);
        }

        void set(const uint32_t x, const uint32_t y, const T &value) {
            vkh::BufferContext::set(context, getIndex(x, y), value);
        }

        void fill(const std::vector<double> &vector) const {
            vkh::BufferContext::fill(context, vector);
        }

        void fillZero() const {
            vkh::BufferContext::fillZero(context);
        }

        uint32_t getIndex(uint32_t x, uint32_t y) const {
            x = std::clamp(x, static_cast<uint32_t>(0), width - 1);
            y = std::clamp(y, static_cast<uint32_t>(0), height - 1);
            return static_cast<uint32_t>(width) * y + x;
        }

        std::array<uint32_t, 2> getLocation(const uint32_t i) const {
            const uint32_t px = i % width;
            const uint32_t py = i / width;
            return {px, py};
        }

        const vkh::BufferContext &getContext() const {
            return context;
        }

        uint32_t getWidth() const {
            return width;
        }

        uint32_t getHeight() const {
            return height;
        }

        uint32_t getLength() const {
            return static_cast<uint32_t>(width) * height;
        }

        void init() override {
            context = vkh::BufferContext::createContext(core, {
                                                  .size = width * height * sizeof(T),
                                                  .usage = usage,
                                                  .properties = properties,
                                              });
            vkh::BufferContext::mapMemory(core, context);
        }

        void destroy() override {
            vkh::BufferContext::unmapMemory(core, context);
            vkh::BufferContext::destroyContext(core, context);
        }
    };
}
