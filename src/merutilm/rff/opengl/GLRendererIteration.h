//
// Created by Merutilm on 2025-05-08.
//

#pragma once
#include "GLRenderer.h"
#include "GLTimeProvider.h"
#include "GLTimeRenderer.h"
#include "../data/Matrix.h"
#include "../settings/PaletteSettings.h"

class GLRendererIteration final : public GLRenderer, public GLTimeRenderer {
    const PaletteSettings *paletteSettings = nullptr;
    int iterWidth = 0;
    int iterHeight = 0;
    double maxIteration = 0.0;

    GLuint paletteTextureID = 0;
    int paletteWidth = 0;
    int paletteHeight = 0;
    int paletteLength = 0;
    std::vector<float> paletteBuffer;
    float time = 0;

    GLuint iterationTextureID = 0;
    std::vector<float> iterationBuffer;

public:
    GLRendererIteration();

    void setIteration(int x, int y, double iteration);

    void setTime(float time) override;

    void setMaxIteration(int maxIteration);

    void reloadIterationBuffer(int iterWidth, int iterHeight, uint64_t maxIteration);

    void setPaletteSettings(const PaletteSettings *paletteSettings);

    int getIterationTextureID() const;

    float getResolutionMultiplier() const;

    void update() override;

    void setAllIterations(Matrix<double> &iterations);

    void destroy() override;


private:
    static std::array<float, 2> doubleToTwoFloatBits(double v);
    static std::vector<float> emptyIterationBuffer(int iterWidth, int iterHeight);
    static std::vector<float> createPaletteBuffer(const PaletteSettings &paletteSettings, int paletteWidth, int paletteHeight);



};
