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
        auto &rmBatchSSBO = rmDesc.get<vkh::ShaderStorage>(0, BINDING_RM_BATCH_SSBO);
        auto &rmBatchResultSSBO = rmDesc.get<vkh::ShaderStorage>(0, BINDING_RM_BATCH_RESULT_SSBO);

        auto &iterSSBO = iterDesc.get<vkh::ShaderStorage>(0, DescIteration::BINDING_SSBO_ITERATION_MATRIX);
        auto &iterSSBOHost = iterSSBO.getHostObject();
        auto &rmBatchSSBOHost = rmBatchSSBO.getHostObject();
        auto &rmBatchResultSSBOHost = rmBatchResultSSBO.getHostObject();


        iterUBOHost.set<glm::uvec2>(DescIteration::TARGET_UBO_ITERATION_EXTENT, {extent.width, extent.height});
        iterUBO.update(DescIteration::TARGET_UBO_ITERATION_EXTENT);


        iterSSBOHost.resizeAndClear<double>(DescIteration::TARGET_SSBO_ITERATION_BUFFER, extent.width * extent.height);
        rmBatchSSBOHost.resizeAndClear<ComputeShaderBatchStagingData>(TARGET_RM_BATCH_STAGING_DATA, extent.width * extent.height);
        rmBatchResultSSBOHost.resizeAndClear<uint32_t>(TARGET_RM_BATCH_RESULT_COMPLETED, extent.width * extent.height);

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
            rmDesc.queue(queue, frameIndex, {}, {BINDING_RM_BATCH_SSBO, BINDING_RM_BATCH_RESULT_SSBO});
        });
    }

    void CPCIterate::setRenderMeta(const FractalSettings &frt, const RenderSettings &render,
                                   const std::vector<complex<float>> &reference, const complex<float> offset,
                                   const uint32_t maxIteration, const PA<float> *mpTableData, const uint64_t tableLen,
                                   const MPAIndexMapper *mapperData, const uint64_t mapperLen, vkh::CommandPool &commandPool) {

        vkh::Descriptor &desc = getDescriptor(SET_RENDER_META);
        auto &rmSSBO = desc.get<vkh::ShaderStorage>(0, BINDING_RM_SSBO);
        auto &rmTableSSBO = desc.get<vkh::ShaderStorage>(0, BINDING_RM_TABLE_SSBO);
        auto &rmMapperSSBO = desc.get<vkh::ShaderStorage>(0, BINDING_RM_MAPPER_SSBO);
        auto &rmBatchInfoUBO = desc.get<vkh::Uniform>(0, BINDING_RM_BATCH_INFO_UBO);


        auto &rmSSBOHost = rmSSBO.getHostObject();
        auto &rmTableSSBOHost = rmTableSSBO.getHostObject();
        auto &rmMapperSSBOHost = rmMapperSSBO.getHostObject();
        auto &rmBatchInfoUBOHost = rmBatchInfoUBO.getHostObject();

        rmSSBOHost.set<uint64_t>(TARGET_RM_MAX_ITERATION, maxIteration);
        rmSSBOHost.set<uint64_t>(TARGET_RM_MAX_REF_ITERATION, reference.size() - 1);
        rmSSBOHost.set<float>(TARGET_RM_LOG_ZOOM, frt.general.logZoom);
        rmSSBOHost.set<float>(TARGET_RM_BAILOUT, frt.general.bailout);
        rmSSBOHost.set<float>(TARGET_RM_CLARITY_MULTIPLIER, render.clarityMultiplier);
        rmSSBOHost.set<uint32_t>(TARGET_RM_DECIMALIZE_ITERATION_METHOD,
                                 static_cast<uint32_t>(frt.perturb.decimalizeIterationMethod));
        rmSSBOHost.set<complex<float>>(TARGET_RM_OFFSET, static_cast<complex<float>>(offset));
        rmSSBOHost.resizeArray<complex<float>>(TARGET_RM_ORBIT, reference.size());
        rmSSBOHost.set<complex<float>>(TARGET_RM_ORBIT, reference);
        rmTableSSBOHost.set<uint64_t>(TARGET_RM_TABLE_LEN, tableLen);

        rmTableSSBOHost.set<uint32_t>(TARGET_RM_TABLE_SELECTION_METHOD, static_cast<uint32_t>(frt.mpa.selectionMethod));
        rmTableSSBOHost.resizeArray<PA<float>>(TARGET_RM_TABLE_DATA, tableLen);
        if (tableLen > 0)
            rmTableSSBOHost.set<PA<float>>(TARGET_RM_TABLE_DATA, mpTableData);

        rmMapperSSBOHost.set<uint64_t>(TARGET_RM_MAPPER_LEN, mapperLen);
        rmMapperSSBOHost.resizeArray<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, mapperLen);
        if (mapperLen > 0)
            rmMapperSSBOHost.set<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, mapperData);

        rmBatchInfoUBOHost.set<uint32_t>(TARGET_RM_BATCH_SIZE, render.computeShader.absIterationBatchSize);

        specializationIndex = static_cast<uint32_t>(render.computeShader.mpaMode);

        rmSSBO.reloadBuffer();
        rmTableSSBO.reloadBuffer();
        rmMapperSSBO.reloadBuffer();
        rmBatchInfoUBO.reloadBuffer();

        rmSSBO.update();
        rmTableSSBO.update();
        rmMapperSSBO.update();
        rmBatchInfoUBO.update();

        rmSSBO.localize(commandPool);
        rmTableSSBO.localize(commandPool);
        rmMapperSSBO.localize(commandPool);
        rmBatchInfoUBO.localize(commandPool);

        writeDescriptorMF([&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            desc.queue(queue, frameIndex, {}, {BINDING_RM_SSBO, BINDING_RM_TABLE_SSBO, BINDING_RM_MAPPER_SSBO, BINDING_RM_BATCH_INFO_UBO});
        });
    }


    const vkh::BufferContext &CPCIterate::getBatchResultBuffer() const {
        return getDescriptor(SET_RENDER_META).get<vkh::ShaderStorage>(0, BINDING_RM_BATCH_RESULT_SSBO).getBufferContext();
    }

    void CPCIterate::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }
    void CPCIterate::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;

        const auto [id, descriptorGenerator] = vkh::DescriptorTemplate::from<DescIteration>();
        auto descManagerIter = descriptorGenerator(wc.core);

        appendUniqueDescriptor(SET_ITERATION, descriptors, std::move(descManagerIter));

        static_assert(sizeof(PA<float>) == 32);
        static_assert(alignof(PA<float>) == 8);
        static_assert(sizeof(ComputeShaderBatchStagingData) == 32);
        static_assert(alignof(ComputeShaderBatchStagingData) == 8);

        vkh::HostDataObjectManager homRm;
        homRm.reserve<uint64_t>(TARGET_RM_MAX_ITERATION);
        homRm.reserve<uint64_t>(TARGET_RM_MAX_REF_ITERATION);
        homRm.reserve<float>(TARGET_RM_LOG_ZOOM);
        homRm.reserve<float>(TARGET_RM_BAILOUT);
        homRm.reserve<float>(TARGET_RM_CLARITY_MULTIPLIER);
        homRm.reserve<uint32_t>(TARGET_RM_DECIMALIZE_ITERATION_METHOD);
        homRm.reserve<complex<float>>(TARGET_RM_OFFSET);
        homRm.reserveArray<complex<float>>(TARGET_RM_ORBIT, 0);


        vkh::HostDataObjectManager homRmTable;
        homRmTable.reserve<uint64_t>(TARGET_RM_TABLE_LEN);
        homRmTable.reserve<uint32_t>(TARGET_RM_TABLE_SELECTION_METHOD, 4);
        homRmTable.reserveArray<PA<float>>(TARGET_RM_TABLE_DATA, 0);

        vkh::HostDataObjectManager homRmMapper;
        homRmMapper.reserve<uint64_t>(TARGET_RM_MAPPER_LEN);
        homRmMapper.reserveArray<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, 0);

        vkh::HostDataObjectManager homRmBatchInfo;
        homRmBatchInfo.reserve<uint32_t>(TARGET_RM_BATCH_SIZE);

        vkh::HostDataObjectManager homRmBatch;
        homRmBatch.reserveArray<ComputeShaderBatchStagingData>(TARGET_RM_BATCH_STAGING_DATA, 1);

        vkh::HostDataObjectManager homRmBatchResult;
        homRmBatchResult.reserveArray<uint32_t>(TARGET_RM_BATCH_RESULT_COMPLETED, 1);

        auto rmSSBO =
                std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRm), vkh::BufferLocalization::UNIDIRECTIONAL, false);
        auto rmTableSSBO =
                std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmTable), vkh::BufferLocalization::UNIDIRECTIONAL, false);
        auto rmMapperSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmMapper),
                                                                 vkh::BufferLocalization::UNIDIRECTIONAL, false);

        auto rmBatchInfoUBO = std::make_unique<vkh::Uniform>(wc.core, std::move(homRmBatchInfo), vkh::BufferLocalization::UNIDIRECTIONAL, false);
        auto rmBatchSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmBatch), vkh::BufferLocalization::UNIDIRECTIONAL, false);
        auto rmBatchResultSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmBatchResult), vkh::BufferLocalization::BIDIRECTIONAL, false);


        vkh::DescriptorManager descManagerRenderMeta;
        descManagerRenderMeta.appendSSBO(BINDING_RM_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmSSBO));
        descManagerRenderMeta.appendSSBO(BINDING_RM_TABLE_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmTableSSBO));
        descManagerRenderMeta.appendSSBO(BINDING_RM_MAPPER_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmMapperSSBO));
        descManagerRenderMeta.appendUBO(BINDING_RM_BATCH_INFO_UBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmBatchInfoUBO));
        descManagerRenderMeta.appendSSBO(BINDING_RM_BATCH_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmBatchSSBO));
        descManagerRenderMeta.appendSSBO(BINDING_RM_BATCH_RESULT_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmBatchResultSSBO));
        appendUniqueDescriptor(SET_RENDER_META, descriptors, std::move(descManagerRenderMeta));
    }
} // namespace merutilm::rff2
