//
// Created by Merutilm on 7/27/26.
//

#include "GPC3DFractal.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "vulkan_helper/engine/wrapped/Vertex.hpp"

namespace merutilm::rff2 {
    void GPC3DFractal::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {}

    void GPC3DFractal::cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, vkh::DescIndexPicker &&descIndices) {}

    void GPC3DFractal::pipelineInitialized() {

        using namespace SharedDescriptorTemplate;
        auto &cameraDesc = getDescriptor(SET_CAMERA);

        writeDescriptorMF([&cameraDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            cameraDesc.queue(queue, frameIndex, {}, {DescCamera3D::BINDING_UBO_CAMERA});
        });
    }

    void GPC3DFractal::renderContextRefreshed() {
        // noop
    }

    void GPC3DFractal::resetBuffer(const uint32_t width, const uint32_t height) {

        vkh::VertexBuffer &vbo = getVertexBuffer();
        vkh::IndexBuffer &ibo = getIndexBuffer();
        vkh::HostDataObject &vboHost = vbo.getHostObject();
        vkh::HostDataObject &iboHost = ibo.getHostObject();
        const uint32_t verticesCount = width * height;
        const uint32_t squaresCount = (width - 1) * (height - 1);
        const uint32_t indicesCount = squaresCount * 6;
        std::vector<uint32_t> indices;
        indices.reserve(indicesCount);

        for (uint32_t i = 0; i < width - 1; ++i) {
            for (uint32_t j = 0; j < height - 1; ++j) {
                uint32_t vi = j * width + i;
                indices.push_back(vi);
                indices.push_back(vi + 1);
                indices.push_back(vi + width);
                indices.push_back(vi + 1);
                indices.push_back(vi + width + 1);
                indices.push_back(vi + width);
            }
        }

        vboHost.resizeArray<vkh::Vertex>(0, verticesCount);
        iboHost.resizeArray<uint32_t>(0, indicesCount);
        iboHost.set<uint32_t>(0, indices);
        vbo.reloadBuffer();
        ibo.reloadBuffer();
        vbo.update();
        ibo.update();
        vbo.lock(wc.getCommandPool());
        ibo.lock(wc.getCommandPool());
    }

    void GPC3DFractal::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }

    void GPC3DFractal::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescIteration>(SET_TIME, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
        appendDescriptor<DescCamera3D>(SET_CAMERA, descriptors);
    }

    void GPC3DFractal::configureVertexBuffer(vkh::HostDataObjectManager &som) { som.reserveArray<vkh::Vertex>(0, 1); }

    void GPC3DFractal::configureIndexBuffer(vkh::HostDataObjectManager &som) { som.reserveArray<uint32_t>(0, 1); }
} // namespace merutilm::rff2
