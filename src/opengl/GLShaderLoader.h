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

        static std::vector<std::unique_ptr<GLShaderConfig>> shaders;

        GLShaderLoader() = delete;

        static void loadShaders();

        static void addShader(const std::filesystem::path &path);

        static void getSource(const std::filesystem::path &path, std::string *src);

        static void compile(GLuint shader, const char *src);

        static GLuint configureProgram(std::string_view vertName, std::string_view fragName);

        static void linkProgram(GLuint shaderProgram);
    };

    inline std::vector<std::unique_ptr<GLShaderConfig>> GLShaderLoader::shaders = {};
}
