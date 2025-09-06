//
// Created by Merutilm on 2025-09-06.
//

#include "CPCIterationPalette2Map.hpp"

#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void CPCIterationPalette2Map::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //noop
    }


    void CPCIterationPalette2Map::pipelineInitialized() {
        const auto &iterAttr = getDescriptor(SET_I2MAP);
        writeDescriptorMF([&iterAttr](vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
            iterAttr.queue(queue, frameIndex, {}, {BINDING_I2MAP_UBO_ATTR});
        });
    }

    void CPCIterationPalette2Map::windowResized() {
        //noop
    }

    void CPCIterationPalette2Map::setInfo(const glm::uvec2 &extent, const double maxIteration, const float time) const {
        using namespace SharedDescriptorTemplate;
        auto &iter = getDescriptor(SET_I2MAP);
        auto &iterNormalSSBO = *iter.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_NORMAL);
        auto &iterZoomedSSBO = *iter.get<vkh::ShaderStorage>(0, BINDING_I2MAP_SSBO_ZOOMED);
        iterNormalSSBO.getHostObject().resizeAndClear<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, extent.x * extent.y);
        iterZoomedSSBO.getHostObject().resizeAndClear<double>(TARGET_I2MAP_SSBO_ZOOMED_ITERATION, extent.x * extent.y);
        iterNormalSSBO.reloadBuffer();
        iterZoomedSSBO.reloadBuffer();

        const auto &iterAttrUBO = *iter.get<vkh::Uniform>(0, BINDING_I2MAP_UBO_ATTR);
        iterAttrUBO.getHostObject().set<glm::uvec2>(TARGET_I2MAP_UBO_ATTR_EXTENT, extent);
        iterAttrUBO.getHostObject().set<double>(TARGET_I2MAP_UBO_ATTR_MAX_ITERATION, maxIteration);
        iterAttrUBO.getHostObject().set<float>(TARGET_I2MAP_UBO_ATTR_TIME, time);

        writeDescriptorMF([&iter](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            iter.queue(queue, frameIndex, {}, {BINDING_I2MAP_SSBO_NORMAL, BINDING_I2MAP_SSBO_ZOOMED});
        });
    }

    void CPCIterationPalette2Map::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void CPCIterationPalette2Map::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto normal = vkh::factory::create<vkh::HostDataObjectManager>();
        normal->reserveArray<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, 0);
        auto normalSSBO = vkh::factory::create<vkh::ShaderStorage>(engine.getCore(), std::move(normal),
                                                                   vkh::BufferLock::ALWAYS_MUTABLE, false);
        auto zoomed = vkh::factory::create<vkh::HostDataObjectManager>();
        zoomed->reserveArray<double>(TARGET_I2MAP_SSBO_NORMAL_ITERATION, 0);
        auto zoomedSSBO = vkh::factory::create<vkh::ShaderStorage>(engine.getCore(), std::move(zoomed),
                                                                   vkh::BufferLock::ALWAYS_MUTABLE, false);
        auto attr = vkh::factory::create<vkh::HostDataObjectManager>();
        attr->reserve<glm::uvec2>(TARGET_I2MAP_UBO_ATTR_EXTENT);
        attr->reserve<double>(TARGET_I2MAP_UBO_ATTR_MAX_ITERATION);
        attr->reserve<float>(TARGET_I2MAP_UBO_ATTR_TIME);
        auto attrUBO = vkh::factory::create<vkh::Uniform>(engine.getCore(), std::move(attr),
                                                          vkh::BufferLock::ALWAYS_MUTABLE, false);

        auto i2mapManager = vkh::factory::create<vkh::DescriptorManager>();
        i2mapManager->appendSSBO(BINDING_I2MAP_SSBO_NORMAL, VK_SHADER_STAGE_COMPUTE_BIT, std::move(normalSSBO));
        i2mapManager->appendSSBO(BINDING_I2MAP_SSBO_ZOOMED, VK_SHADER_STAGE_COMPUTE_BIT, std::move(zoomedSSBO));
        i2mapManager->appendUBO(BINDING_I2MAP_SSBO_ZOOMED, VK_SHADER_STAGE_COMPUTE_BIT, std::move(attrUBO));
        appendUniqueDescriptor(SET_I2MAP, descriptors, std::move(i2mapManager));
        appendDescriptor<DescVideo>(SET_VIDEO, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);

        auto output = vkh::factory::create<vkh::HostDataObjectManager>();
        output->reserveArray<double>(TARGET_OUTPUT_SSBO_RESULT_ITERATION, 0);
        auto outputSSBO = vkh::factory::create<vkh::ShaderStorage>(engine.getCore(), std::move(output),
                                                                   vkh::BufferLock::ALWAYS_MUTABLE, false);

        auto outputManager = vkh::factory::create<vkh::DescriptorManager>();
        outputManager->appendStorageImage(BINDING_OUTPUT_MERGED_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
        outputManager->appendSSBO(BINDING_OUTPUT_MERGED_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT, std::move(outputSSBO));

        appendUniqueDescriptor(SET_OUTPUT, descriptors, std::move(outputManager));
    }
}
