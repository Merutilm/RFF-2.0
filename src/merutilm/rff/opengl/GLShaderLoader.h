#pragma once


#include <array>
#include <string>
#include <glad.h>

#include "../ui/RFFConstants.h"

class GLShaderLoader {
    GLuint shaderProgram;
    GLuint vaoID;

    std::array<int, 6> elementArray = {
        2, 1, 0,
        0, 1, 3
    };


    static void compile(int shader, const char* src);

    void link() const;

    static int textureUnitToIndex(int textureUnit);

    int getLocation(const std::string &varName) const;

public:
    GLShaderLoader(std::string_view vertexName, std::string_view fragmentName);

    void use() const;

    void draw() const;

    static void detach();

    static int recreateTexture2D(GLuint textureID, int width, int height, TextureFormat textureFormat,
                                 bool linearInterpolation);
    void uploadTexture2D(const std::string &varName, int textureUnit, int textureID) const;

    void uploadTexture2D(const std::string &varName, int textureUnit, int textureID, const void *buffer, int w, int h,
                         TextureFormat textureFormat) const;

    void uploadDouble(const std::string &varName, double value) const;

    void uploadBool(const std::string &varName, bool value) const;

    void uploadFloat(const std::string &varName, float value) const;

    void upload2i(const std::string &varName, int x, int y) const;

    void uploadInt(const std::string &varName, int value) const;
};
