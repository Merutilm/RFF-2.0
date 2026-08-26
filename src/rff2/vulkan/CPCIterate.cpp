//
// Created by Merutilm on 8/24/26.
//

#include "CPCIterate.hpp"

#include "../calc/complex.hpp"
#include "../settings/Selectable.h"
#include "SharedDescriptorTemplate.hpp"
namespace merutilm::rff2 {

    void CPCIterate::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {

    }

    void CPCIterate::pipelineInitialized() {
        //noop
    }

    void CPCIterate::renderContextRefreshed() {
        //noop
    }
    vkh::PipelineSpecialization CPCIterate::createSpecializationInfo() {
        std::vector<RndMPAModeForComputeShader> values = Selectable::values<RndMPAModeForComputeShader>();
        vkh::PipelineSpecialization specialization(values.size());
        specialization.appendEntry(SPECIALIZATION_MPA_MODE, std::move(values));
        return specialization;
    }

    void CPCIterate::setRenderMeta(const std::vector<complex<float>> &reference, const complex<float> offset, const uint32_t maxIteration, const float logZoom,
                                          const float bailout, const float clarityMultiplier, RndMPAModeForComputeShader mpaMode, FrtDecimalizeIterationMethod decimalizeIterationMethod, const PA<float> * mpTableData, const uint64_t tableLen,
                                   const MPAIndexMapper *mapperData, const uint64_t mapperLen, FrtMPASelectionMethod selectionMethod) {

        vkh::Descriptor &desc = getDescriptor(SET_RENDER_META);
        auto &rmSSBO = desc.get<vkh::ShaderStorage>(0, BINDING_RM_SSBO);
        auto &rmTableSSBO = desc.get<vkh::ShaderStorage>(0, BINDING_RM_TABLE_SSBO);
        auto &rmMapperSSBO = desc.get<vkh::ShaderStorage>(0, BINDING_RM_MAPPER_SSBO);


        auto &rmSSBOHost = rmSSBO.getHostObject();
        auto &rmTableSSBOHost = rmTableSSBO.getHostObject();
        auto &rmMapperSSBOHost = rmMapperSSBO.getHostObject();

        rmSSBOHost.set<uint64_t>(TARGET_RM_MAX_ITERATION, maxIteration);
        rmSSBOHost.set<uint64_t>(TARGET_RM_MAX_REF_ITERATION, reference.size() - 1);
        rmSSBOHost.set<float>(TARGET_RM_LOG_ZOOM, logZoom);
        rmSSBOHost.set<float>(TARGET_RM_BAILOUT, bailout);
        rmSSBOHost.set<float>(TARGET_RM_CLARITY_MULTIPLIER, clarityMultiplier);
        rmSSBOHost.set<uint32_t>(TARGET_RM_DECIMALIZE_ITERATION_METHOD, static_cast<uint32_t>(decimalizeIterationMethod));
        rmSSBOHost.set<complex<float>>(TARGET_RM_OFFSET, static_cast<complex<float>>(offset));
        rmSSBOHost.resizeArray<complex<float>>(TARGET_RM_ORBIT, reference.size());
        rmSSBOHost.set<complex<float>>(TARGET_RM_ORBIT, reference);
        rmTableSSBOHost.set<uint64_t>(TARGET_RM_TABLE_LEN, tableLen);

        specializationIndex = static_cast<uint32_t>(mpaMode);

        rmTableSSBOHost.set<uint32_t>(TARGET_RM_TABLE_SELECTION_METHOD, static_cast<uint32_t>(selectionMethod));
        rmTableSSBOHost.resizeArray<PA<float>>(TARGET_RM_TABLE_DATA, tableLen);
        if (tableLen > 0) rmTableSSBOHost.set<PA<float>>(TARGET_RM_TABLE_DATA, mpTableData);

        rmMapperSSBOHost.set<uint64_t>(TARGET_RM_MAPPER_LEN, mapperLen);
        rmMapperSSBOHost.resizeArray<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, mapperLen);
        if (mapperLen > 0) rmMapperSSBOHost.set<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, mapperData);

        rmSSBO.reloadBuffer();
        rmTableSSBO.reloadBuffer();
        rmMapperSSBO.reloadBuffer();

        rmSSBO.update();
        rmTableSSBO.update();
        rmMapperSSBO.update();

        rmSSBO.lock(wc.getCommandPool());
        rmTableSSBO.lock(wc.getCommandPool());
        rmMapperSSBO.lock(wc.getCommandPool());

        writeDescriptorMF([&desc](vkh::DescriptorUpdateQueue & queue, const uint32_t frameIndex) {
            desc.queue(queue, frameIndex, {}, {BINDING_RM_SSBO, BINDING_RM_TABLE_SSBO, BINDING_RM_MAPPER_SSBO});
        });
    }

    void CPCIterate::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        //noop
    }
    void CPCIterate::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);

        static_assert(sizeof(PA<float>) == 32);
        static_assert(alignof(PA<float>) == 8);

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

        auto rmSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRm), vkh::BufferLock::LOCK_ONLY, false);
        auto rmTableSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmTable), vkh::BufferLock::LOCK_ONLY, false);
        auto rmMapperSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmMapper), vkh::BufferLock::LOCK_ONLY, false);


        vkh::DescriptorManager descManager;
        descManager.appendSSBO(BINDING_RM_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmSSBO));
        descManager.appendSSBO(BINDING_RM_TABLE_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmTableSSBO));
        descManager.appendSSBO(BINDING_RM_MAPPER_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmMapperSSBO));
        appendUniqueDescriptor(SET_RENDER_META, descriptors, std::move(descManager));
    }
} // namespace merutilm::rff2
