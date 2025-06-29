#pragma once


#include <array>
#include <string>
#include <glad.h>

#include "../ui/Constants.h"

namespace merutilm::rff2 {
    class GLShader {
        GLuint shaderProgram;
        GLuint vaoID;

        std::array<int, 6> elementArray = {
            2, 1, 0,
            0, 1, 3
        };


        static void compile(GLuint shader, const char *src);

        void link() const;

        static uint16_t textureUnitToIndex(GLuint textureUnit);

        GLuint getLocation(const std::string &varName) const;

    public:
        explicit GLShader(std::string_view vertexName, std::string_view fragmentName);

        void use() const;

        void draw() const;

        static void detach();

        static GLuint recreateTexture2D(GLuint textureID, uint16_t width, uint16_t height, TextureFormat textureFormat);

        void uploadTexture2D(const std::string &varName, GLuint textureUnit, uint16_t textureID) const;

        void uploadTexture2D(const std::string &varName, GLuint textureUnit, uint16_t textureID, const void *buffer, uint16_t w, uint16_t h,
                             TextureFormat textureFormat) const;

        void uploadDouble(const std::string &varName, double value) const;

        void uploadBool(const std::string &varName, bool value) const;

        void uploadFloat(const std::string &varName, float value) const;

        void upload2i(const std::string &varName, int x, int y) const;

        void uploadInt(const std::string &varName, int value) const;
    };
}