#include <engine/camera.h>
#include <engine/game.h>

void Camera::onNotification(Notification type)
{
    switch (type)
    {
    case Notification::START:
        setActive(); // TODO: check if current active camera is null in game
        updateProjection();
        break;

    case Notification::SCREEN:
        updateProjection();
        break;
    }
}

void Camera::setActive()
{
    Game &game = getGame();
    game.activeCamera = this;
}

void Camera::updateProjection()
{
    Game &game = getGame();
    const glm::vec2 &screenSize = game.getScreenSize();

    projection = glm::perspective(glm::radians(fov), screenSize.x / screenSize.y, 0.1f, 100.0f);
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::inverse(transform.getMatrix());
}

glm::mat4 Camera::getProjectionMatrix() const
{
    return projection;
}

void Camera::setUniforms(Shader &shader) const
{
    shader.setUniform("view", getViewMatrix());
    shader.setUniform("viewPos", transform.position);
    shader.setUniform("projection", getProjectionMatrix());
}