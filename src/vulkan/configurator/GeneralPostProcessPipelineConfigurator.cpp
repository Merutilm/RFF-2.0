//
// Created by Merutilm on 2025-08-05.
//

#include "GeneralPostProcessPipelineConfigurator.hpp"

#include "../struct/Vertex.hpp"

namespace merutilm::mvk {

    void GeneralPostProcessPipelineConfigurator::updateUninitializedVertexIndex() {
        if (!initializedVertexIndex) {
            const uint32_t maxFramesInFlight = engine.getCore().getPhysicalDevice().getMaxFramesInFlight();
            for (int i = 0; i < maxFramesInFlight; ++i) {
                getVertexBuffer().update(i);
                getIndexBuffer().update(i);
            }
            getVertexBuffer().finalize(engine.getCommandPool());
            getIndexBuffer().finalize(engine.getCommandPool());
            initializedVertexIndex = true;
        }
    }


    void GeneralPostProcessPipelineConfigurator::configureVertexBuffer(ShaderObjectManager &som) {
        som.add(0, std::vector{
                    Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                    Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                    Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
                    Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                });
    }

    void GeneralPostProcessPipelineConfigurator::configureIndexBuffer(ShaderObjectManager &som) {
        som.add(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
    }
}
