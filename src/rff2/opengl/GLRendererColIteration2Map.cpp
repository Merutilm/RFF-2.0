//
// Created by Merutilm on 2025-07-01.
//

#include "GLRendererColIteration2Map.h"

#include "../attr/ShdPaletteAttribute.h"

namespace merutilm::rff2 {
    GLRendererColIteration2Map::GLRendererColIteration2Map() : GLRendererIteration2Map("col_iteration_palette_2_map.frag") {};


    void GLRendererColIteration2Map::setTime(const float time) {
        this->time = time;
    }

    void GLRendererColIteration2Map::setPaletteSettings(const ShdPaletteAttribute &paletteSettings) {
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

    void GLRendererColIteration2Map::update(){
        GLRendererIteration2Map::update();
        const GLShader &shader = getShader();
        shader.uploadTexture2D("palette", GL_TEXTURE1, paletteTextureID, paletteBuffer.data(), paletteWidth,
                           paletteHeight,
                           Constants::TextureFormats::FF4);
        shader.uploadInt("paletteWidth", paletteWidth);
        shader.uploadInt("paletteHeight", paletteHeight);
        shader.uploadInt("paletteLength", paletteLength);
        shader.uploadDouble("paletteOffset",
                            static_cast<double>(paletteSettings->offsetRatio) - static_cast<double>(time) *
                            paletteSettings->animationSpeed / paletteSettings->
                            iterationInterval);
        shader.uploadFloat("paletteInterval", paletteSettings->iterationInterval);
        shader.uploadInt("smoothing", static_cast<int>(paletteSettings->colorSmoothing));
    }


    std::vector<float> GLRendererColIteration2Map::createPaletteBuffer(const ShdPaletteAttribute &paletteSettings,
                                                                const uint16_t paletteWidth,
                                                                const uint16_t paletteHeight) {
        const std::vector<NormalizedColor> &colors = paletteSettings.colors;
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
