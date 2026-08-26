//
// Created by Merutilm on 2025-07-11.
//

#pragma once

#include <vulkan_helper/engine/descriptor/Descriptor.hpp>
#include <vulkan_helper/engine/pipeline/PipelineLayout.hpp>
#include <vulkan_helper/engine/pipeline/ShaderModule.hpp>

#include <vulkan_helper/engine/pipeline/PipelineSpecialization.hpp>

namespace merutilm::vkh {
    struct PipelineManager {
        PipelineLayout * layout = nullptr;
        PipelineSpecialization specialization;
        std::vector<Descriptor *> descriptors = {};
        std::vector<ShaderModule *> shaderModules = {};

        explicit PipelineManager(PipelineLayout & layout, PipelineSpecialization &&specialization);


        void attachShader(ShaderModule * shaderStage);

        void attachDescriptor(std::vector<Descriptor *> &&descriptor);
    };




}
