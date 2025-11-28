#pragma once

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Object;
class Camera;
class Light;

struct MouseMove
{
    double prevXPos;
    double prevYPos;

    double xPos;
    double yPos;

    glm::vec2 delta;
};

struct KeyInput
{
    int key;
    int scancode;
    int action;
    int mods;
};

class Game
{
public:
    Game();
    ~Game();

    void run();
    void clear();

    void addObject(std::unique_ptr<Object> object);
    void setCursorMode(int value) const;

    float getTime() const;
    float getDeltaTime() const;
    const KeyInput &getKeyInput() const;
    const MouseMove &getMouseMove() const;
    const glm::vec2 &getScreenSize() const;

    Camera *activeCamera = nullptr;
    std::vector<Light *> lights;

private:
    bool initialized = false;
    GLFWwindow *_window = nullptr;
    GLuint _whiteTexture;

    float _time = 0.0f;
    float _deltaTime = 0.0f;
    KeyInput _keyInput = {0};
    MouseMove _mouseMove = {0};
    glm::vec2 _screenSize = glm::vec2(800.0f, 600.0f);

    std::vector<std::unique_ptr<Object>> _objects;

    void registerCallbacks();
    void render() const;
    void imguiRender() const;

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow *window, double xPos, double yPos);
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);
};