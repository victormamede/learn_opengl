#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "shader.h"
#include "texture.h"

struct Material
{
    std::string name = "Material";
    glm::vec3 ambient = glm::vec3(0.2f);
    glm::vec3 diffuse = glm::vec3(1.0f);
    glm::vec3 specular = glm::vec3(1.0f);
    glm::vec3 emission = glm::vec3(0.0f);
    float shininess = 0.5f;

    std::shared_ptr<Texture> diffuseMap = nullptr;
    std::shared_ptr<Texture> specularMap = nullptr;
    std::shared_ptr<Texture> emissionMap = nullptr;

    void setUniforms(Shader &shader) const;
};
