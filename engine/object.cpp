#include <engine/object.h>

#include <imgui.h>

Object::Object() : transform(), objectName("Object"), _game(nullptr) {}

void Object::init(Game &game)
{
    _game = &game;
    onStart();
}

Game &Object::getGame() const
{
    return *_game;
}

void Object::imguiDraw()
{
    ImGui::PushID(this);
    if (ImGui::CollapsingHeader(objectName.c_str()))
        transform.imguiDraw();
    ImGui::PopID();
}

void Object::onStart() {}
void Object::update(float deltaTime) {}
void Object::draw() const {}

void Object::mouseInput(double xPos, double yPos) {}
void Object::keyInput(int key, int scancode, int action, int mods) {}