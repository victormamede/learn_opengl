#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include <engine/transform.h>
#include "imgui.h"

Transform::Transform() : position(glm::vec3(0.0f)),
                         rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
                         scale(glm::vec3(1.0f)) {}

glm::mat4 Transform::getMatrix() const
{
    glm::mat4 matrix(1.0f);

    matrix = glm::translate(matrix, position);
    matrix *= glm::mat4_cast(rotation);
    matrix = glm::scale(matrix, scale);

    return matrix;
}

glm::vec3 Transform::forward() const
{
    return rotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Transform::up() const
{
    return rotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Transform::right() const
{
    return rotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

void Transform::imguiDraw()
{
    ImGui::PushID(this);
    ImGui::SeparatorText("Transform");
    ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
    if (ImGui::DragFloat3("Rotation", glm::value_ptr(eulerAngles), 10.0f))
        rotation = glm::quat(glm::radians(eulerAngles));
    ImGui::DragFloat3("Scale", glm::value_ptr(scale), 0.1f);
    ImGui::PopID();
}