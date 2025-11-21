#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
    Transform();

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 getMatrix() const;

    glm::vec3 forward() const;
    glm::vec3 up() const;
    glm::vec3 right() const;

    void ImGuiDebug();
};