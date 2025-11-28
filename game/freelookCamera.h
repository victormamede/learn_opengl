#pragma once

#include <engine/camera.h>

struct Input
{
    bool forward = false;
    bool backward = false;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool boost = false;
};

class FreelookCamera : public Camera
{
    void onNotification(Notification type) override;

private:
    bool _isPaused = false;
    glm::vec2 _lookRotation = glm::vec3(0.0f);

    Input _input;

    void updateRotation();

    void onStart();
    void update();
    void mouseInput();
    void keyInput();
};