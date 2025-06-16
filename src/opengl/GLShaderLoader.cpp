//
// Created by Merutilm on 2025-06-17.
//


#include "GLShaderLoader.h"

#include <fstream>

#include "../ui/Utilities.h"

namespace merutilm::rff {

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


    void GLShaderLoader::loadShaders() {
        const auto shaderDirectory = Utilities::getDefaultPath() / Constants::GLConfig::SHADER_DIRECTORY_DEFAULT;
        if (!std::filesystem::is_directory(shaderDirectory)) {
            return;
        }

        for (const auto &path: std::filesystem::directory_iterator(shaderDirectory)) {
            std::filesystem::path shaderPath = path;
            addShader(shaderPath);
        }
    }

    void GLShaderLoader::addShader(const std::filesystem::path &path) {
        const auto name = path.filename();
        auto config = std::make_unique<GLShaderConfig>(name);
        const GLuint shaderID = config->getShaderID();
        std::string src;
        getSource(path, &src);
        compile(shaderID, src.data());
        shaders.push_back(std::move(config));

        if (const GLuint error = glGetError(); error != GL_NO_ERROR) {
            std::cout << "OpenGL Error at Load Shaders : " << path << std::flush;
            exit(error);
        }
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

    GLuint GLShaderLoader::configureProgram(const std::string_view vertName, const std::string_view fragName) {
        const GLuint program = glCreateProgram();
        GLuint vert = 0;
        GLuint frag = 0;

        for (const auto &shader: shaders) {
            if (shader->getFilename() == vertName) {
                vert = shader->getShaderID();
            }
            if (shader->getFilename() == fragName) {
                frag = shader->getShaderID();
            }
        }
        if (vert == 0) {
            std::cerr << "Cannot find vertex shader : " << vertName << std::endl;
        }
        if (frag == 0) {
            std::cerr << "Cannot find fragment shader : " << fragName << std::endl;
        }

        glAttachShader(program, vert);
        glAttachShader(program, frag);

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
