//
// Created by Merutilm on 2025-06-13.
//

#pragma once
#include "GLIterationTextureProvider.h"
#include "GLRenderer.h"
#include "../data/Matrix.h"
#include "../settings/DataSettings.h"

namespace merutilm::rff {
    class GLRendererIteration2Map final : public GLRenderer, public GLIterationTextureProvider{
        int iterWidth = 0;
        int iterHeight = 0;
        double maxIteration = 0.0;
        const DataSettings *dataSettings = nullptr;
        float currentFrame = 0;
        int iterationTextureID = 0;
        std::vector<float> iterationBuffer;

    public:
        GLRendererIteration2Map();

        void reloadIterationBuffer(int iterWidth, int iterHeight, long maxIteration);

        ~GLRendererIteration2Map() override = default;

        GLRendererIteration2Map(const GLRendererIteration2Map &) = delete;

        GLRendererIteration2Map(GLRendererIteration2Map &&) = delete;

        GLRendererIteration2Map &operator=(const GLRendererIteration2Map &) = delete;

        GLRendererIteration2Map &operator=(GLRendererIteration2Map &&) = delete;

        void setCurrentFrame(float currentFrame);

        GLuint getIterationTextureID() override;

        void setDataSettings(const DataSettings &dataSettings);

        void setAllIterations(const std::vector<double> &normal, const std::vector<double> &zoomed);

        void update() override;

    private:

        static std::array<float, 4> doubleToTwoFloatBits(double normal, double zoomed);

        static std::vector<float> emptyIterationBuffer(uint16_t w, uint16_t h);
    };
}