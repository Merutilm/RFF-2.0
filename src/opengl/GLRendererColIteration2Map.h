//
// Created by Merutilm on 2025-07-01.
//

#pragma once
#include "GLRendererIteration2Map.h"
#include "GLTimeRenderer.h"
#include "../settings/PaletteSettings.h"

namespace merutilm::rff2 {
    class GLRendererColIteration2Map final : public GLRendererIteration2Map, public GLTimeRenderer{
        const PaletteSettings *paletteSettings = nullptr;
        GLuint paletteTextureID = 0;
        uint16_t paletteWidth = 0;
        uint16_t paletteHeight = 0;
        uint32_t paletteLength = 0;
        std::vector<float> paletteBuffer;

        float time = 0;


    public:
        GLRendererColIteration2Map();

        void setTime(float time) override;

        void setPaletteSettings(const PaletteSettings &paletteSettings);

        void update() override;

        static std::vector<float> createPaletteBuffer(const PaletteSettings &paletteSettings, uint16_t paletteWidth,
                                                      uint16_t paletteHeight);
    };
}
