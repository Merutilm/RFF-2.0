//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "BufferObject.hpp"


namespace merutilm::vkh {
    class VertexBuffer final : public BufferObject {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions = {};
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = {};

    public:
        explicit VertexBuffer(Core & core, HostDataObjectManager &&manager, BufferLock bufferLock, bool multiframeEnabled);

        ~VertexBuffer() override;

        VertexBuffer(const VertexBuffer &) = delete;

        VertexBuffer &operator=(const VertexBuffer &) = delete;

        VertexBuffer(VertexBuffer &&) = delete;

        VertexBuffer &operator=(VertexBuffer &&) = delete;

        [[nodiscard]] const std::vector<VkVertexInputAttributeDescription> &getVertexInputAttributeDescriptions() const {
            return vertexInputAttributeDescriptions;
        }

        [[nodiscard]] const std::vector<VkVertexInputBindingDescription> &getVertexInputBindingDescriptions() const {
            return bindingDescriptions;
        }

    protected:

        void init() override;

        void cleanup() override;

    private:
        template<typename T>
        static VkFormat findFormat();
    };



    template<typename T>
    VkFormat VertexBuffer::findFormat() {
        if constexpr (std::is_same_v<T, float>) {
            return VK_FORMAT_R32_SFLOAT;
        }
        if constexpr (std::is_same_v<T, glm::vec2>) {
            return VK_FORMAT_R32G32_SFLOAT;
        }
        if constexpr (std::is_same_v<T, glm::vec3>) {
            return VK_FORMAT_R32G32B32_SFLOAT;
        }
        if constexpr (std::is_same_v<T, glm::vec4>) {
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        }
        return VK_FORMAT_UNDEFINED;
    }
}
