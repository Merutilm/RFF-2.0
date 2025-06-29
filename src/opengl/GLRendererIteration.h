//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "GLIterationTextureProvider.h"
#include "GLRenderer.h"
#include "GLTimeRenderer.h"
#include "../data/Matrix.h"
#include "../settings/PaletteSettings.h"

namespace merutilm::rff2 {
    class GLRendererIteration final : public GLRenderer, public GLTimeRenderer, public GLIterationTextureProvider {
        const PaletteSettings *paletteSettings = nullptr;
        uint16_t iterWidth = 0;
        uint16_t iterHeight = 0;
        double maxIteration = 0.0;

        GLuint paletteTextureID = 0;
        uint16_t paletteWidth = 0;
        uint16_t paletteHeight = 0;
        uint32_t paletteLength = 0;
        std::vector<float> paletteBuffer;
        float time = 0;

        GLuint iterationTextureID = 0;
        std::vector<float> iterationBuffer;

    public:
        explicit GLRendererIteration();

        ~GLRendererIteration() override;

        GLRendererIteration(const GLRendererIteration &) = delete;

        GLRendererIteration &operator=(const GLRendererIteration &) = delete;

        GLRendererIteration(GLRendererIteration &&) = delete;

        GLRendererIteration &operator=(GLRendererIteration &&) = delete;

        void setIteration(uint16_t x, uint16_t y, double iteration);

        void setTime(float time) override;

        void fillZero();

        GLuint getIterationTextureID() override;

        void reloadIterationBuffer(uint16_t iterWidth, uint16_t iterHeight);

        void setPaletteSettings(const PaletteSettings &paletteSettings);

        int getIterationTextureID() const;

        void setMaxIteration(double maxIteration);

        void update() override;

        void setAllIterations(const Matrix<double> &iterations);

    private:
        static std::array<float, 2> doubleToTwoFloatBits(double v);

        static std::vector<float> emptyIterationBuffer(uint16_t iterWidth, uint16_t iterHeight);


        static std::vector<float> createPaletteBuffer(const PaletteSettings &paletteSettings, uint16_t paletteWidth,
                                                      uint16_t paletteHeight);
    };
}