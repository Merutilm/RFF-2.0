//
// Created by Merutilm on 7/8/26.
//

#pragma once
#include "RenderPassAttachment.hpp"
#include "RenderPassAttachmentReference.hpp"
#include "Subpassdependency.hpp"
namespace merutilm::vkh {

    struct RenderPassAttachmentReferenceDetail {
        const RenderPassAttachment * targetAttachment;
        const RenderPassAttachmentReference srcReferenceInfo;
        const SubpassDependency dependency;
        const RenderPassAttachmentReference dstReferenceInfo;



    };
} // namespace merutilm::vkh
