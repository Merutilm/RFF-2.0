//
// Created by Merutilm on 7/7/26.
//

#pragma once

#include <vector>

#include "vulkan_helper/engine/configurator/GraphicsPipelineConfigurator.hpp"
#include "vulkan_helper/engine/wrapped/RenderPassAttachmentReferenceDetail.hpp"

namespace merutilm::vkh {

    class GraphicsPipelineNode {

    protected:
        std::vector<GraphicsPipelineNode *> depends = {};
        std::vector<GraphicsPipelineNode *> next = {};
        GraphicsPipelineConfigurator * configurator = nullptr;
        RenderPassAttachmentReferenceDetail attachmentReference;
        std::function<DescIndexPicker()> descIndexPickerFunc;
        uint32_t subpass;

    public:

        template<typename Func> requires std::is_invocable_r_v<DescIndexPicker, Func>
        explicit GraphicsPipelineNode(std::vector<GraphicsPipelineNode *> &&depends, GraphicsPipelineConfigurator *configurator, RenderPassAttachmentReferenceDetail &&attachmentReference, const uint32_t subpass, Func &&descIndexPickerFunc) :
            depends(std::move(depends)), configurator(configurator), attachmentReference(std::move(attachmentReference)), subpass(subpass), descIndexPickerFunc(std::forward<Func>(descIndexPickerFunc)) {
            for (const auto depend: this->depends) {
                depend->next.emplace_back(this);
            }
        }

        virtual ~GraphicsPipelineNode() = default;
        GraphicsPipelineNode(const GraphicsPipelineNode &) = delete;
        GraphicsPipelineNode &operator=(const GraphicsPipelineNode &) = delete;
        GraphicsPipelineNode(GraphicsPipelineNode &&) = delete;
        GraphicsPipelineNode &operator=(GraphicsPipelineNode &&) = delete;


        [[nodiscard]] const std::vector<GraphicsPipelineNode *> &getDepends() const { return this->depends; }

        [[nodiscard]] const std::vector<GraphicsPipelineNode *> &getNext() const { return this->next; }

        [[nodiscard]] const RenderPassAttachmentReferenceDetail &getAttachmentReference() const { return this->attachmentReference; }

        [[nodiscard]] GraphicsPipelineConfigurator &getPipelineConfigurator() const { return *this->configurator; }

        [[nodiscard]] DescIndexPicker genPicker() const { return this->descIndexPickerFunc(); }

        [[nodiscard]] uint32_t getSubpass() const { return this->subpass; }

        void synchronizeNext() const;
    };
} // namespace merutilm::vkh
