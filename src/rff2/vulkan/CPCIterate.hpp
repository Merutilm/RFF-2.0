//
// Created by Merutilm on 8/24/26.
//

#pragma once
#include "../calc/complex.hpp"
#include "../mrthy/MPAIndexMapper.hpp"
#include "../mrthy/PA.h"
#include "../settings/FractalSettings.h"
#include "../settings/RenderSettings.h"
#include "vulkan_helper/engine/configurator/ComputePipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct CPCIterate final : public vkh::ComputePipelineConfigurator {

        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_RENDER_META = 1;


        static constexpr uint32_t SPECIALIZATION_MPA_MODE = 0;


        explicit CPCIterate(vkh::Engine &engine, vkh::WindowContext &wc) :
            ComputePipelineConfigurator(engine, wc, "vk_iterate.comp") {

        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;
        void pipelineInitialized() override;
        void renderContextRefreshed() override;

        vkh::PipelineSpecialization createSpecializationInfo() override;

        [[nodiscard]] const vkh::BufferContext &getWriteBuffer() const;

        void resetWriteBuffer(VkExtent2D extent, vkh::CommandPool &commandPool);

        void setRenderMeta(const FractalSettings &frt, const RenderSettings &render,
                           const std::vector<complex<float>> &reference, complex<float> offset, uint32_t maxIteration,
                           const PA<float> *mpTableData, uint64_t tableLen, const MPAIndexMapper *mapperData,
                           uint64_t mapperLen, vkh::CommandPool &commandPool);



        [[nodiscard]] const vkh::BufferContext &getBatchResultBuffer() const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;
        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;

    };

}
