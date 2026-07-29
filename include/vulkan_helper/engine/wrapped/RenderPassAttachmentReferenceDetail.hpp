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
        const std::optional<SubpassDependency> dependency;
        const std::optional<RenderPassAttachmentReference> dstReferenceInfo;

    };
} // namespace merutilm::vkh
