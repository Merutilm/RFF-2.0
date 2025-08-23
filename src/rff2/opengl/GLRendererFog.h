//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLRendererGaussianBlur.h"
#include "../attr/ShdFogAttribute.h"

namespace merutilm::rff2 {
    class GLRendererFog final : public GLRendererGaussianBlur {
        const ShdFogAttribute *fogSettings = nullptr;

    public:
        GLRendererFog();

        ~GLRendererFog() override = default;

        GLRendererFog(const GLRendererFog &) = delete;

        GLRendererFog &operator=(const GLRendererFog &) = delete;

        GLRendererFog(GLRendererFog &&) = delete;

        GLRendererFog &operator=(GLRendererFog &&) = delete;


        void setFogSettings(const ShdFogAttribute &fogSettings);

        void update() override;
    };
}