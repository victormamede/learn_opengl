#include <engine/mesh.h>
#include <assimp/postprocess.h>

Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<unsigned int> &indices,
           std::vector<Texture> textures) : Mesh::Mesh(vertices, indices)
{
    _textures = std::move(textures);
}
Mesh::Mesh(const std::vector<Vertex> &vertices,
           const std::vector<unsigned int> &indices)
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    glBindVertexArray(_vao);

    // VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    _indicesCount = indices.size();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesCount * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    GLsizei stride = sizeof(Vertex);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &_ebo);
    glDeleteBuffers(1, &_vbo);
    glDeleteVertexArrays(1, &_vao);
}

void Mesh::draw(const Shader &shader) const
{
    bool culled = false;
    if (!cullFaces)
    {
        glDisable(GL_CULL_FACE);
        culled = true;
    }

    for (int i = 0; i < _textures.size(); i++)
    {
        const Texture &texture = _textures[i];
        texture.use(i);

        switch (texture.type)
        {
        case TextureType::DIFFUSE:
            shader.setUniform("material.diffuseMap", i);
        case TextureType::SPECULAR:
            shader.setUniform("material.specularMap", i);
        case TextureType::EMISSION:
            shader.setUniform("material.emissionMap", i);
        }
    }

    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    // Re enable for next render
    if (culled)
        glEnable(GL_CULL_FACE);
}

Model::Model(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    _directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        aiVector3D &pos = mesh->mVertices[i];
        aiVector3D &normal = mesh->mNormals[i];
        Vertex vertex =
            {
                .position = {pos.x, pos.y, pos.z},
                .normal = {normal.x, normal.y, normal.z},
            };

        if (mesh->mTextureCoords[0])
        {
            auto &texCoords = mesh->mTextureCoords[0][i];
            vertex.texCoords.x = texCoords.x;
            vertex.texCoords.y = texCoords.y;
        }
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                                                aiTextureType_DIFFUSE);
        std::vector<Texture> specularMaps = loadMaterialTextures(material,
                                                                 aiTextureType_SPECULAR);
        textures.insert(
            textures.end(),
            std::make_move_iterator(diffuseMaps.begin()),
            std::make_move_iterator(diffuseMaps.end()));
        textures.insert(
            textures.end(),
            std::make_move_iterator(specularMaps.begin()),
            std::make_move_iterator(specularMaps.end()));
    }

    _meshes.push_back(std::make_unique<Mesh>(vertices, indices, std::move(textures)));
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type)
{
    std::vector<Texture> textures;
    int count = mat->GetTextureCount(type);

    for (unsigned int i = 0; i < count; i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string fullPath = _directory + "/" + str.C_Str();
        TextureType textureType;

        switch (type)
        {
        case aiTextureType_DIFFUSE:
            textureType = TextureType::DIFFUSE;
            break;
        case aiTextureType_SPECULAR:
            textureType = TextureType::SPECULAR;
            break;
        case aiTextureType_EMISSION_COLOR:
            textureType = TextureType::EMISSION;
            break;
        }

        textures.emplace_back(fullPath, textureType);
    }

    return std::move(textures);
}

void Model::draw(const Shader &shader) const
{
    for (const std::unique_ptr<Mesh> &mesh : _meshes)
        mesh->draw(shader);
}