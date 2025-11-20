#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    // the program ID
    GLuint ID;

    // constructor reads and builds the shader
    Shader(const char *vertexPath, const char *fragmentPath);

    // use/activate the shader
    void use();

    // utility uniform functions
    void setUniform(const std::string &name, bool value) const;
    void setUniform(const std::string &name, int value) const;
    void setUniform(const std::string &name, float value) const;
    void setUniform(const std::string &name, float x, float y, float z, float w) const;
    void setUniform(const std::string &name, const glm::mat4 &matrix) const;
};
