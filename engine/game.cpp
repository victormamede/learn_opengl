#include <engine/game.h>
#include <engine/object.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

Game::Game()
{
    // Initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    _window = glfwCreateWindow(_screenSize.x, _screenSize.y, "LearnOpenGL", NULL, NULL);
    if (_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return;
    }
    glfwMakeContextCurrent(_window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, _screenSize.x, _screenSize.y);
    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
    glfwWindowHint(GLFW_SAMPLES, 4);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

    io.IniFilename = nullptr;

    registerCallbacks();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(_window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    glClearColor(0.1f, 0.2f, 0.4f, 1.0f);

    // Create a 1x1 texture that will be used for shaders fallback
    glActiveTexture(GL_TEXTURE12);
    glGenTextures(1, &_whiteTexture);
    glBindTexture(GL_TEXTURE_2D, _whiteTexture);

    // A single white pixel
    unsigned char whitePixel[4] = {255, 255, 255, 255}; // RGBA

    // Upload to GPU
    glTexImage2D(GL_TEXTURE_2D,
                 0,                // mipmap level
                 GL_RGBA,          // internal format
                 1, 1,             // width, height
                 0,                // border
                 GL_RGBA,          // data format
                 GL_UNSIGNED_BYTE, // data type
                 whitePixel);

    // Texture parameters (same as normal textures)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    initialized = true;

    glActiveTexture(GL_TEXTURE0);

    // Show a black screen waiting for run()
    render();
}

void Game::clear()
{
    _objects.clear();
    lights.clear();
    activeCamera = nullptr;
}

Game::~Game()
{
    clear();

    glDeleteTextures(1, &_whiteTexture);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    std::cout << "Destroyed game" << std::endl;
}

void Game::run()
{
    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(_window))
    {
        // Calculate deltaTime
        _time = glfwGetTime();
        _deltaTime = _time - lastFrame;
        lastFrame = _time;

        glfwPollEvents();

        for (const std::unique_ptr<Object> &object : _objects)
            object->notification(Notification::UPDATE);

        render();
    }
}

void Game::render() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glStencilMask(0x00);
    for (const std::unique_ptr<Object> &object : _objects)
        object->notification(Notification::DRAW);

    imguiRender();

    glfwSwapBuffers(_window);
}

float Game::getTime() const { return _time; }
float Game::getDeltaTime() const { return _deltaTime; }
const KeyInput &Game::getKeyInput() const { return _keyInput; }
const MouseMove &Game::getMouseMove() const { return _mouseMove; }
const glm::vec2 &Game::getScreenSize() const { return _screenSize; }

void Game::addObject(std::unique_ptr<Object> object)
{
    object->init(*this);
    object->notification(Notification::START);
    _objects.push_back(std::move(object));
}

void Game::imguiRender() const
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 200.0f), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoSavedSettings))
    {
        ImGui::Text("Mouse pos: %0.f, %0.f", _mouseMove.xPos, _mouseMove.yPos);
        ImGui::Text("Mouse delta: %0.f, %0.f", _mouseMove.delta.x, _mouseMove.delta.y);
        ImGui::Text("Key input: key %0d, action %0d", _keyInput.key, _keyInput.action);

        for (const std::unique_ptr<Object> &object : _objects)
            object->notification(Notification::IMGUI_DRAW);

        if (ImGui::Button("QUIT"))
            glfwSetWindowShouldClose(_window, true);
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::setCursorMode(int mode) const
{
    glfwSetInputMode(_window, GLFW_CURSOR, mode);
}

void Game::registerCallbacks()
{
    glfwSetWindowUserPointer(_window, this);

    glfwSetFramebufferSizeCallback(_window, framebufferSizeCallback);
    glfwSetKeyCallback(_window, keyCallback);
    glfwSetCursorPosCallback(_window, mouseCallback);
    glfwSetMouseButtonCallback(_window, mouseButtonCallback);
    glfwSetScrollCallback(_window, scrollCallback);
}

void Game::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    Game *game = (Game *)glfwGetWindowUserPointer(window);
    game->_screenSize.x = (float)width;
    game->_screenSize.y = (float)height;

    for (const std::unique_ptr<Object> &object : game->_objects)
        object->notification(Notification::SCREEN);

    glViewport(0, 0, width, height);
}

void Game::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    Game *game = (Game *)glfwGetWindowUserPointer(window);
    game->_keyInput = {key, scancode, action, mods};
    for (const std::unique_ptr<Object> &object : game->_objects)
        object->notification(Notification::KEY_INPUT);
}

void Game::mouseCallback(GLFWwindow *window, double xPos, double yPos)
{
    Game *game = (Game *)glfwGetWindowUserPointer(window);

    double prevXPos = game->_mouseMove.xPos;
    double prevYPos = game->_mouseMove.yPos;

    glm::vec2 delta = glm::vec2(
        xPos - prevXPos,
        yPos - prevYPos);

    game->_mouseMove = {
        prevXPos,
        prevYPos,
        xPos,
        yPos,
        delta,
    };

    for (const std::unique_ptr<Object> &object : game->_objects)
        object->notification(Notification::MOUSE_MOVE);
}

void Game::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
}
void Game::scrollCallback(GLFWwindow *window, double xOffset, double yOffset)
{
}