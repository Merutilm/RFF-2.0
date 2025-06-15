#include "GLShaderLoader.h"

#include <filesystem>
#include <fstream>
#include <iostream>

#include "../ui/Constants.h"
#include "../ui/Utilities.h"


merutilm::rff::GLShaderLoader::GLShaderLoader(std::string_view vertexName, std::string_view fragmentName) {
    try {

        std::filesystem::path fullVertexName = Utilities::getDefaultPath() / Constants::GLConfig::SHADER_PATH_PREFIX / vertexName;
        std::filesystem::path fullFragmentName = Utilities::getDefaultPath() /  Constants::GLConfig::SHADER_PATH_PREFIX / fragmentName;
        std::ifstream vertexFile(fullVertexName);
        std::ifstream fragmentFile(fullFragmentName);

        if (vertexFile.fail()) {
            std::cerr << Constants::GLConfig::MESSAGE_CANNOT_OPEN_FILE << fullVertexName << std::endl;
            return;
        }

        if (fragmentFile.fail()) {
            std::cerr << Constants::GLConfig::MESSAGE_CANNOT_OPEN_FILE << fullFragmentName << std::endl;
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

        vertexFile.close();
        fragmentFile.close();

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

void merutilm::rff::GLShaderLoader::compile(const int shader, const char* src) {
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


void merutilm::rff::GLShaderLoader::link() const {
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

int merutilm::rff::GLShaderLoader::textureUnitToIndex(const int textureUnit) {
    return textureUnit - GL_TEXTURE0;
}

int merutilm::rff::GLShaderLoader::getLocation(const std::string &varName) const {
    const int loc = glGetUniformLocation(shaderProgram, varName.data());
    if (loc == -1) {
        std::cout << "Error: Cannot found uniform : " + varName << "\n" << std::flush;
    }
    return loc;
}

void merutilm::rff::GLShaderLoader::use() const {
    glUseProgram(shaderProgram);
    glBindVertexArray(vaoID);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void merutilm::rff::GLShaderLoader::draw() const {
    glDrawElements(GL_TRIANGLES, static_cast<GLuint>(elementArray.size()), GL_UNSIGNED_INT, nullptr);
}

void merutilm::rff::GLShaderLoader::detach() {
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

int merutilm::rff::GLShaderLoader::recreateTexture2D(GLuint textureID, const uint16_t width, const uint16_t height,
                                      const TextureFormat textureFormat) {
    if (glIsTexture(textureID)) {
        glDeleteTextures(1, &textureID);
    }
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, textureFormat[Constants::TextureFormats::INTERNAL], width, height, 0,
                 textureFormat[Constants::TextureFormats::FORMAT],
                 textureFormat[Constants::TextureFormats::TYPE], nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return textureID;
}


void merutilm::rff::GLShaderLoader::uploadTexture2D(const std::string &varName, const int textureUnit, const int textureID) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    uploadInt(varName, textureUnitToIndex(textureUnit));
}

void merutilm::rff::GLShaderLoader::uploadTexture2D(const std::string &varName, const int textureUnit, const int textureID,
                                     const void *buffer, const uint16_t w, const uint16_t h,
                                     const TextureFormat textureFormat) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, textureFormat[Constants::TextureFormats::FORMAT],
                    textureFormat[Constants::TextureFormats::TYPE], buffer);


    uploadInt(varName, textureUnitToIndex(textureUnit));
}


void merutilm::rff::GLShaderLoader::uploadDouble(const std::string &varName, const double value) const {
    const int varLocation = getLocation(varName);
    glUniform1d(varLocation, value);
}

void merutilm::rff::GLShaderLoader::uploadBool(const std::string &varName, const bool value) const {
    const int varLocation = getLocation(varName);
    glUniform1i(varLocation, value ? 1 : 0);
}


void merutilm::rff::GLShaderLoader::uploadFloat(const std::string &varName, const float value) const {
    const int varLocation = getLocation(varName);
    glUniform1f(varLocation, value);
}

void merutilm::rff::GLShaderLoader::upload2i(const std::string &varName, const int x, const int y) const {
    const int varLocation = getLocation(varName);
    glUniform2i(varLocation, x, y);
}


void merutilm::rff::GLShaderLoader::uploadInt(const std::string &varName, const int value) const {
    const int varLocation = getLocation(varName);
    glUniform1i(varLocation, value);
}
