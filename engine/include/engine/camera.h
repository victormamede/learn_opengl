#pragma once

#include "object.h"
#include "shader.h"

class Camera : public Object
{
public:
    void onNotification(Notification type) override;

    float fov;
    glm::mat4 projection;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void setUniforms(Shader &shader) const;
    void setActive();

private:
    void updateProjection();
};