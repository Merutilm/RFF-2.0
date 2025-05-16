#include "GLShaderLoader.h"

#include <fstream>
#include <iostream>

#include "../ui/RFF.h"


GLShaderLoader::GLShaderLoader(std::string_view vertexName, std::string_view fragmentName) {
    try {
        std::string fullVertexName;
        fullVertexName.append(RFF::GLConfig::SHADER_PATH_PREFIX).append(vertexName).append(RFF::GLConfig::VERTEX_PATH_SUFFIX);
        std::string fullFragmentName;
        fullFragmentName.append(RFF::GLConfig::SHADER_PATH_PREFIX).append(fragmentName).append(RFF::GLConfig::FRAGMENT_PATH_SUFFIX);
        std::ifstream vertexFile(fullVertexName);
        std::ifstream fragmentFile(fullFragmentName);

        if (vertexFile.fail()) {
            std::cout << RFF::GLConfig::MESSAGE_CANNOT_OPEN_FILE << fullVertexName << std::endl;
            return;
        }

        if (fragmentFile.fail()) {
            std::cout << RFF::GLConfig::MESSAGE_CANNOT_OPEN_FILE << fullFragmentName << std::endl;
            return;
        }


        std::string src;
        std::string vertexSrc;
        std::string fragmentSrc;

        while (getline(vertexFile, src)) {
            vertexSrc += src;
            vertexSrc.push_back('\n');
        }

        while (getline(fragmentFile, src)) {
            fragmentSrc += src;
            fragmentSrc.push_back('\n');
        }

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        compile(vertexShader, vertexSrc.data());

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        compile(fragmentShader, fragmentSrc.data());

        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        link();

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    } catch (const std::ifstream::failure &e) {
        std::cout << "OpenGL Error : " << e.what() << std::endl;
    }

    std::array<float, 28> vertexArray = {
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


    int positionSize = 3;
    int colorSize = 4;
    int floatSizeBytes = sizeof(float);
    int vertexSizeBytes = (positionSize + colorSize) * floatSizeBytes;

    glVertexAttribPointer(0, positionSize, GL_FLOAT, false, vertexSizeBytes, nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, colorSize, GL_FLOAT, false, vertexSizeBytes,
                          (const void *) size_t(positionSize * floatSizeBytes));
    glEnableVertexAttribArray(1);
}

void GLShaderLoader::compile(const int shader, const char* src) {
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success;
    GLint logLength;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::string infoLog(logLength, '\0');

    if (success == GL_FALSE) {
        glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
        std::cout << "Shader Compilation Error: " << infoLog << std::endl;
    }
}


void GLShaderLoader::link() const {
    glLinkProgram(shaderProgram);
    GLint success;
    GLint logLength;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
    std::string infoLog(logLength, '\0');

    if (success == GL_FALSE) {
        glGetProgramInfoLog(shaderProgram, logLength, nullptr, infoLog.data());
        std::cout << "Shader linkage Error: " << infoLog << "\n" << std::flush;
    }
}

int GLShaderLoader::textureUnitToIndex(const int textureUnit) {
    return textureUnit - GL_TEXTURE0;
}

int GLShaderLoader::getLocation(const std::string &varName) const {
    const int loc = glGetUniformLocation(shaderProgram, varName.data());
    if (loc == -1) {
        std::cout << "Error: Cannot found uniform : " + varName << "\n" << std::flush;
    }
    return loc;
}

void GLShaderLoader::use() const {
    glUseProgram(shaderProgram);
    glBindVertexArray(vaoID);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void GLShaderLoader::draw() const {
    glDrawElements(GL_TRIANGLES, static_cast<GLuint>(elementArray.size()), GL_UNSIGNED_INT, nullptr);
}

void GLShaderLoader::detach() {
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

int GLShaderLoader::recreateTexture2D(GLuint textureID, const int width, const int height,
                                      const TextureFormat textureFormat,
                                      const bool linearInterpolation) {
    if (glIsTexture(textureID)) {
        glDeleteTextures(1, &textureID);
    }
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat[RFF::TextureFormats::INTERNAL], width, height, 0,
                 textureFormat[RFF::TextureFormats::FORMAT],
                 textureFormat[RFF::TextureFormats::TYPE], nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    if (linearInterpolation) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    return textureID;
}


void GLShaderLoader::uploadTexture2D(const std::string &varName, const int textureUnit, const int textureID) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    uploadInt(varName, textureUnitToIndex(textureUnit));
}

void GLShaderLoader::uploadTexture2D(const std::string &varName, const int textureUnit, const int textureID,
                                     const void *buffer, const int w, const int h,
                                     const TextureFormat textureFormat) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, textureFormat[RFF::TextureFormats::FORMAT],
                    textureFormat[RFF::TextureFormats::TYPE], buffer);


    uploadInt(varName, textureUnitToIndex(textureUnit));
}


void GLShaderLoader::uploadDouble(const std::string &varName, const double value) const {
    const int varLocation = getLocation(varName);
    glUniform1d(varLocation, value);
}

void GLShaderLoader::uploadBool(const std::string &varName, const bool value) const {
    const int varLocation = getLocation(varName);
    glUniform1i(varLocation, value ? 1 : 0);
}


void GLShaderLoader::uploadFloat(const std::string &varName, const float value) const {
    const int varLocation = getLocation(varName);
    glUniform1f(varLocation, value);
}

void GLShaderLoader::upload2i(const std::string &varName, const int x, const int y) const {
    const int varLocation = getLocation(varName);
    glUniform2i(varLocation, x, y);
}


void GLShaderLoader::uploadInt(const std::string &varName, const int value) const {
    const int varLocation = getLocation(varName);
    glUniform1i(varLocation, value);
}
