//
// Created by Merutilm on 2025-07-29.
//

#include "../vulkan/GPCIterationPalette.hpp"

#include "../app/Utilities.h"
#include "../settings/PerturbationMainIterator.hpp"
#include "../settings/ShdPaletteSettings.h"
#include "GPCSmoothZoom.hpp"
#include "desc/SharedDescriptorTemplate.hpp"
#include "vulkan_helper/util/BufferImageContextUtils.hpp"
#include "vulkan_helper/util/DescriptorUpdater.hpp"

namespace merutilm::rff2 {
    vkh::PipelineSpecialization GPCIterationPalette::createSpecializationInfo() {
        auto values = Selectable::values<PerturbationMainIterator>();
        vkh::PipelineSpecialization specialization(values.size());
        specialization.appendEntry(SPECIALIZATION_PERTURBATION_MAIN_ITERATOR, std::move(values));
        return specialization;
    }


    void GPCIterationPalette::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //noop
    }
    void GPCIterationPalette::setPerturbationMainIterator(PerturbationMainIterator mainIterator) {
        specializationIndex = static_cast<uint32_t>(mainIterator);
    }




    void GPCIterationPalette::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        auto &timeDesc = getDescriptor(SET_TIME);
        auto &iterDesc = getDescriptor(SET_ITERATION);
        writeDescriptorMF([&timeDesc, &iterDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            timeDesc.queue(queue, frameIndex, {}, {DescTime::BINDING_UBO_TIME});
            iterDesc.queue(queue, frameIndex, {}, {DescIteration::BINDING_UBO_ITERATION_INFO});
        });
    }

    void GPCIterationPalette::renderContextRefreshed() {
        // no operation
    }


    void GPCIterationPalette::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }

    void GPCIterationPalette::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescPalette>(SET_PALETTE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
        appendDescriptor<DescBatchResult>(SET_BATCH_RESULT, descriptors);
        appendDescriptor<DescSmoothZoom>(SET_SMOOTH_ZOOM, descriptors);
    }
} // namespace merutilm::rff2
