#pragma once

#include "transform.h"

class Camera
{
public:
    Transform transform;
    glm::vec2 screenSize;

    float fov;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
};