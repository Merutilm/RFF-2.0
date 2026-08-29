//
// Created by Merutilm on 2025-09-06.
//

#include "CPC2MapIterationStripe.hpp"

#include "../settings/ShdPaletteSettings.h"
#include "SharedImageContextIndices.hpp"
#include "desc/SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void CPC2MapIterationStripe::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        // noop
    }


    void CPC2MapIterationStripe::pipelineInitialized() {
        //noop
    }

    void CPC2MapIterationStripe::renderContextRefreshed() {
        using namespace SharedImageContextIndices;
        auto &outDesc = getDescriptor(SET_OUTPUT_IMAGE);
        auto &[outImg] = outDesc.get<vkh::StorageImage>(0, BINDING_OUTPUT_MERGED_IMAGE);
        outImg = wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY);
        writeDescriptorMF([&outDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            outDesc.queue(queue, frameIndex, {}, {BINDING_OUTPUT_MERGED_IMAGE});
        });
    }


    void CPC2MapIterationStripe::setAllIterations(const std::vector<double> &normal,
                                                  const std::vector<double> &zoomed) const {
        using namespace SharedDescriptorTemplate;
        auto &map2Desc = getDescriptor(SET_I2MAP);
        auto &map2DescNormalSSBO = map2Desc.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_NORMAL);
        map2DescNormalSSBO.getHostObject().set<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, normal);
        auto &map2DescZoomedSSBO = map2Desc.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_ZOOMED);
        map2DescZoomedSSBO.getHostObject().set<double>(TARGET_I2MAP_SSBO_ZOOMED_ITERATION, zoomed);

        map2DescNormalSSBO.update();
        map2DescZoomedSSBO.update();
    }

    void CPC2MapIterationStripe::set2MapSize(const VkExtent2D &extent) {
        using namespace SharedDescriptorTemplate;
        const auto &[width, height] = extent;
        setExtent(extent);
        auto &iter = getDescriptor(SET_I2MAP);
        auto &iterNormalSSBO = iter.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_NORMAL);
        iterNormalSSBO.getHostObject().resizeAndClear<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, width * height);
        iterNormalSSBO.reloadBuffer();

        auto &iterZoomedSSBO = iter.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_ZOOMED);
        iterZoomedSSBO.getHostObject().resizeAndClear<double>(TARGET_I2MAP_SSBO_ZOOMED_ITERATION, width * height);
        iterZoomedSSBO.reloadBuffer();

        auto &iterOut = getDescriptor(SET_OUTPUT_ITERATION);
        auto &iterOutSSBO = iterOut.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
        iterOutSSBO.getHostObject().resizeAndClear<double>(DescIteration::TARGET_SSBO_ITERATION_BUFFER, width * height);
        iterOutSSBO.reloadBuffer();
        iterOutSSBO.localize(wc.getCommandPool());

        auto &iterOutUBO = iterOut.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);
        iterOutUBO.getHostObject().set<glm::uvec2>(DescIteration::TARGET_UBO_ITERATION_EXTENT, {width, height});
        iterOutUBO.update(DescIteration::TARGET_UBO_ITERATION_EXTENT);


        writeDescriptorMF([&iter, &iterOut](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            iter.queue(queue, frameIndex, {}, {BINDING_I2MAP_SSBO_NORMAL, BINDING_I2MAP_SSBO_ZOOMED});
            iterOut.queue(queue, frameIndex, {},
                          {DescIteration::BINDING_UBO_ITERATION_INFO, DescIteration::BINDING_SSBO_ITERATION_MATRIX});
        });
    }


    void CPC2MapIterationStripe::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }

    void CPC2MapIterationStripe::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto normal = vkh::HostDataObjectManager();
        normal.reserveArray<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, 1);
        auto normalSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(normal),
                                                               vkh::BufferLocalization::ALWAYS_EXPOSED, false);
        auto zoomed = vkh::HostDataObjectManager();
        zoomed.reserveArray<double>(TARGET_I2MAP_SSBO_ZOOMED_ITERATION, 1);
        auto zoomedSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(zoomed),
                                                               vkh::BufferLocalization::ALWAYS_EXPOSED, false);

        auto i2mapManager = vkh::DescriptorManager();
        i2mapManager.appendSSBO(BINDING_I2MAP_SSBO_NORMAL, VK_SHADER_STAGE_COMPUTE_BIT, std::move(normalSSBO));
        i2mapManager.appendSSBO(BINDING_I2MAP_SSBO_ZOOMED, VK_SHADER_STAGE_COMPUTE_BIT, std::move(zoomedSSBO));
        appendUniqueDescriptor(SET_I2MAP, descriptors, std::move(i2mapManager));
        appendDescriptor<DescVideo>(SET_VIDEO, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);

        auto outputManager = vkh::DescriptorManager();
        outputManager.appendStorageImage(BINDING_OUTPUT_MERGED_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
        appendUniqueDescriptor(SET_OUTPUT_IMAGE, descriptors, std::move(outputManager));
        appendDescriptor<DescIteration>(SET_OUTPUT_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
    }
} // namespace merutilm::rff2
