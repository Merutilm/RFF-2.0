//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "GLRenderer.h"
namespace merutilm::rff {
    class GLRendererDisplayer final : public GLRenderer {
    public:
        GLRendererDisplayer();

        ~GLRendererDisplayer() override = default;

        GLRendererDisplayer(const GLRendererDisplayer &) = delete;

        GLRendererDisplayer &operator=(const GLRendererDisplayer &) = delete;

        GLRendererDisplayer(const GLRendererDisplayer &&) = delete;

        GLRendererDisplayer &operator=(const GLRendererDisplayer &&) = delete;

        void update() override;

    };
}