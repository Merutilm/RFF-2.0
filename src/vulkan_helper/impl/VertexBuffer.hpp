//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "BufferObject.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class VertexBufferImpl final : public BufferObjectAbstract {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions = {};
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions = {};

    public:
        explicit VertexBufferImpl(CoreRef core, HostDataObjectManager &&manager, BufferLock bufferLock, bool multiframeEnabled);

        ~VertexBufferImpl() override;

        VertexBufferImpl(const VertexBufferImpl &) = delete;

        VertexBufferImpl &operator=(const VertexBufferImpl &) = delete;

        VertexBufferImpl(VertexBufferImpl &&) = delete;

        VertexBufferImpl &operator=(VertexBufferImpl &&) = delete;

        [[nodiscard]] const std::vector<VkVertexInputAttributeDescription> &getVertexInputAttributeDescriptions() const {
            return vertexInputAttributeDescriptions;
        }

        [[nodiscard]] const std::vector<VkVertexInputBindingDescription> &getVertexInputBindingDescriptions() const {
            return bindingDescriptions;
        }

    private:

        void init() override;

        void destroy() override;

        template<typename T>
        static VkFormat getFormat();
    };

    using VertexBuffer = std::unique_ptr<VertexBufferImpl>;
    using VertexBufferPtr = VertexBufferImpl *;
    using VertexBufferRef = VertexBufferImpl &;

    template<typename T>
    VkFormat VertexBufferImpl::getFormat() {
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
