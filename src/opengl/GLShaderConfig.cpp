//
// Created by Merutilm on 2025-06-17.
//
#include "GLShaderConfig.h"

#include <iostream>


namespace merutilm::rff {
    GLShaderConfig::GLShaderConfig(const std::filesystem::path &filename) {
        this->filename = filename.string();
        this->shaderID = createMatchingShader(filename);
    }


    GLShaderConfig::~GLShaderConfig() {
        glDeleteShader(shaderID);
    }

    GLuint GLShaderConfig::createMatchingShader(const std::filesystem::path &filename) {
        GLuint shaderID = 0;
        const auto extension = filename.extension().string();
        if (extension == ".vert") {
            shaderID = glCreateShader(GL_VERTEX_SHADER);
        }
        if (extension == ".frag") {
            shaderID = glCreateShader(GL_FRAGMENT_SHADER);
        }

        if (shaderID == 0){
            std::cerr << "Unsupported shader type : " << extension << std::endl;
        }
        return shaderID;
    }
}
