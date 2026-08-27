//
// Created by Merutilm on 8/24/26.
//

#include "CPCIterate.hpp"

#include "../calc/complex.hpp"
#include "../settings/Selectable.h"
#include "SharedDescriptorTemplate.hpp"
#include "../data/ComputeShaderBatchStagingData.hpp"

namespace merutilm::rff2 {

    void CPCIterate::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {}

    void CPCIterate::pipelineInitialized() {
        // noop
    }

    void CPCIterate::renderContextRefreshed() {
        // noop
    }
    vkh::PipelineSpecialization CPCIterate::createSpecializationInfo() {
        std::vector<RndCmpMPAMode> values = Selectable::values<RndCmpMPAMode>();
        vkh::PipelineSpecialization specialization(values.size());
        specialization.appendEntry(SPECIALIZATION_MPA_MODE, std::move(values));
        return specialization;
    }

    const vkh::BufferContext &CPCIterate::getWriteBuffer() const {
        using namespace SharedDescriptorTemplate;
        auto &iterDesc = getDescriptor(SET_ITERATION);
        auto &iterSSBO = iterDesc.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
        return iterSSBO.getBufferContext();
    }

    void CPCIterate::resetWriteBuffer(const VkExtent2D extent, vkh::CommandPool &commandPool) {
        using namespace SharedDescriptorTemplate;
        setExtent(extent);

        vkh::Descriptor &iterDesc = getDescriptor(SET_ITERATION);

        auto &iterUBO = iterDesc.get<vkh::Uniform>(0, DescIteration::BINDING_UBO_ITERATION_INFO);
        auto &iterUBOHost = iterUBO.getHostObject();

        if (iterUBOHost.getElementCount(DescIteration::TARGET_SSBO_ITERATION_BUFFER) == extent.width * extent.height) return;


        vkh::Descriptor &rmDesc = getDescriptor(SET_RENDER_META);
        auto &rmBatchSSBO = rmDesc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_BATCH_SSBO);
        auto &rmBatchResultSSBO = rmDesc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_BATCH_RESULT_SSBO);

        auto &iterSSBO = iterDesc.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
        auto &iterSSBOHost = iterSSBO.getHostObject();
        auto &rmBatchSSBOHost = rmBatchSSBO.getHostObject();
        auto &rmBatchResultSSBOHost = rmBatchResultSSBO.getHostObject();


        iterUBOHost.set<glm::uvec2>(DescIteration::TARGET_UBO_ITERATION_EXTENT, {extent.width, extent.height});
        iterUBO.update(DescIteration::TARGET_UBO_ITERATION_EXTENT);


        iterSSBOHost.resizeAndClear<double>(DescIteration::TARGET_SSBO_ITERATION_BUFFER, extent.width * extent.height);
        rmBatchSSBOHost.resizeAndClear<ComputeShaderBatchStagingData>(DescRenderMeta::TARGET_RM_BATCH_STAGING_DATA, extent.width * extent.height);
        rmBatchResultSSBOHost.resizeAndClear<uint32_t>(DescRenderMeta::TARGET_RM_BATCH_RESULT_COMPLETED, extent.width * extent.height);

        iterSSBO.reloadBuffer();
        rmBatchSSBO.reloadBuffer();
        rmBatchResultSSBO.reloadBuffer();

        iterSSBO.update();
        rmBatchSSBO.update();
        rmBatchResultSSBO.update();

        iterSSBO.localize(commandPool);
        rmBatchSSBO.localize(commandPool);
        rmBatchResultSSBO.localize(commandPool);

        writeDescriptorMF([&iterDesc, &rmDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            iterDesc.queue(queue, frameIndex, {}, {DescIteration::BINDING_UBO_ITERATION_INFO, DescIteration::BINDING_SSBO_ITERATION_MATRIX});
            rmDesc.queue(queue, frameIndex, {}, {DescRenderMeta::BINDING_RM_BATCH_SSBO, DescRenderMeta::BINDING_RM_BATCH_RESULT_SSBO});
        });
    }

    void CPCIterate::setBatchSize(vkh::CommandPool &commandPool, const uint32_t batchSize) const {
        using namespace SharedDescriptorTemplate;
        vkh::Descriptor &desc = getDescriptor(SET_RENDER_META);
        auto &rmBatchInfoUBO = desc.get<vkh::Uniform>(0, DescRenderMeta::BINDING_RM_BATCH_INFO_UBO);
        auto &rmBatchInfoUBOHost = rmBatchInfoUBO.getHostObject();
        rmBatchInfoUBOHost.set<uint32_t>(DescRenderMeta::TARGET_RM_BATCH_SIZE, batchSize);
        rmBatchInfoUBO.reloadBuffer();
        rmBatchInfoUBO.update();
        rmBatchInfoUBO.localize(commandPool);
        writeDescriptorMF([&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            desc.queue(queue, frameIndex, {}, {DescRenderMeta::BINDING_RM_BATCH_INFO_UBO});
        });
    }

    void CPCIterate::setRenderMeta(const FractalSettings &frt, const RenderSettings &render,
                                   const std::vector<complex<float>> &reference, const complex<float> offset,
                                   const uint32_t maxIteration, const PA<float> *mpTableData, const uint64_t tableLen,
                                   const MPAIndexMapper *mapperData, const uint64_t mapperLen, vkh::CommandPool &commandPool) {

        using namespace SharedDescriptorTemplate;
        vkh::Descriptor &desc = getDescriptor(SET_RENDER_META);
        auto &rmSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_SSBO);
        auto &rmTableSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_TABLE_SSBO);
        auto &rmMapperSSBO = desc.get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_MAPPER_SSBO);

        auto &rmSSBOHost = rmSSBO.getHostObject();
        auto &rmTableSSBOHost = rmTableSSBO.getHostObject();
        auto &rmMapperSSBOHost = rmMapperSSBO.getHostObject();

        rmSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_MAX_ITERATION, maxIteration);
        rmSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_MAX_REF_ITERATION, reference.size() - 1);
        rmSSBOHost.set<float>(DescRenderMeta::TARGET_RM_LOG_ZOOM, frt.general.logZoom);
        rmSSBOHost.set<float>(DescRenderMeta::TARGET_RM_BAILOUT, frt.general.bailout);
        rmSSBOHost.set<float>(DescRenderMeta::TARGET_RM_CLARITY_MULTIPLIER, render.clarityMultiplier);
        rmSSBOHost.set<uint32_t>(DescRenderMeta::TARGET_RM_DECIMALIZE_ITERATION_METHOD,
                                 static_cast<uint32_t>(frt.perturb.decimalizeIterationMethod));
        rmSSBOHost.set<complex<float>>(DescRenderMeta::TARGET_RM_OFFSET, static_cast<complex<float>>(offset));
        rmSSBOHost.resizeArray<complex<float>>(DescRenderMeta::TARGET_RM_ORBIT, reference.size());
        rmSSBOHost.set<complex<float>>(DescRenderMeta::TARGET_RM_ORBIT, reference);
        rmTableSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_TABLE_LEN, tableLen);

        rmTableSSBOHost.set<uint32_t>(DescRenderMeta::TARGET_RM_TABLE_SELECTION_METHOD, static_cast<uint32_t>(frt.mpa.selectionMethod));
        rmTableSSBOHost.resizeArray<PA<float>>(DescRenderMeta::TARGET_RM_TABLE_DATA, tableLen);
        if (tableLen > 0)
            rmTableSSBOHost.set<PA<float>>(DescRenderMeta::TARGET_RM_TABLE_DATA, mpTableData);

        rmMapperSSBOHost.set<uint64_t>(DescRenderMeta::TARGET_RM_MAPPER_LEN, mapperLen);
        rmMapperSSBOHost.resizeArray<MPAIndexMapper>(DescRenderMeta::TARGET_RM_MAPPER_DATA, mapperLen);
        if (mapperLen > 0)
            rmMapperSSBOHost.set<MPAIndexMapper>(DescRenderMeta::TARGET_RM_MAPPER_DATA, mapperData);

        specializationIndex = static_cast<uint32_t>(render.computeShader.mpaMode);

        rmSSBO.reloadBuffer();
        rmTableSSBO.reloadBuffer();
        rmMapperSSBO.reloadBuffer();

        rmSSBO.update();
        rmTableSSBO.update();
        rmMapperSSBO.update();

        rmSSBO.localize(commandPool);
        rmTableSSBO.localize(commandPool);
        rmMapperSSBO.localize(commandPool);

        writeDescriptorMF([&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            desc.queue(queue, frameIndex, {}, {DescRenderMeta::BINDING_RM_SSBO, DescRenderMeta::BINDING_RM_TABLE_SSBO, DescRenderMeta::BINDING_RM_MAPPER_SSBO});
        });
    }


    const vkh::BufferContext &CPCIterate::getBatchResultBuffer() const {
        using namespace SharedDescriptorTemplate;
        return getDescriptor(SET_RENDER_META).get<vkh::ShaderStorage>(0, DescRenderMeta::BINDING_RM_BATCH_RESULT_SSBO).getBufferContext();
    }

    void CPCIterate::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }
    void CPCIterate::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;

        appendDescriptor<DescRenderMetaIterationVariant>(SET_ITERATION, descriptors);
        appendDescriptor<DescRenderMeta>(SET_RENDER_META, descriptors);
    }
} // namespace merutilm::rff2
