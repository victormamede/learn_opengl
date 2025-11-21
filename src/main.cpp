#include <iostream>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.h"
#include "cube.h"
#include "camera.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH 1200

struct WindowUserData
{
    WindowUserData(glm::vec2 &pos, Camera &cam)
        : camera(cam), hasLastCursorPosition(false), propertiesShown(false) {}

    bool hasLastCursorPosition;
    glm::vec2 lastCursorPosition;
    Camera &camera;
    bool propertiesShown;
};

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, WindowUserData &userData, float deltaTime);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

int main()
{
    // Initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // Hide cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader shader("./shaders/vertex.vs", "./shaders/fragment.fs");

    GLuint textures[2];
    glGenTextures(2, textures);

    int width, height, nrChannels;
    unsigned char *data;

    glBindTexture(GL_TEXTURE_2D, textures[0]);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("./textures/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, textures[1]);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("./textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

    io.IniFilename = nullptr;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true); // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    {
        Cube cube;
        Camera camera{
            .screenSize = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT),
            .fov = 70.0f};

        camera.transform.position =
            glm::vec3(0.0f, 0.0f, 3.0f);

        glm::vec2 lastCursorPosition;

        WindowUserData userData(lastCursorPosition, camera);

        glfwSetWindowUserPointer(window, &userData);

        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwSetScrollCallback(window, scroll_callback);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        float lastFrame = glfwGetTime();

        while (!glfwWindowShouldClose(window))
        {
            // Calculate deltaTime
            float currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwPollEvents();
            processInput(window, userData, deltaTime);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            shader.setUniform("texture1", 0);
            shader.setUniform("texture2", 1);

            shader.setUniform("view", camera.getViewMatrix());
            shader.setUniform("projection", camera.getProjectionMatrix());

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0]);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, textures[1]);

            shader.setUniform("model", cube.transform.getMatrix());
            cube.draw();

            ImGui::SetNextWindowSizeConstraints(ImVec2(300.0f, 200.0f), ImVec2(FLT_MAX, FLT_MAX));
            if (ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
            {
                if (ImGui::CollapsingHeader("Cube"))
                    cube.transform.ImGuiDebug();

                if (ImGui::CollapsingHeader("Camera"))
                    camera.transform.ImGuiDebug();

                if (ImGui::Button("QUIT"))
                    glfwSetWindowShouldClose(window, true);
            }
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, WindowUserData &userData, float deltaTime)
{
    float moveSpeed = 5.0f;
    glm::vec2 moveDirection = glm::vec2(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        moveDirection.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        moveDirection.y -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        moveDirection.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        moveDirection.x -= 1.0f;

    glm::vec3 movement = glm::vec3(moveDirection.x, 0.0f, -moveDirection.y) * moveSpeed * deltaTime;

    Camera &camera = userData.camera;
    camera.transform.position += camera.transform.rotation * movement;

    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    WindowUserData *userData = (WindowUserData *)glfwGetWindowUserPointer(window);

    if (userData->propertiesShown)
    {
        userData->hasLastCursorPosition = false;
        ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
        return;
    }

    glm::vec2 currentPosition(
        xPos,
        yPos);

    if (!userData->hasLastCursorPosition)
    {
        userData->lastCursorPosition = currentPosition;
        userData->hasLastCursorPosition = true;
    }
    glm::vec2 delta = currentPosition - userData->lastCursorPosition;
    userData->lastCursorPosition = currentPosition;

    float sensitivity = 0.01f;

    glm::vec3 right = userData->camera.transform.right();

    userData->camera.transform.rotation = glm::angleAxis(
                                              -delta.x * sensitivity,
                                              glm::vec3(0.0f, 1.0f, 0.0f)) *
                                          userData->camera.transform.rotation;

    userData->camera.transform.rotation *= glm::angleAxis(
        -delta.y * sensitivity,
        glm::vec3(1.0f, 0.0f, 0.0f));
}

void scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
    WindowUserData *userData = (WindowUserData *)glfwGetWindowUserPointer(window);

    float fov = userData->camera.fov;

    fov -= (float)yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f;

    userData->camera.fov = fov;

    ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    {
        WindowUserData *userData = (WindowUserData *)glfwGetWindowUserPointer(window);

        userData->propertiesShown = !userData->propertiesShown;
        if (userData->propertiesShown)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}