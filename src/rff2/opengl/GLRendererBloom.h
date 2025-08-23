//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include "GLRendererGaussianBlur.h"
#include "../attr/ShdBloomAttribute.h"

namespace merutilm::rff2 {
    class GLRendererBloom final : public GLRendererGaussianBlur {
        const ShdBloomAttribute *bloomSettings = nullptr;

    public:
        explicit GLRendererBloom();

        ~GLRendererBloom() override = default;

        GLRendererBloom(const GLRendererBloom &) = delete;

        GLRendererBloom &operator=(const GLRendererBloom &) = delete;

        GLRendererBloom(GLRendererBloom &&) = delete;

        GLRendererBloom &operator=(GLRendererBloom &&) = delete;

        void setBloomSettings(const ShdBloomAttribute &bloomSettings);

        void update() override;

    };
}