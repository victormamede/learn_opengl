#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "shader.h"
#include "texture.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

class Mesh
{
public:
    Mesh() = delete;
    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices);
    Mesh(const std::vector<Vertex> &vertices,
         const std::vector<unsigned int> &indices,
         std::vector<Texture> textures);
    ~Mesh();

    void draw(const Shader &shader) const;

private:
    unsigned int _indicesCount;
    std::vector<Texture> _textures;

    GLuint _vao;
    GLuint _ebo;
    GLuint _vbo;
};

class Model
{
public:
    Model(const std::string &path);
    void draw(const Shader &shader) const;

private:
    // model data
    std::vector<std::unique_ptr<Mesh>> _meshes;
    std::string _directory;

    void processNode(aiNode *node, const aiScene *scene);
    void processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type);
};