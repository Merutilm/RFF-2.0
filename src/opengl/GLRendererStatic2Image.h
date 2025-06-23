//
// Created by Merutilm on 2025-06-23.
//

#pragma once
#include "GLRenderer.h"
#include "../settings/DataSettings.h"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff {
    class GLRendererStatic2Image final : public GLRenderer {

        const DataSettings * dataSettings = nullptr;
        float currentFrame = 0;
        void * normalTextureBuffer = nullptr;
        GLuint normalTextureID = 0;
        void * zoomedTextureBuffer = nullptr;
        GLuint zoomedTextureID = 0;

    public:
        GLRendererStatic2Image();

        ~GLRendererStatic2Image() override = default;

        GLRendererStatic2Image(const GLRendererStatic2Image &) = delete;

        GLRendererStatic2Image &operator=(const GLRendererStatic2Image &) = delete;

        GLRendererStatic2Image(GLRendererStatic2Image &&) = delete;

        GLRendererStatic2Image &operator=(GLRendererStatic2Image &&) = delete;

        void setImageBuffer(const cv::Mat &normal, const cv::Mat &zoomed);

        void setCurrentFrame(float currentFrame);

        void setDataSettings(const DataSettings &dataSettings);

        void update() override;
    };
}
