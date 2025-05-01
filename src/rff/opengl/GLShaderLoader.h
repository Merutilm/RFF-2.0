#pragma once


#include <string>
#include <../include/glad/glad.h>


constexpr short MAX_LOG_LEN = 512;
constexpr std::string SHADER_PATH_PREFIX = "../shaders/";
constexpr std::string SHADER_PATH_SUFFIX = ".glsl";
constexpr std::string MESSAGE_CANNOT_OPEN_FILE = "Error: Could not open file: ";

class GLShaderLoader {

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;
    GLuint vaoID;
    GLuint fbo;
    GLuint fboTextureID;

    int elementArray[6] = {
        2, 1, 0,
        0, 1, 3
    };


    void link() const;

    static void compile(int shader, const std::string &src);


public:
    GLShaderLoader(std::string vertexName, std::string fragmentName);
};
