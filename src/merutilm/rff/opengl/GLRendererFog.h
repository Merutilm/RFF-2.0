//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLRendererGaussianBlur.h"
#include "../settings/FogSettings.h"

namespace merutilm::rff {
    class GLRendererFog final : public GLRendererGaussianBlur {
        const FogSettings *fogSettings = nullptr;

    public:
        GLRendererFog();

        ~GLRendererFog() override = default;

        GLRendererFog(const GLRendererFog &) = delete;

        GLRendererFog &operator=(const GLRendererFog &) = delete;

        GLRendererFog(GLRendererFog &&) = delete;

        GLRendererFog &operator=(GLRendererFog &&) = delete;


        void setFogSettings(const FogSettings &fogSettings);

        void update() override;
    };
}