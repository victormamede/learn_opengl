#include <engine/object.h>

#include <imgui.h>

Object::Object() : transform(), objectName("Object"), _game(nullptr) {}

void Object::notification(Notification type)
{
    switch (type)
    {
    case Notification::IMGUI_DRAW:
        ImGui::PushID(this);
        if (ImGui::CollapsingHeader(objectName.c_str()))
            transform.imguiDraw();
        ImGui::PopID();
        break;
    }

    onNotification(type);
}

void Object::init(Game &game)
{
    _game = &game;
}

Game &Object::getGame() const
{
    return *_game;
}

void Object::onNotification(Notification type) {}