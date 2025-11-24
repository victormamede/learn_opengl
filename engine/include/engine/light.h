#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include "object.h"
#include "shader.h"

class Light : public Object
{
public:
    virtual ~Light() = default;

    void onStart() override;

    glm::vec3 ambient = glm::vec3(0.0f);
    glm::vec3 diffuse = glm::vec3(0.0f);
    glm::vec3 specular = glm::vec3(0.0f);

    virtual void setUniforms(Shader &shader) const;
    static void resetCounters();
};

class DirectionalLight : public Light
{
public:
    // TODO: have some kind of light manager
    static int count;

    void setUniforms(Shader &shader) const override;
};

class PointLight : public Light
{
public:
    // TODO: have some kind of light manager
    static int count;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.6f;

    void setUniforms(Shader &shader) const override;
};

class SpotLight : public PointLight
{
public:
    // TODO: have some kind of light manager
    static int count;

    float cutOff = glm::radians(20.0f);
    float outerCutOff = glm::radians(45.0f);

    void setUniforms(Shader &shader) const override;
};