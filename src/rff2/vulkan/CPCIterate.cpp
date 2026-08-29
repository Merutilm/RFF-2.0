//
// Created by Merutilm on 8/24/26.
//

#include "CPCIterate.hpp"

#include "../calc/complex.hpp"
#include "../data/ComputeShaderBatchStagingData.hpp"
#include "../settings/Selectable.h"
#include "desc/SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {

    void CPCIterate::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {}

    void CPCIterate::pipelineInitialized() {
        // noop
    }

    void CPCIterate::renderContextRefreshed() {
        // noop
    }
    vkh::PipelineSpecialization CPCIterate::createSpecializationInfo() {
        vkh::PipelineSpecialization specialization(2);
        specialization.appendEntry(SPECIALIZATION_MPA_MODE, std::vector{0, 1});
        return specialization;
    }

    void CPCIterate::setMPAIgnore(const bool ignore) {
        specializationIndex = ignore ? 1 : 0;
    }

    void CPCIterate::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }
    void CPCIterate::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;

        appendDescriptor<DescRenderMetaIterationVariant>(SET_ITERATION, descriptors);
        appendDescriptor<DescRenderMeta>(SET_RENDER_META, descriptors);
        appendDescriptor<DescBatchResult>(SET_BATCH_RESULT, descriptors);
    }
} // namespace merutilm::rff2
