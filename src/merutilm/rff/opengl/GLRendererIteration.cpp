//
// Created by Merutilm on 2025-05-08.
//

#include "GLRendererIteration.h"

GLRendererIteration::GLRendererIteration() : GLRenderer("iteration_palette") {
}

void GLRendererIteration::setIteration(const int x, const int y, const double iteration) {
    const int index = (y * iterWidth + x) * 2;
    auto [a, b] = doubleToTwoFloatBits(iteration);
    iterationBuffer[index] = a;
    iterationBuffer[index + 1] = b;
}

void GLRendererIteration::setTime(const float time) {
    this->time = time;
}

void GLRendererIteration::setMaxIteration(const int maxIteration) {
    this->maxIteration = maxIteration;
}

void GLRendererIteration::reloadIterationBuffer(const int iterWidth, const int iterHeight,
                                                const uint64_t maxIteration) {
    this->iterationBuffer = emptyIterationBuffer(iterWidth, iterHeight);
    this->iterationTextureID = GLShaderLoader::recreateTexture2D(iterationTextureID, iterWidth, iterHeight,
                                                                   RFFConstants::TextureFormats::FLOAT2, false);
    this->maxIteration = static_cast<double>(maxIteration);
    this->iterWidth = iterWidth;
    this->iterHeight = iterHeight;
}

void GLRendererIteration::setPaletteSettings(const PaletteSettings *paletteSettings) {
    this->paletteSettings = paletteSettings;
    this->paletteLength = static_cast<int>(paletteSettings->colors.size());
    GLint max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    this->paletteWidth = std::min(max, paletteLength);
    this->paletteHeight = (paletteLength - 1) / paletteWidth + 1;
    this->paletteBuffer = createPaletteBuffer(*paletteSettings, paletteWidth, paletteHeight);
    this->paletteTextureID = GLShaderLoader::recreateTexture2D(paletteTextureID, paletteWidth, paletteHeight, RFFConstants::TextureFormats::FLOAT4, true);
}

int GLRendererIteration::getIterationTextureID() const {
    return iterationTextureID;
}

float GLRendererIteration::getResolutionMultiplier() const {
    return static_cast<float>(iterWidth) / static_cast<float>(getWidth());
}

void GLRendererIteration::update() {
    if (paletteBuffer.empty()) {
        return;
    }
    const GLShaderLoader &shader = getShaderLoader();
    if (const GLuint previousFBOTextureID = getPrevFBOTextureID(); previousFBOTextureID == 0) {
        if (iterationBuffer.empty()) {
            return;
        }
        shader.uploadTexture2D("iterations", GL_TEXTURE0, iterationTextureID, iterationBuffer.data(), iterWidth,
                               iterHeight, RFFConstants::TextureFormats::FLOAT2);
        shader.uploadFloat("resolutionMultiplier", getResolutionMultiplier());
    } else {
        shader.uploadTexture2D("iterations", GL_TEXTURE0, previousFBOTextureID);
        shader.uploadFloat("resolutionMultiplier", 1);
    }

    shader.uploadDouble("maxIteration", maxIteration);

    shader.uploadTexture2D("palette", GL_TEXTURE1, paletteTextureID, paletteBuffer.data(), paletteWidth, paletteHeight,
                           RFFConstants::TextureFormats::FLOAT4);
    shader.uploadInt("paletteWidth", paletteWidth);
    shader.uploadInt("paletteHeight", paletteHeight);
    shader.uploadInt("paletteLength", paletteLength);
    shader.uploadFloat("paletteOffset",
                       paletteSettings->offsetRatio - time * paletteSettings->animationSpeed / paletteSettings->
                       iterationInterval);
    shader.uploadFloat("paletteInterval", paletteSettings->iterationInterval);
    shader.uploadInt("smoothing", static_cast<int>(paletteSettings->colorSmoothing));
}


std::array<float, 2> GLRendererIteration::doubleToTwoFloatBits(const double v) {
    const uint64_t a = std::bit_cast<uint64_t>(v);
    const auto high = static_cast<uint32_t>(a >> 32);
    const auto low = static_cast<uint32_t>(a & 0xFFFFFFFF);
    return std::array{std::bit_cast<float>(high), std::bit_cast<float>(low)};
}

std::vector<float> GLRendererIteration::emptyIterationBuffer(const int iterWidth, const int iterHeight) {
    return std::vector<float>(iterWidth * iterHeight * 2);
}

void GLRendererIteration::setAllIterations(Matrix<double> &iterations) {
    iterationBuffer = emptyIterationBuffer(iterWidth, iterHeight);
    for (int i = 0; i < iterWidth * iterHeight; i++) {
        auto [high, low] = doubleToTwoFloatBits(iterations[i]);
        iterationBuffer[i * 2] = high;
        iterationBuffer[i * 2 + 1] = low;
    }
}

std::vector<float> GLRendererIteration::createPaletteBuffer(const PaletteSettings &paletteSettings, const int paletteWidth, const int paletteHeight) {
    const std::vector<Color> &colors = paletteSettings.colors;
    auto result = std::vector<float>();
    result.reserve(paletteWidth * paletteHeight * 4);
    for (const auto &[r, g, b, a] : colors) {
        result.push_back(r);
        result.push_back(g);
        result.push_back(b);
        result.push_back(a);
    }
    return result;
}


void GLRendererIteration::destroy() {
    glDeleteTextures(1, &iterationTextureID);
    glDeleteTextures(1, &paletteTextureID);
}

