#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"

class Cube
{
public:
    Cube();
    Cube(glm::mat4 transform);
    ~Cube();

    void draw(Shader &shader) const;

    glm::mat4 Transform;

private:
    GLuint _vao;
    GLuint _vbo;
};