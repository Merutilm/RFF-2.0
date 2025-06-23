//
// Created by Merutilm on 2025-05-08.
//

#include "GLRendererIteration.h"

namespace merutilm::rff {
    GLRendererIteration::GLRendererIteration() : GLRenderer("iteration_palette.frag") {
    }

    GLRendererIteration::~GLRendererIteration() {
        if (glIsTexture(iterationTextureID)) {
            glDeleteTextures(1, &iterationTextureID);
        }
        if (glIsTexture(paletteTextureID)) {
            glDeleteTextures(1, &paletteTextureID);
        }
    }

    void GLRendererIteration::setIteration(const uint16_t x, const uint16_t y, const double iteration) {
        const uint32_t index = (static_cast<uint32_t>(y) * iterWidth + x) * 2;
        auto [a, b] = doubleToTwoFloatBits(iteration);
        iterationBuffer[index] = a;
        iterationBuffer[index + 1] = b;
    }

    void GLRendererIteration::setTime(const float time) {
        this->time = time;
    }

    GLuint GLRendererIteration::getIterationTextureID() {
        return iterationTextureID;
    }

    void GLRendererIteration::reloadIterationBuffer(const uint16_t iterWidth, const uint16_t iterHeight) {
        this->iterationBuffer = emptyIterationBuffer(iterWidth, iterHeight);
        this->iterationTextureID = GLShader::recreateTexture2D(iterationTextureID, iterWidth, iterHeight,
                                                                     Constants::TextureFormats::FF2);
        this->iterWidth = iterWidth;
        this->iterHeight = iterHeight;
    }

    void GLRendererIteration::setPaletteSettings(const PaletteSettings &paletteSettings) {
        this->paletteSettings = &paletteSettings;
        this->paletteLength = static_cast<uint32_t>(paletteSettings.colors.size());
        GLint max;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
        this->paletteWidth = static_cast<uint16_t>(std::min(static_cast<uint32_t>(max), paletteLength));
        this->paletteHeight = static_cast<uint16_t>((paletteLength - 1) / paletteWidth + 1);
        this->paletteBuffer = createPaletteBuffer(paletteSettings, paletteWidth, paletteHeight);
        this->paletteTextureID = GLShader::recreateTexture2D(paletteTextureID, paletteWidth, paletteHeight,
                                                                   Constants::TextureFormats::FF4);
    }

    int GLRendererIteration::getIterationTextureID() const {
        return iterationTextureID;
    }

    void GLRendererIteration::update() {
        if (paletteBuffer.empty()) {
            return;
        }
        const GLShader &shader = getShader();
        if (const GLuint previousFBOTextureID = getPrevFBOTextureID(); previousFBOTextureID == 0) {
            if (iterationBuffer.empty()) {
                return;
            }
            shader.uploadTexture2D("iterations", GL_TEXTURE0, iterationTextureID, iterationBuffer.data(), iterWidth,
                                   iterHeight, Constants::TextureFormats::FF2);
        } else {
            shader.uploadTexture2D("iterations", GL_TEXTURE0, previousFBOTextureID);
        }

        shader.uploadDouble("maxIteration", maxIteration);

        shader.uploadTexture2D("palette", GL_TEXTURE1, paletteTextureID, paletteBuffer.data(), paletteWidth, paletteHeight,
                               Constants::TextureFormats::FF4);
        shader.uploadInt("paletteWidth", paletteWidth);
        shader.uploadInt("paletteHeight", paletteHeight);
        shader.uploadInt("paletteLength", paletteLength);
        shader.uploadDouble("paletteOffset",
                           static_cast<double>(paletteSettings->offsetRatio) - static_cast<double>(time) * paletteSettings->animationSpeed / paletteSettings->
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

    std::vector<float> GLRendererIteration::emptyIterationBuffer(const uint16_t iterWidth, const uint16_t iterHeight) {
        return std::vector<float>(static_cast<uint32_t>(iterWidth) * iterHeight * 2);
    }

    void GLRendererIteration::fillZero() {
        std::ranges::fill(iterationBuffer, 0);
    }


    void GLRendererIteration::setMaxIteration(const double maxIteration) {
        this->maxIteration = maxIteration;
    }

    void GLRendererIteration::setAllIterations(const Matrix<double> &iterations) {
        iterationBuffer = emptyIterationBuffer(iterWidth, iterHeight);
        for (uint32_t i = 0; i < static_cast<uint32_t>(iterWidth) * iterHeight; i++) {
            auto [high, low] = doubleToTwoFloatBits(iterations[i]);
            iterationBuffer[i * 2] = high;
            iterationBuffer[i * 2 + 1] = low;
        }
    }

    std::vector<float> GLRendererIteration::createPaletteBuffer(const PaletteSettings &paletteSettings,
                                                                const uint16_t paletteWidth, const uint16_t paletteHeight) {
        const std::vector<ColorFloat> &colors = paletteSettings.colors;
        auto result = std::vector<float>();
        result.reserve(static_cast<uint32_t>(paletteWidth) * paletteHeight * 4);
        for (const auto &[r, g, b, a]: colors) {
            result.push_back(r);
            result.push_back(g);
            result.push_back(b);
            result.push_back(a);
        }
        return result;
    }
}