//
// Created by Merutilm on 2025-06-13.
//

#include "GLRendererIteration2Map.h"

#include <cstring>


merutilm::rff::GLRendererIteration2Map::GLRendererIteration2Map() : GLRenderer("iteration_palette_2_map.frag") {
}

void merutilm::rff::GLRendererIteration2Map::reloadIterationBuffer(const int iterWidth, const int iterHeight, const long maxIteration) {
    this->iterationBuffer = emptyIterationBuffer(iterWidth, iterHeight);
    this->iterationTextureID = getShaderLoader().recreateTexture2D(iterationTextureID, iterWidth, iterHeight,
                                                                   Constants::TextureFormats::FLOAT4);
    this->iterWidth = iterWidth;
    this->iterHeight = iterHeight;
    this->maxIteration = maxIteration;
}

void merutilm::rff::GLRendererIteration2Map::setCurrentFrame(const float currentFrame) {
    this->currentFrame = currentFrame;
}

GLuint merutilm::rff::GLRendererIteration2Map::getIterationTextureID() {
    return getFBOTextureID();
}


void merutilm::rff::GLRendererIteration2Map::setDataSettings(const DataSettings &dataSettings) {
    this->dataSettings = &dataSettings;
}


void merutilm::rff::GLRendererIteration2Map::update() {
    const GLShaderLoader &shader = getShaderLoader();
    shader.upload2i("resolution", getWidth(), getHeight());
    shader.uploadDouble("maxIteration", maxIteration);
    shader.uploadTexture2D("normalAndZoomed", GL_TEXTURE0, iterationTextureID, iterationBuffer.data(), iterWidth,
                           iterHeight,
                           Constants::TextureFormats::FLOAT4);
    shader.uploadFloat("defaultZoomIncrement", dataSettings->defaultZoomIncrement);
    shader.uploadFloat("currentFrame", currentFrame);
}

std::array<float, 4> merutilm::rff::GLRendererIteration2Map::doubleToTwoFloatBits(const double normal, const double zoomed) {
    const uint64_t na = std::bit_cast<uint64_t>(normal);
    const auto nHigh = static_cast<uint32_t>(na >> 32);
    const auto nLow = static_cast<uint32_t>(na & 0xffffffffL);
    const uint64_t za = std::bit_cast<uint64_t>(zoomed);
    const auto zHigh = static_cast<uint32_t>(za >> 32);
    const auto zLow = static_cast<uint32_t>(za & 0xffffffffL);

    return std::array{
        std::bit_cast<float>(nHigh), std::bit_cast<float>(nLow), std::bit_cast<float>(zHigh), std::bit_cast<float>(zLow)
    };
}

std::vector<float> merutilm::rff::GLRendererIteration2Map::emptyIterationBuffer(const uint16_t w, const uint16_t h) {
    return std::vector<float>(static_cast<uint32_t>(w) * h * 4);
}


void merutilm::rff::GLRendererIteration2Map::setAllIterations(const std::vector<double> &normal, const std::vector<double> &zoomed) {
    for (int i = 0; i < iterWidth * iterHeight * 4; i += 4) {
        auto v = doubleToTwoFloatBits(normal[i / 4], zoomed[i / 4]);
        std::memcpy(iterationBuffer.data() + i, v.data(), 4 * sizeof(float));
    }
}
