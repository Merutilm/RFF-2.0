#pragma once


#include <array>
#include <string>
#include <glad.h>

constexpr short MAX_LOG_LEN = 512;
constexpr auto SHADER_PATH_PREFIX = "../shaders/";
constexpr auto VERTEX_PATH_SUFFIX = ".vert";
constexpr auto FRAGMENT_PATH_SUFFIX = ".frag";
constexpr auto MESSAGE_CANNOT_OPEN_FILE = "Error: Could not open file: ";

class GLShaderLoader {

    GLuint shaderProgram;
    GLuint vaoID;
    GLuint fbo;
    GLuint fboTextureID;

    std::array<int, 6> elementArray = {
        2, 1, 0,
        0, 1, 3
    };


    void link() const;

    static void compile(int shader, const std::string &src);


public:
    GLShaderLoader(std::string vertexName, std::string fragmentName);
};
