#include "GLShader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "GLShaderLoader.h"
#include "../ui/Constants.h"


namespace merutilm::rff {
    GLShader::GLShader(const std::string_view vertexName, const std::string_view fragmentName) {

        const GLShaderConfig vert = GLShaderLoader::configureShader(vertexName);
        const GLShaderConfig frag = GLShaderLoader::configureShader(fragmentName);
        shaderProgram = GLShaderLoader::configureProgram(vert, frag);

        constexpr std::array<float, 28> vertexArray = {
            //location     //color
            1, -1, 1, 0, 0, 0, 1,
            -1, 1, 1, 0, 0, 0, 1,
            1, 1, 1, 0, 0, 0, 1,
            -1, -1, 1, 0, 0, 0, 1
        };


        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        GLuint vboID;
        glGenBuffers(1, &vboID);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray.data(), GL_STATIC_DRAW);


        GLuint eboID;
        glGenBuffers(1, &eboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementArray), elementArray.data(), GL_STATIC_DRAW);


        constexpr uint16_t positionSize = 3;
        constexpr uint16_t colorSize = 4;
        constexpr uint16_t floatSizeBytes = sizeof(float);
        constexpr uint16_t vertexSizeBytes = (positionSize + colorSize) * floatSizeBytes;

        glVertexAttribPointer(0, positionSize, GL_FLOAT, false, vertexSizeBytes, nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, colorSize, GL_FLOAT, false, vertexSizeBytes,
                              (const void *) size_t(positionSize * floatSizeBytes));
        glEnableVertexAttribArray(1);
    }

    void GLShader::compile(const GLuint shader, const char *src) {
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
        }
    }


    void GLShader::link() const {
        glLinkProgram(shaderProgram);
        GLint success;
        GLint logLength;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        std::string infoLog(logLength, '\0');

        if (success == GL_FALSE) {
            glGetProgramInfoLog(shaderProgram, logLength, nullptr, infoLog.data());
            std::cerr << "Shader linkage Error: " << infoLog << "\n" << std::flush;
        }
    }

    uint16_t GLShader::textureUnitToIndex(const GLuint textureUnit) {
        return static_cast<uint16_t>(textureUnit - GL_TEXTURE0);
    }

    GLuint GLShader::getLocation(const std::string &varName) const {
        const GLuint loc = glGetUniformLocation(shaderProgram, varName.data());
        if (loc == -1) {
            std::cerr << "Error: Cannot found uniform : " + varName << "\n" << std::flush;
        }
        return loc;
    }

    void GLShader::use() const {
        glUseProgram(shaderProgram);
        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    void GLShader::draw() const {
        glDrawElements(GL_TRIANGLES, static_cast<GLuint>(elementArray.size()), GL_UNSIGNED_INT, nullptr);
    }

    void GLShader::detach() {
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
        glBindVertexArray(0);
        glUseProgram(0);
    }

    GLuint GLShader::recreateTexture2D(GLuint textureID, const uint16_t width, const uint16_t height,
                                    const TextureFormat textureFormat) {
        if (glIsTexture(textureID)) {
            glDeleteTextures(1, &textureID);
        }
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, textureFormat[Constants::TextureFormats::INTERNAL], width, height, 0,
                     textureFormat[Constants::TextureFormats::FORMAT],
                     textureFormat[Constants::TextureFormats::TYPE], nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return textureID;
    }


    void GLShader::uploadTexture2D(const std::string &varName, const GLuint textureUnit, const uint16_t textureID) const {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);
        uploadInt(varName, textureUnitToIndex(textureUnit));
    }

    void GLShader::uploadTexture2D(const std::string &varName, const GLuint textureUnit, const uint16_t textureID,
                                   const void *buffer, const uint16_t w, const uint16_t h,
                                   const TextureFormat textureFormat) const {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, textureFormat[Constants::TextureFormats::FORMAT],
                        textureFormat[Constants::TextureFormats::TYPE], buffer);


        uploadInt(varName, textureUnitToIndex(textureUnit));
    }


    void GLShader::uploadDouble(const std::string &varName, const double value) const {
        const GLuint varLocation = getLocation(varName);
        glUniform1d(varLocation, value);
    }

    void GLShader::uploadBool(const std::string &varName, const bool value) const {
        const GLuint varLocation = getLocation(varName);
        glUniform1i(varLocation, value ? 1 : 0);
    }


    void GLShader::uploadFloat(const std::string &varName, const float value) const {
        const GLuint varLocation = getLocation(varName);
        glUniform1f(varLocation, value);
    }

    void GLShader::upload2i(const std::string &varName, const int x, const int y) const {
        const GLuint varLocation = getLocation(varName);
        glUniform2i(varLocation, x, y);
    }


    void GLShader::uploadInt(const std::string &varName, const int value) const {
        const GLuint varLocation = getLocation(varName);
        glUniform1i(varLocation, value);
    }
}
