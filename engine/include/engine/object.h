#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>

#include "transform.h"

class Game;

class Object
{
public:
    Object();
    virtual ~Object() = default;

    std::string objectName;
    Transform transform;

    Game &getGame() const;

private:
    Game *_game;

    void init(Game &game);
    virtual void onStart();

    virtual void update(float deltaTime);
    virtual void draw() const;

    virtual void mouseInput(double xPos, double yPos);
    virtual void keyInput(int key, int scancode, int action, int mods);

    virtual void imguiDraw();

    friend class Game;
};
