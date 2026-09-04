//
// Created by Merutilm on 8/24/26.
//

#pragma once
#include "../mrthy/MPAIndexMapper.hpp"
#include "../mrthy/PA.h"
#include "../settings/FractalSettings.h"
#include "../settings/RenderSettings.h"
#include "desc/SharedDescriptorTemplate.hpp"
#include "vulkan_helper/engine/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2 {

    template<Number Num>
    struct CPCIterate final : public vkh::ComputePipelineConfigurator {

        static constexpr uint32_t SET_ITERATION = 0;

        static constexpr uint32_t SET_RENDER_META = 1;
        static constexpr uint32_t BINDING_RM_SSBO = 0;
        static constexpr uint32_t TARGET_RM_MAX_ITERATION = 0;
        static constexpr uint32_t TARGET_RM_MAX_REF_ITERATION = 1;
        static constexpr uint32_t TARGET_RM_LOG_ZOOM = 2;
        static constexpr uint32_t TARGET_RM_BAILOUT = 3;
        static constexpr uint32_t TARGET_RM_CLARITY_MULTIPLIER = 4;
        static constexpr uint32_t TARGET_RM_DECIMALIZE_ITERATION_METHOD = 5;
        static constexpr uint32_t TARGET_RM_OFFSET = 6;
        static constexpr uint32_t TARGET_RM_ORBIT = 7;

        static constexpr uint32_t BINDING_RM_TABLE_SSBO = 1;
        static constexpr uint32_t TARGET_RM_TABLE_LEN = 0;
        static constexpr uint32_t TARGET_RM_TABLE_SELECTION_METHOD = 1;
        static constexpr uint32_t TARGET_RM_TABLE_DATA = 2;

        static constexpr uint32_t BINDING_RM_MAPPER_SSBO = 2;
        static constexpr uint32_t TARGET_RM_MAPPER_LEN = 0;
        static constexpr uint32_t TARGET_RM_MAPPER_DATA = 1;

        static constexpr uint32_t BINDING_RM_BATCH_INFO_UBO = 3;
        static constexpr uint32_t TARGET_RM_BATCH_SIZE = 0;

        static constexpr uint32_t BINDING_RM_BATCH_SSBO = 4;
        static constexpr uint32_t TARGET_RM_BATCH_STAGING_DATA = 0;


        static constexpr uint32_t SET_BATCH_RESULT = 2;


        static constexpr uint32_t SPECIALIZATION_MPA_MODE = 0;


        explicit CPCIterate(vkh::Engine &engine, vkh::WindowContext &wc) :
            ComputePipelineConfigurator(engine, wc, "vk_iterate.comp") {}

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;
        void pipelineInitialized() override;
        void renderContextRefreshed() override;

        vkh::PipelineSpecialization createSpecializationInfo() override;

        void setMeta(const FractalSettings &frt, const RenderSettings &render,
                     const std::vector<complex<Num>> &reference, complex<Num> offset, uint64_t maxIteration,
                     const PA<Num> *mpTableData, uint64_t tableLen, const MPAIndexMapper *mapperData,
                     uint64_t mapperLen, vkh::CommandPool &commandPool) const;

        void resizeWriteBuffer(uint32_t width, uint32_t height) const;

        void clearWriteBuffer(vkh::CommandPool &commandPool) const;

        void setBatchSize(uint32_t batchSize) const;

        void setMPAIgnore(bool ignore);

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;
        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };

    template<Number Num>
    void CPCIterate<Num>::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {}

    template<Number Num>
    void CPCIterate<Num>::pipelineInitialized() {
        auto &desc = getDescriptor(SET_RENDER_META);
        writeDescriptorMF([&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            desc.queue(queue, frameIndex, {}, {BINDING_RM_BATCH_INFO_UBO});
        });

    }

    template<Number Num>
    void CPCIterate<Num>::renderContextRefreshed() {
        // noop
    }
    template<Number Num>
    vkh::PipelineSpecialization CPCIterate<Num>::createSpecializationInfo() {
        vkh::PipelineSpecialization specialization(2);
        specialization.appendEntry(SPECIALIZATION_MPA_MODE, std::vector{0, 1});
        return specialization;
    }
    template<Number Num>
    void CPCIterate<Num>::setMeta(const FractalSettings &frt, const RenderSettings &render,
                             const std::vector<complex<Num>> &reference, const complex<Num> offset,
                             const uint64_t maxIteration, const PA<Num> *mpTableData, const uint64_t tableLen,
                             const MPAIndexMapper *mapperData, const uint64_t mapperLen,
                             vkh::CommandPool &commandPool) const {

        using namespace SharedDescriptorTemplate;
        auto &desc = getDescriptor(SET_RENDER_META);
        auto &rmSSBO = desc.template get<vkh::ShaderStorage>(0, BINDING_RM_SSBO);
        auto &rmTableSSBO = desc.template get<vkh::ShaderStorage>(0, BINDING_RM_TABLE_SSBO);
        auto &rmMapperSSBO = desc.template get<vkh::ShaderStorage>(0, BINDING_RM_MAPPER_SSBO);

        auto &rmSSBOHost = rmSSBO.getHostObject();
        auto &rmTableSSBOHost = rmTableSSBO.getHostObject();
        auto &rmMapperSSBOHost = rmMapperSSBO.getHostObject();

        rmSSBOHost.template set<uint64_t>(TARGET_RM_MAX_ITERATION, maxIteration);
        rmSSBOHost.template set<uint64_t>(TARGET_RM_MAX_REF_ITERATION, reference.size() - 1);
        rmSSBOHost.template set<float>(TARGET_RM_LOG_ZOOM, frt.general.logZoom);
        rmSSBOHost.template set<float>(TARGET_RM_BAILOUT, frt.general.bailout);
        rmSSBOHost.template set<float>(TARGET_RM_CLARITY_MULTIPLIER, render.display.clarityMultiplier);
        rmSSBOHost.template set<uint32_t>(TARGET_RM_DECIMALIZE_ITERATION_METHOD,
                                 static_cast<uint32_t>(frt.perturb.decimalizeIterationMethod));
        rmSSBOHost.template set<complex<Num>>(TARGET_RM_OFFSET, static_cast<complex<Num>>(offset));
        rmSSBOHost.template resizeArray<complex<Num>>(TARGET_RM_ORBIT, reference.size());
        rmSSBOHost.template set<complex<Num>>(TARGET_RM_ORBIT, reference);
        rmTableSSBOHost.template set<uint64_t>(TARGET_RM_TABLE_LEN, tableLen);

        rmTableSSBOHost.template set<uint32_t>(TARGET_RM_TABLE_SELECTION_METHOD, static_cast<uint32_t>(frt.mpa.selectionMethod));
        rmTableSSBOHost.template resizeArray<PA<Num>>(TARGET_RM_TABLE_DATA, tableLen);
        if (tableLen > 0)
            rmTableSSBOHost.template set<PA<Num>>(TARGET_RM_TABLE_DATA, mpTableData);

        rmMapperSSBOHost.template set<uint64_t>(TARGET_RM_MAPPER_LEN, mapperLen);
        rmMapperSSBOHost.template resizeArray<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, mapperLen);
        if (mapperLen > 0)
            rmMapperSSBOHost.template set<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, mapperData);

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
            desc.queue(queue, frameIndex, {}, {BINDING_RM_SSBO, BINDING_RM_TABLE_SSBO, BINDING_RM_MAPPER_SSBO});
        });
    }
    template<Number Num>
    void CPCIterate<Num>::resizeWriteBuffer(const uint32_t width, const uint32_t height) const {
        using namespace SharedDescriptorTemplate;

        auto &desc = getDescriptor(SET_RENDER_META);
        auto &rmBatchSSBO = desc.template get<vkh::ShaderStorage>(0, BINDING_RM_BATCH_SSBO);
        auto &rmBatchSSBOHost = rmBatchSSBO.getHostObject();

        rmBatchSSBOHost.template resizeAndClear<ComputeShaderBatchStagingData<float>>(TARGET_RM_BATCH_STAGING_DATA,
                                                                             width * height);

        rmBatchSSBO.reloadBuffer();
        rmBatchSSBO.update();
        rmBatchSSBO.localize(wc.getCommandPool());

        writeDescriptorMF([&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            desc.queue(queue, frameIndex, {}, {BINDING_RM_BATCH_SSBO});
        });
    }


    template<Number Num>
    void CPCIterate<Num>::clearWriteBuffer(vkh::CommandPool &commandPool) const {
        using namespace SharedDescriptorTemplate;
        auto &desc = getDescriptor(SET_RENDER_META);
        auto &rmBatchSSBO = desc.template get<vkh::ShaderStorage>(0, BINDING_RM_BATCH_SSBO);
        auto &rmBatchSSBOHost = rmBatchSSBO.getHostObject();

        rmBatchSSBOHost.reset(TARGET_RM_BATCH_STAGING_DATA);
        rmBatchSSBO.reloadBuffer();
        rmBatchSSBO.update();
        rmBatchSSBO.localize(commandPool);
        writeDescriptorMF([&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            desc.queue(queue, frameIndex, {}, {BINDING_RM_BATCH_SSBO});
        });
    }
    template<Number Num>
    void CPCIterate<Num>::setBatchSize(const uint32_t batchSize) const {
        using namespace SharedDescriptorTemplate;
        auto &desc = getDescriptor(SET_RENDER_META);
        auto &rmBatchInfoUBO = desc.template get<vkh::Uniform>(0, BINDING_RM_BATCH_INFO_UBO);
        auto &rmBatchInfoUBOHost = rmBatchInfoUBO.getHostObject();
        rmBatchInfoUBOHost.template set<uint32_t>(TARGET_RM_BATCH_SIZE, batchSize);
        rmBatchInfoUBO.update();
    }
    template<Number Num>
    void CPCIterate<Num>::setMPAIgnore(const bool ignore) { specializationIndex = ignore ? 1 : 0; }

    template<Number Num>
    void CPCIterate<Num>::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }
    template<Number Num>
    void CPCIterate<Num>::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;

        appendDescriptor<DescRenderMetaIterationVariant>(SET_ITERATION, descriptors);

        vkh::HostDataObjectManager homRm;
        homRm.template reserve<uint64_t>(TARGET_RM_MAX_ITERATION);
        homRm.template reserve<uint64_t>(TARGET_RM_MAX_REF_ITERATION);
        homRm.template reserve<float>(TARGET_RM_LOG_ZOOM);
        homRm.template reserve<float>(TARGET_RM_BAILOUT);
        homRm.template reserve<float>(TARGET_RM_CLARITY_MULTIPLIER);
        homRm.template reserve<uint32_t>(TARGET_RM_DECIMALIZE_ITERATION_METHOD);
        homRm.template reserve<complex<Num>>(TARGET_RM_OFFSET);
        homRm.template reserveArray<complex<Num>>(TARGET_RM_ORBIT, 0);


        vkh::HostDataObjectManager homRmTable;
        homRmTable.template reserve<uint64_t>(TARGET_RM_TABLE_LEN);
        homRmTable.template reserve<uint32_t>(TARGET_RM_TABLE_SELECTION_METHOD);
        homRmTable.template reserveArray<PA<Num>>(TARGET_RM_TABLE_DATA, 0);

        vkh::HostDataObjectManager homRmMapper;
        homRmMapper.template reserve<uint64_t>(TARGET_RM_MAPPER_LEN);
        homRmMapper.template reserveArray<MPAIndexMapper>(TARGET_RM_MAPPER_DATA, 0);

        vkh::HostDataObjectManager homRmBatchInfo;
        homRmBatchInfo.template reserve<uint32_t>(TARGET_RM_BATCH_SIZE);

        vkh::HostDataObjectManager homRmBatch;
        homRmBatch.template reserveArray<ComputeShaderBatchStagingData<Num>>(TARGET_RM_BATCH_STAGING_DATA, 1);


        auto rmSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRm),
                                                           vkh::BufferLocalization::UNIDIRECTIONAL, false);
        auto rmTableSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmTable),
                                                                vkh::BufferLocalization::UNIDIRECTIONAL, false);
        auto rmMapperSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmMapper),
                                                                 vkh::BufferLocalization::UNIDIRECTIONAL, false);

        auto rmBatchInfoUBO = std::make_unique<vkh::Uniform>(wc.core, std::move(homRmBatchInfo),
                                                             vkh::BufferLocalization::UNIDIRECTIONAL, false);
        auto rmBatchSSBO = std::make_unique<vkh::ShaderStorage>(wc.core, std::move(homRmBatch),
                                                                vkh::BufferLocalization::UNIDIRECTIONAL, false);


        vkh::DescriptorManager descManagerRenderMeta;
        descManagerRenderMeta.appendSSBO(BINDING_RM_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmSSBO));
        descManagerRenderMeta.appendSSBO(BINDING_RM_TABLE_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmTableSSBO));
        descManagerRenderMeta.appendSSBO(BINDING_RM_MAPPER_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmMapperSSBO));
        descManagerRenderMeta.appendUBO(BINDING_RM_BATCH_INFO_UBO, VK_SHADER_STAGE_COMPUTE_BIT,
                                        std::move(rmBatchInfoUBO));
        descManagerRenderMeta.appendSSBO(BINDING_RM_BATCH_SSBO, VK_SHADER_STAGE_COMPUTE_BIT, std::move(rmBatchSSBO));

        appendUniqueDescriptor(SET_RENDER_META, descriptors, std::move(descManagerRenderMeta));
        appendDescriptor<DescBatchResult>(SET_BATCH_RESULT, descriptors);
    }

} // namespace merutilm::rff2
