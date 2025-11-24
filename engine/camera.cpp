#include <engine/camera.h>
#include <engine/game.h>

void Camera::onStart()
{
    Game &game = getGame();
    if (game.activeCamera == nullptr)
        game.activeCamera = this;
}

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

void Camera::setUniforms(Shader &shader) const
{
    shader.setUniform("view", getViewMatrix());
    shader.setUniform("viewPos", transform.position);
    shader.setUniform("projection", getProjectionMatrix());
}