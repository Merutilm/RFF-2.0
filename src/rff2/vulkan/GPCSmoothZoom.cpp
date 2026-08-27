//
// Created by Merutilm on 7/28/26.
//

#include "GPCSmoothZoom.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/repo/GlobalSamplerRepo.hpp"

namespace merutilm::rff2 {


    void GPCSmoothZoom::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //noop
    }

    void GPCSmoothZoom::pipelineInitialized() {
        writeDescriptorMF([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_SMOOTH_ZOOM).queue(queue, frameIndex, {}, {BINDING_SMOOTH_ZOOM_UBO});
            getDescriptor(SET_SAMPLE).queue(queue, frameIndex, {}, {BINDING_SAMPLE_RESOLUTION_UBO});

        });
    }


    void GPCSmoothZoom::setRescaledResolution(const glm::vec2 &newResolution) const {
        auto &resDesc = getDescriptor(SET_SAMPLE);
        auto &resUBO = resDesc.get<vkh::Uniform>(0, BINDING_SAMPLE_RESOLUTION_UBO);
        auto &resUBOHost = resUBO.getHostObject();
        resUBOHost.set<glm::uvec2>(TARGET_SAMPLE_EXTENT, newResolution);
        resUBO.update();
    }

    void GPCSmoothZoom::renderContextRefreshed() {auto &sic = wc.getSharedImageContext();
        auto &resampleDesc = getDescriptor(SET_SAMPLE);
        resampleDesc.get<vkh::CombinedImageSampler>(0, BINDING_SAMPLE_SAMPLER)
                .setImageContextMF(
                        sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY));


        writeDescriptorMF([&resampleDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            resampleDesc.queue(queue, frameIndex, {}, {BINDING_SAMPLE_SAMPLER});
        });}



    void GPCSmoothZoom::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }

    void GPCSmoothZoom::resetSmoothZoom() const {
        setSmoothZoomData(glm::vec2(0.0f, 0.0f), 0.0f);
    }

    void GPCSmoothZoom::setSmoothZoomData(const glm::vec2 &positionDelta, const float logZoomDelta) const {

        vkh::Descriptor &smoothZoomDesc = getDescriptor(SET_SMOOTH_ZOOM);
        auto &smoothZoomUBO = smoothZoomDesc.get<vkh::Uniform>(0, BINDING_SMOOTH_ZOOM_UBO);
        vkh::HostDataObject &smoothZoomUBOHost = smoothZoomUBO.getHostObject();
        smoothZoomUBOHost.set(TARGET_SMOOTH_ZOOM_POSITION_DELTA, positionDelta);
        smoothZoomUBOHost.set(TARGET_SMOOTH_ZOOM_LOG_ZOOM_DELTA, logZoomDelta);
        smoothZoomUBO.update();

    }

    void GPCSmoothZoom::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        vkh::Sampler &sampler = pickFromGlobalRepository<vkh::GlobalSamplerRepo, vkh::Sampler &>(
                VkSamplerCreateInfo{.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                    .pNext = nullptr,
                                    .flags = 0,
                                    .magFilter = VK_FILTER_LINEAR,
                                    .minFilter = VK_FILTER_LINEAR,
                                    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                                    .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                                    .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                                    .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                                    .mipLodBias = 0,
                                    .anisotropyEnable = VK_FALSE,
                                    .maxAnisotropy = 0,
                                    .compareEnable = VK_FALSE,
                                    .compareOp = VK_COMPARE_OP_ALWAYS,
                                    .minLod = 0,
                                    .maxLod = 0,
                                    .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                                    .unnormalizedCoordinates = VK_FALSE});
        auto descManager = vkh::DescriptorManager();
        auto combinedSampler = std::make_unique<vkh::CombinedImageSampler>(wc.core, sampler, true);
        descManager.appendCombinedImgSampler(BINDING_SAMPLE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                              std::move(combinedSampler));
        auto uboManager = vkh::HostDataObjectManager();
        uboManager.reserve<glm::uvec2>(TARGET_SAMPLE_EXTENT);
        descManager.appendUBO(
                BINDING_SAMPLE_RESOLUTION_UBO, VK_SHADER_STAGE_FRAGMENT_BIT,
                std::make_unique<vkh::Uniform>(wc.core, std::move(uboManager), vkh::BufferLocalization::BIDIRECTIONAL, false));

        appendUniqueDescriptor(SET_SAMPLE, descriptors, std::move(descManager));


        vkh::DescriptorManager descManager2;
        vkh::HostDataObjectManager hdm;
        hdm.reserve<glm::vec2>(TARGET_SMOOTH_ZOOM_POSITION_DELTA);
        hdm.reserve<float>(TARGET_SMOOTH_ZOOM_LOG_ZOOM_DELTA);
        auto uniform = std::make_unique<vkh::Uniform>(wc.core, std::move(hdm), vkh::BufferLocalization::BIDIRECTIONAL, false);
        descManager2.appendUBO(BINDING_SMOOTH_ZOOM_UBO, VK_SHADER_STAGE_FRAGMENT_BIT, std::move(uniform));

        appendUniqueDescriptor(SET_SMOOTH_ZOOM, descriptors, std::move(descManager2));
    }
} // namespace merutilm::rff2
