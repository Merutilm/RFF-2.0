//
// Created by Merutilm on 7/27/26.
//

#include "GPC3DFractal.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "SharedDescriptorTemplate.hpp"
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "vulkan_helper/engine/wrapped/Vertex.hpp"

namespace merutilm::rff2 {
    void GPC3DFractal::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {}

    void GPC3DFractal::cmdRender(const VkCommandBuffer cbh, const uint32_t frameIndex,
                                 vkh::DescIndexPicker &&descIndices) {
        pipeline->cmdBindAll(cbh, frameIndex, std::move(descIndices));
        cmdPushAll(cbh);
        cmdDraw(cbh, frameIndex, TARGET_IBO);
    }

    void GPC3DFractal::pipelineInitialized() {

        using namespace SharedDescriptorTemplate;
        auto &cameraDesc = getDescriptor(SET_CAMERA);
        auto &fractal3dDesc = getDescriptor(SET_FRACTAL3D);

        writeDescriptorMF([&cameraDesc, &fractal3dDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            cameraDesc.queue(queue, frameIndex, {}, {DescCamera3D::BINDING_UBO_CAMERA});
            fractal3dDesc.queue(queue, frameIndex, {}, {DescFractal3D::BINDING_UBO_F3D});
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
                indices.push_back(vi + width);
                indices.push_back(vi + 1);
                indices.push_back(vi + 1);
                indices.push_back(vi + width);
                indices.push_back(vi + width + 1);
            }
        }

        vboHost.resizeArray<vkh::Vertex>(0, verticesCount);
        iboHost.resizeArray<uint32_t>(0, indicesCount);
        iboHost.set<uint32_t>(0, indices);
        vbo.reloadBuffer();
        ibo.reloadBuffer();
        updateBufferMF([&vbo, &ibo](const uint32_t frameIndex) {
            vbo.updateMF(frameIndex);
            ibo.updateMF(frameIndex);
        });
        vbo.lock(wc.getCommandPool());
        ibo.lock(wc.getCommandPool());

    }

    void GPC3DFractal::setFractal3D(const ShdFractal3DSettings &fractal3DSettings) const {

        using namespace SharedDescriptorTemplate;
        auto &cameraDesc = getDescriptor(SET_CAMERA);
        auto &cameraUBO = cameraDesc.get<vkh::Uniform>(0, DescCamera3D::BINDING_UBO_CAMERA);
        auto &cameraUBOHost = cameraUBO.getHostObject();

        auto &f3dDesc = getDescriptor(SET_FRACTAL3D);
        auto &f3dUBO = f3dDesc.get<vkh::Uniform>(0, DescFractal3D::BINDING_UBO_F3D);
        auto &f3dUBOHost = f3dUBO.getHostObject();


        const float altitudeRad = glm::radians(fractal3DSettings.altitude);
        const float rotationRad = glm::radians(fractal3DSettings.rotation);

        const float distance = 2.0f;

        const glm::vec3 cameraPos = {0,
                                     distance * cos(altitudeRad), distance * sin(altitudeRad)};

        const glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 1, 0));

        cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_MODEL, glm::mat4{1.0f});
        cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_VIEW, view);
        cameraUBOHost.set<glm::mat4>(DescCamera3D::TARGET_CAMERA_PROJ, glm::mat4{1.0f});


        f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_BASE_ITERATION, fractal3DSettings.baseIteration);
        f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_DEPTH_DIVISOR, fractal3DSettings.depthDivisor);
        f3dUBOHost.set<float>(DescFractal3D::TARGET_F3D_ROTATION, rotationRad);

        updateBufferMF([&cameraUBO](const uint32_t frameIndex) { cameraUBO.updateMF(frameIndex); });
        f3dUBO.update();
    }

    void GPC3DFractal::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }

    void GPC3DFractal::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
        appendDescriptor<DescCamera3D>(SET_CAMERA, descriptors);
        appendDescriptor<DescFractal3D>(SET_FRACTAL3D, descriptors);
    }

    void GPC3DFractal::configureVertexBuffer(vkh::HostDataObjectManager &som) {
        som.reserveArray<vkh::Vertex>(TARGET_VBO, 1);
    }

    void GPC3DFractal::configureIndexBuffer(vkh::HostDataObjectManager &som) {
        som.reserveArray<uint32_t>(TARGET_IBO, 1);
    }
} // namespace merutilm::rff2
