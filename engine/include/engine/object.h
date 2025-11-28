#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <variant>
#include <string>

#include "transform.h"

class Game;

enum Notification
{
    START,
    UPDATE,
    DRAW,
    IMGUI_DRAW,
    KEY_INPUT,
    MOUSE_MOVE,
    SCREEN,
};

class Object
{
public:
    Object();
    virtual ~Object() = default;

    std::string objectName;
    Transform transform;

    Game &getGame() const;

    void init(Game &game);
    void notification(Notification type);
    virtual void onNotification(Notification type);

private:
    Game *_game;
};
