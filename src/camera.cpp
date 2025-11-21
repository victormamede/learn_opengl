#include "camera.h"

glm::mat4 Camera::getViewMatrix() const
{
    return glm::inverse(transform.getMatrix());
}

glm::mat4 Camera::getProjectionMatrix() const
{
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(fov), screenSize.x / screenSize.y, 0.1f, 100.0f);

    return projection;
}