#pragma once

#include <vector>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1200

class Object;
class Camera;
class Light;

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

    Camera *activeCamera = nullptr;
    std::vector<Light *> lights;

private:
    bool initialized = false;
    GLFWwindow *_window = nullptr;
    GLuint _whiteTexture;
    float _time = 0.0f;

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