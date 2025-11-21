#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "transform.h"

class Cube
{
public:
    Cube();
    ~Cube();

    void draw() const;

    Transform transform;

private:
    GLuint _vao;
    GLuint _vbo;
};