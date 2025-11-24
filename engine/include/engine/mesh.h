#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "camera.h"
#include "light.h"
#include "material.h"

class Mesh
{
public:
    Mesh() = delete;
    Mesh(const std::vector<float> &vertices,
         const std::vector<unsigned int> &indices);
    ~Mesh();

    void draw() const;

private:
    unsigned int indicesCount;

    GLuint _vao;
    GLuint _ebo;
    GLuint _vbo;
};