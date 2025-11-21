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

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, Camera &camera, float deltaTime);
void mouse_callback(GLFWwindow *window, double xPos, double yPos);
void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

struct WindowUserData
{
    WindowUserData(glm::vec2 &pos, Camera &cam)
        : camera(cam), hasLastCursorPosition(false) {}

    bool hasLastCursorPosition;
    glm::vec2 lastCursorPosition;
    Camera &camera;
};

int main()
{
    // Initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
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

    {
        Cube cube;
        Camera camera{
            .screenSize = glm::vec2(800.0f, 600.0f),
            .fov = 70.0f};

        camera.transform.position =
            glm::vec3(0.0f, 0.0f, 3.0f);

        glm::vec2 lastCursorPosition;

        WindowUserData userData(lastCursorPosition, camera);

        glfwSetWindowUserPointer(window, &userData);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        float lastFrame = glfwGetTime();

        glm::quat rotation = glm::angleAxis(glm::radians(90.0f), cube.transform.up());
        cube.transform.rotation *= rotation;

        while (!glfwWindowShouldClose(window))
        {
            // Calculate deltaTime
            float currentFrame = glfwGetTime();
            float deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwPollEvents();
            processInput(window, camera, deltaTime);

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

            glfwSwapBuffers(window);
        }
    }

    glfwTerminate();

    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, Camera &camera, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

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

    camera.transform.position += camera.transform.rotation * movement;
}

void mouse_callback(GLFWwindow *window, double xPos, double yPos)
{
    WindowUserData *userData = (WindowUserData *)glfwGetWindowUserPointer(window);

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
}