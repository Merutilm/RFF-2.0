//
// Created by Merutilm on 2025-06-17.
//

#pragma once
#include <filesystem>
#include "GLShader.h"

namespace merutilm::rff2 {
    class GLShaderConfig {
        std::string filename;
        GLuint shaderID;

    public:
        explicit GLShaderConfig(const std::filesystem::path &filename);

        ~GLShaderConfig();

        GLShaderConfig(const GLShaderConfig &other) = default;

        GLShaderConfig &operator=(const GLShaderConfig &other) = default;

        GLShaderConfig(GLShaderConfig &&other) = default;

        GLShaderConfig &operator=(GLShaderConfig &&other) = default;

        [[nodiscard]] GLuint getShaderID() const { return shaderID; }

        [[nodiscard]] const std::string &getFilename() const { return filename; }

    private:
        static GLuint createMatchingShader(const std::filesystem::path &filename);
    };

}
