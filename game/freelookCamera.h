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
private:
    bool _isPaused = false;
    glm::vec2 _lookRotation = glm::vec3(0.0f);

    bool _hasPrevLocation = false;
    glm::vec2 _prevLocation;
    Input _input;

    void updateRotation();

    void onStart() override;
    void update(float deltaTime) override;
    void mouseInput(double xPos, double yPos) override;
    void keyInput(int key, int scancode, int action, int mods) override;
};