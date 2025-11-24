#include <engine/material.h>

void Material::setUniforms(Shader &shader) const
{
    shader.setUniform("material.ambient", ambient);
    shader.setUniform("material.diffuse", diffuse);
    shader.setUniform("material.specular", specular);
    shader.setUniform("material.shininess", shininess);
    shader.setUniform("material.emission", emission);

    if (diffuseMap)
    {
        diffuseMap->use(1);
        shader.setUniform("material.diffuseMap", 1);
    }
    else
        shader.setUniform("material.diffuseMap", 12);

    if (specularMap)
    {
        specularMap->use(2);
        shader.setUniform("material.specularMap", 2);
    }
    else
        shader.setUniform("material.specularMap", 12);
    if (emissionMap)
    {
        emissionMap->use(3);
        shader.setUniform("material.emissionMap", 3);
    }
    else
        shader.setUniform("material.emissionMap", 12);
}
