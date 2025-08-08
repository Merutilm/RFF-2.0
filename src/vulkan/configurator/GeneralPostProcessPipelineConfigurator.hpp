//
// Created by Merutilm on 2025-08-05.
//

#pragma once
#include "GeneralPipelineConfigurator.hpp"

namespace merutilm::mvk {
    class GeneralPostProcessPipelineConfigurator : public GeneralPipelineConfigurator {

        bool initializedVertexIndex = false;

    public:

        using GeneralPipelineConfigurator::GeneralPipelineConfigurator;

        ~GeneralPostProcessPipelineConfigurator() override = default;

        GeneralPostProcessPipelineConfigurator(const GeneralPostProcessPipelineConfigurator &) = delete;

        GeneralPostProcessPipelineConfigurator &operator=(const GeneralPostProcessPipelineConfigurator &) = delete;

        GeneralPostProcessPipelineConfigurator(GeneralPostProcessPipelineConfigurator &&) = delete;

        GeneralPostProcessPipelineConfigurator &operator=(GeneralPostProcessPipelineConfigurator &&) = delete;

    protected:
        void updateUninitializedVertexIndex();

        void configureVertexBuffer(ShaderObjectManager &som) override;

        void configureIndexBuffer(ShaderObjectManager &som) override;
    };
}
