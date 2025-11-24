#pragma once

#include "object.h"
#include "shader.h"

class Camera : public Object
{
public:
    void onStart() override;

    glm::vec2 screenSize;

    float fov;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void setUniforms(Shader &shader) const;
};