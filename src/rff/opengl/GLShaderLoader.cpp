
#include "GLShaderLoader.h"
#include <fstream>
#include <iostream>

void GLShaderLoader::compile(const int shader, const std::string &src) {
    const char *sourcePtr = src.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);
    GLint success;
    char infoLog[MAX_LOG_LEN];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(shader, MAX_LOG_LEN, nullptr, infoLog);
        std::cout << "Error: " << infoLog << std::endl;
    }
}


void GLShaderLoader::link() const {
    glLinkProgram(shaderProgram);
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    char infoLog[MAX_LOG_LEN];

    if (success == GL_FALSE) {
        glGetProgramInfoLog(shaderProgram, MAX_LOG_LEN, nullptr, infoLog);
        std::cout << "Error: " << infoLog << std::endl;
    }
}


GLShaderLoader::GLShaderLoader(std::string vertexName, std::string fragmentName) {
    {
        try {
            vertexName = SHADER_PATH_PREFIX + vertexName + SHADER_PATH_SUFFIX;
            fragmentName = SHADER_PATH_PREFIX + fragmentName + SHADER_PATH_SUFFIX;
            std::ifstream vertexFile(vertexName);
            std::ifstream fragmentFile(fragmentName);

            if (vertexFile.fail()) {
                std::cout << MESSAGE_CANNOT_OPEN_FILE << vertexName << std::endl;
                return;
            }

            if (fragmentFile.fail()) {
                std::cout << MESSAGE_CANNOT_OPEN_FILE << fragmentName << std::endl;
                return;
            }


            std::string src;
            std::string vertexSrc;
            std::string fragmentSrc;

            while (getline(vertexFile, src)) {
                vertexSrc += src;
                vertexSrc.push_back('\n');
            };

            while (getline(fragmentFile, src)) {
                fragmentSrc += src;
                fragmentSrc.push_back('\n');
            }

            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            compile(vertexShader, vertexSrc);

            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            compile(fragmentShader, fragmentSrc);

            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertexShader);
            glAttachShader(shaderProgram, fragmentShader);
            link();

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        } catch (const std::ifstream::failure &e) {
            std::cout << "Error: " << e.what() << std::endl;
        }

        float vertexArray[28] = {
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);


        GLuint eboID;
        glGenBuffers(1, &eboID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementArray), elementArray, GL_STATIC_DRAW);


        int positionSize = 3;
        int colorSize = 4;
        int floatSizeBytes = sizeof(float);
        int vertexSizeBytes = (positionSize + colorSize) * floatSizeBytes;

        glVertexAttribPointer(0, positionSize, GL_FLOAT, false, vertexSizeBytes, nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, colorSize, GL_FLOAT, false, vertexSizeBytes,
                              (const void *) (positionSize * floatSizeBytes));
        glEnableVertexAttribArray(1);

        //-----------------------------------------------
        //Generate FBO
        //-----------------------------------------------
        glGenFramebuffers(1, &fbo);
        glGenTextures(1, &fboTextureID);
    }
}
