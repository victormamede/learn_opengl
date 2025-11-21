#include <iostream>

#include "transform.h"

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
    return glm::vec3(0.0f, 0.0f, 1.0f) * rotation;
}

glm::vec3 Transform::up() const
{
    return glm::vec3(0.0f, 1.0f, 0.0f) * rotation;
}

glm::vec3 Transform::right() const
{
    return glm::vec3(1.0f, 0.0f, 0.0f) * rotation;
}
