//
// Created by Merutilm on 2025-06-23.
//

#include "GLRendererStatic2Image.h"

#include <iostream>

#include "../settings/DataSettings.h"
#include "opencv2/core.hpp"
#include "opencv2/core/mat.hpp"

namespace merutilm::rff {
    GLRendererStatic2Image::GLRendererStatic2Image() : GLRenderer("static_2_image.frag") {
    }

    void GLRendererStatic2Image::setImageBuffer(const cv::Mat &normal, const cv::Mat &zoomed) {
        this->normalTextureID = GLShader::recreateTexture2D(normalTextureID, normal.cols, normal.rows,
                                                            Constants::TextureFormats::UCF3);
        this->normalTextureBuffer = normal.data;
        this->zoomedTextureID = GLShader::recreateTexture2D(zoomedTextureID, zoomed.cols, zoomed.rows,
                                                            Constants::TextureFormats::UCF3);
        this->zoomedTextureBuffer = zoomed.data;
    }

    void GLRendererStatic2Image::setCurrentFrame(const float currentFrame) {
        this->currentFrame = currentFrame;
    }


    void GLRendererStatic2Image::setDataSettings(const DataSettings &dataSettings) {
        this->dataSettings = &dataSettings;
    }

    void GLRendererStatic2Image::update() {
        const GLShader &shader = getShader();
        shader.uploadTexture2D("normal", GL_TEXTURE0, this->normalTextureID, normalTextureBuffer, getWidth(), getHeight(), Constants::TextureFormats::UCF3);
        shader.uploadTexture2D("zoomed", GL_TEXTURE1, this->zoomedTextureID, zoomedTextureBuffer, getWidth(), getHeight(), Constants::TextureFormats::UCF3);
        shader.upload2i("resolution", getWidth(), getHeight());
        shader.uploadFloat("defaultZoomIncrement", dataSettings->defaultZoomIncrement);
        shader.uploadFloat("currentFrame", currentFrame);
    }
}
