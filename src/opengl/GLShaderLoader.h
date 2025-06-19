//
// Created by Merutilm on 2025-06-17.
//

#pragma once
#include <filesystem>
#include <vector>

#include "GLShader.h"
#include "GLShaderConfig.h"

namespace merutilm::rff {

    struct GLShaderLoader {

        GLShaderLoader() = delete;

        static GLShaderConfig configureShader(std::string_view name);

        static void getSource(const std::filesystem::path &path, std::string *src);

        static void compile(GLuint shader, const char *src);

        static GLuint configureProgram(const GLShaderConfig &vert, const GLShaderConfig &frag);

        static void linkProgram(GLuint shaderProgram);
    };

}
