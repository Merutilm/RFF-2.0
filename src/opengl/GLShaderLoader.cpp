//
// Created by Merutilm on 2025-06-17.
//


#include "GLShaderLoader.h"

#include <fstream>

#include "../ui/Utilities.h"

namespace merutilm::rff2 {

    void GLShaderLoader::getSource(const std::filesystem::path &path, std::string *src) {
        std::ifstream file(path);

        if (file.fail()) {
            std::cerr << Constants::GLConfig::MESSAGE_CANNOT_OPEN_FILE << path << std::endl;
            return;
        }

        std::string result;
        std::string line;
        while (getline(file, line)) {
            result += line;
            result.push_back('\n');
        }
        file.close();

        *src = result;
    }


    GLShaderConfig GLShaderLoader::configureShader(const std::string_view name) {
        const auto filename = Utilities::getDefaultPath() / Constants::GLConfig::SHADER_DIRECTORY_DEFAULT / name;
        const auto config = GLShaderConfig(filename);
        const GLuint shaderID = config.getShaderID();
        std::string src;
        getSource(filename, &src);
        compile(shaderID, src.data());

        if (const GLuint error = glGetError(); error != GL_NO_ERROR) {
            std::cout << "OpenGL Error at Load Shaders : " << name << std::flush;
            exit(error);
        }
        return config;
    }

    void GLShaderLoader::compile(const GLuint shader, const char *src) {
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint success;
        GLint logLength;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        std::string infoLog(logLength, '\0');

        if (success == GL_FALSE) {
            glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
            std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
            throw std::runtime_error(infoLog);
        }
    }

    GLuint GLShaderLoader::configureProgram(const GLShaderConfig &vert, const GLShaderConfig &frag) {
        const GLuint program = glCreateProgram();

        if (!glIsShader(vert.getShaderID())) {
            std::cerr << "Invalid vertex shader ID : " << vert.getFilename() << "\n" << std::flush;
        }
        if (!glIsShader(frag.getShaderID())) {
            std::cerr << "Invalid fragment shader ID : " << frag.getFilename() << "\n" << std::flush;
        }

        glAttachShader(program, vert.getShaderID());
        glAttachShader(program, frag.getShaderID());

        linkProgram(program);

        return program;
    }


    void GLShaderLoader::linkProgram(const GLuint shaderProgram) {
        glLinkProgram(shaderProgram);
        GLint success;
        GLint logLength;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        std::string infoLog(logLength, '\0');

        if (success == GL_FALSE) {
            glGetProgramInfoLog(shaderProgram, logLength, nullptr, infoLog.data());
            std::cerr << "Shader linkage Error: " << infoLog.data() << "\n" << std::flush;
        }
    }
}
