//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include <array>

#include "glad.h"

using TextureFormat = std::array<GLuint, 3>;

namespace merutilm::rff2::Constants::TextureFormats {
    constexpr int INTERNAL = 0;
    constexpr int FORMAT = 1;
    constexpr int TYPE = 2;
    constexpr TextureFormat UCF1 = {GL_R32F, GL_RED, GL_UNSIGNED_BYTE};
    constexpr TextureFormat UCF2 = {GL_RG32F, GL_RG, GL_UNSIGNED_BYTE};
    constexpr TextureFormat UCF3 = {GL_RGB32F, GL_RGB, GL_UNSIGNED_BYTE};
    constexpr TextureFormat UCF4 = {GL_RGBA32F, GL_RGBA, GL_UNSIGNED_BYTE};
    constexpr TextureFormat II1 = {GL_R32I, GL_RED_INTEGER, GL_INT};
    constexpr TextureFormat II2 = {GL_RG32I, GL_RG_INTEGER, GL_INT};
    constexpr TextureFormat II3 = {GL_RGB32I, GL_RGB_INTEGER, GL_INT};
    constexpr TextureFormat II4 = {GL_RGBA32I, GL_RGBA_INTEGER, GL_INT};
    constexpr TextureFormat FF1 = {GL_R32F, GL_RED, GL_FLOAT};
    constexpr TextureFormat FF2 = {GL_RG32F, GL_RG, GL_FLOAT};
    constexpr TextureFormat FF3 = {GL_RGB32F, GL_RGB, GL_FLOAT};
    constexpr TextureFormat FF4 = {GL_RGBA32F, GL_RGBA, GL_FLOAT};
}
