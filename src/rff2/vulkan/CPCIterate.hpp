//
// Created by Merutilm on 8/24/26.
//

#pragma once
#include "../calc/complex.hpp"
#include "../mrthy/MPAIndexMapper.hpp"
#include "../mrthy/PA.h"
#include "../settings/FrtDecimalizeIterationMethod.h"
#include "../settings/FrtMPASelectionMethod.h"
#include "../settings/RndMPAModeForComputeShader.hpp"
#include "vulkan_helper/engine/configurator/ComputePipelineConfigurator.hpp"


namespace merutilm::rff2 {
    struct CPCIterate final : public vkh::ComputePipelineConfigurator {

        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_RENDER_META = 1;


        static constexpr uint32_t SPECIALIZATION_MPA_MODE = 0;

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

        CPCIterate(vkh::Engine &engine, vkh::WindowContext &wc) :
            ComputePipelineConfigurator(engine, wc, "vk_iterate.comp") {

        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;
        void pipelineInitialized() override;
        void renderContextRefreshed() override;

        vkh::PipelineSpecialization createSpecializationInfo() override;

        void setRenderMeta(const std::vector<complex<float>> &reference, complex<float> offset, uint32_t maxIteration,
                           float logZoom, float bailout, float clarityMultiplier, RndMPAModeForComputeShader mpaMode, FrtDecimalizeIterationMethod decimalizeIterationMethod,
                           const PA<float> *mpTableData, uint64_t tableLen, const MPAIndexMapper *mapperData,
                           uint64_t mapperLen, FrtMPASelectionMethod selectionMethod);

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;
        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;

    };

}
