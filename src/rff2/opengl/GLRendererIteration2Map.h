//
// Created by Merutilm on 2025-06-13.
//

#pragma once
#include "GLIterationTextureProvider.h"
#include "GLRenderer.h"
#include "../data/Matrix.h"
#include "../attr/VidDataAttribute.h"
#include "../attr/VideoAttribute.h"

namespace merutilm::rff2 {
    class GLRendererIteration2Map : public GLRenderer{
        uint16_t iterWidth = 0;
        uint16_t iterHeight = 0;
        double maxIteration = 0.0;
        const VideoAttribute *videoSettings = nullptr;
        float currentFrame = 0;
        GLuint colIterationTextureID = 0;
        GLuint iterationTextureID = 0;
        std::vector<float> iterationBuffer;

    public:
        explicit GLRendererIteration2Map(std::string_view name);

        void reloadIterationBuffer(uint16_t iterWidth, uint16_t iterHeight);

        void setMaxIteration(double maxIteration);

        ~GLRendererIteration2Map() override = default;

        GLRendererIteration2Map(const GLRendererIteration2Map &) = delete;

        GLRendererIteration2Map(GLRendererIteration2Map &&) = delete;

        GLRendererIteration2Map &operator=(const GLRendererIteration2Map &) = delete;

        GLRendererIteration2Map &operator=(GLRendererIteration2Map &&) = delete;

        void setCurrentFrame(float currentFrame);

        void setVideoSettings(const VideoAttribute &dataSettings);

        void setAllIterations(const std::vector<double> &normal, const std::vector<double> &zoomed);

        void update() override;

    private:

        static std::array<float, 4> doubleToTwoFloatBits(double normal, double zoomed);

        static std::vector<float> emptyIterationBuffer(uint16_t w, uint16_t h);
    };
}
