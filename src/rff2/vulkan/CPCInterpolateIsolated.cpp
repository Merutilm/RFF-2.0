//
// Created by Merutilm on 8/28/26.
//

#include "CPCInterpolateIsolated.hpp"

#include "desc/SharedDescriptorTemplate.hpp"
namespace merutilm::rff2 {

    void CPCInterpolateIsolated::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {}
    void CPCInterpolateIsolated::pipelineInitialized() {}
    void CPCInterpolateIsolated::renderContextRefreshed() {}
    void CPCInterpolateIsolated::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {}
    void CPCInterpolateIsolated::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        appendDescriptor<DescRenderMetaIterationVariant>(SET_ITERATION, descriptors);
        appendDescriptor<DescBatchResult>(SET_BATCH_RESULT, descriptors);
    }
} // namespace merutilm::rff2
