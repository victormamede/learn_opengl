#include "freelookCamera.h"

#include <engine/game.h>
#include <GLFW/glfw3.h>

void FreelookCamera::onNotification(Notification type)
{
    Camera::onNotification(type);

    switch (type)
    {
    case Notification::START:
        onStart();
        break;
    case Notification::UPDATE:
        update();
        break;

    case Notification::MOUSE_MOVE:
        mouseInput();
        break;

    case Notification::KEY_INPUT:
        keyInput();
        break;
    }
}

void FreelookCamera::onStart()
{
    Game &game = getGame();
    game.setCursorMode(GLFW_CURSOR_DISABLED);

    updateRotation();
}

void FreelookCamera::update()
{
    float deltaTime = getGame().getDeltaTime();

    if (_isPaused)
        return;

    const float moveSpeed = 5.0f;
    const float boostMultiplier = 2.0f;

    glm::vec3 moveDirection = glm::vec3(0.0f);
    if (_input.forward)
        moveDirection.z -= 1.0f;
    if (_input.backward)
        moveDirection.z += 1.0f;
    if (_input.right)
        moveDirection.x += 1.0f;
    if (_input.left)
        moveDirection.x -= 1.0f;

    glm::vec3 movement = transform.rotation * moveDirection;
    if (_input.up)
        movement.y += 1.0f;
    if (_input.down)
        movement.y -= 1.0f;

    float finalSpeed = moveSpeed;
    if (_input.boost)
        finalSpeed *= boostMultiplier;

    movement *= finalSpeed;

    transform.position += movement * deltaTime;
}

void FreelookCamera::mouseInput()
{
    const MouseMove &mouseMove = getGame().getMouseMove();
    if (_isPaused)
        return;

    float sensitivity = 0.01f;

    glm::vec3 right = transform.right();

    _lookRotation.x += mouseMove.delta.x * sensitivity;
    _lookRotation.x = glm::mod(_lookRotation.x, glm::radians(360.0f));

    _lookRotation.y += mouseMove.delta.y * sensitivity;
    _lookRotation.y = glm::clamp(_lookRotation.y, glm::radians(-89.0f), glm::radians(89.0f));

    updateRotation();
}

void FreelookCamera::updateRotation()
{
    glm::quat rotation = glm::angleAxis(
        -_lookRotation.x,
        glm::vec3(0.0f, 1.0f, 0.0f));

    rotation *= glm::angleAxis(
        -_lookRotation.y,
        glm::vec3(1.0f, 0.0f, 0.0f));

    transform.rotation = rotation;
}

inline void handleKey(bool &result, int action);
void FreelookCamera::keyInput()
{
    Game &game = getGame();
    const KeyInput &keyInput = game.getKeyInput();

    int action = keyInput.action;

    switch (keyInput.key)
    {
    case GLFW_KEY_W:
        handleKey(_input.forward, action);
        break;
    case GLFW_KEY_S:
        handleKey(_input.backward, action);
        break;
    case GLFW_KEY_A:
        handleKey(_input.left, action);
        break;
    case GLFW_KEY_D:
        handleKey(_input.right, action);
        break;
    case GLFW_KEY_SPACE:
        handleKey(_input.up, action);
        break;
    case GLFW_KEY_LEFT_CONTROL:
        handleKey(_input.down, action);
        break;
    case GLFW_KEY_LEFT_SHIFT:
        handleKey(_input.boost, action);
        break;

    case GLFW_KEY_ESCAPE:
        if (action == GLFW_RELEASE)
        {
            _isPaused = !_isPaused;
            if (_isPaused)
                game.setCursorMode(GLFW_CURSOR_NORMAL);
            else
                game.setCursorMode(GLFW_CURSOR_DISABLED);
        }
        break;
    }
}

inline void handleKey(bool &result, int action)
{
    if (action == GLFW_PRESS)
        result = true;

    if (action == GLFW_RELEASE)
        result = false;
}