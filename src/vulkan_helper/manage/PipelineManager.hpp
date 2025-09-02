//
// Created by Merutilm on 2025-07-11.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "../impl/Descriptor.hpp"
#include "../impl/PipelineLayout.hpp"
#include "../impl/ShaderModule.hpp"

namespace merutilm::vkh {
    struct PipelineManagerImpl {
        PipelineLayoutRef layout;
        std::vector<DescriptorPtr> descriptors = {};
        std::vector<ShaderModulePtr> shaderModules = {};

        explicit PipelineManagerImpl(PipelineLayoutRef layout) : layout(layout) {
        }


        void attachShader(ShaderModulePtr shaderStage) {
            shaderModules.emplace_back(shaderStage);
        }

        void attachDescriptor(std::vector<DescriptorPtr> &&descriptor) { descriptors = std::move(descriptor); }

    };

    using PipelineManager = std::unique_ptr<PipelineManagerImpl>;
    using PipelineManagerPtr = PipelineManagerImpl *;
    using PipelineManagerRef = PipelineManagerImpl &;
}
