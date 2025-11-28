#include <engine/light.h>
#include <engine/game.h>
#include <format>

void Light::onNotification(Notification type)
{
    switch (type)
    {
    case Notification::START:
        Game &game = getGame();
        game.lights.push_back(this);
        break;
    }
}

void Light::setUniforms(Shader &shader) const
{
}

void DirectionalLight::setUniforms(Shader &shader) const
{
    std::string prefix = std::format("directionalLights[{}]", count);

    shader.setUniform(prefix + ".direction", transform.rotation * glm::vec3(0.0f, 0.0f, 1.0f));
    shader.setUniform(prefix + ".ambient", ambient);
    shader.setUniform(prefix + ".diffuse", diffuse);
    shader.setUniform(prefix + ".specular", specular);

    count++;
    shader.setUniform("directionalLightsCount", count);
}

void PointLight::setUniforms(Shader &shader) const
{

    std::string prefix = std::format("pointLights[{}]", count);

    shader.setUniform(prefix + ".position", transform.position);
    shader.setUniform(prefix + ".ambient", ambient);
    shader.setUniform(prefix + ".diffuse", diffuse);
    shader.setUniform(prefix + ".specular", specular);
    shader.setUniform(prefix + ".constant", constant);
    shader.setUniform(prefix + ".linear", linear);
    shader.setUniform(prefix + ".quadratic", quadratic);

    count++;
    shader.setUniform("pointLightsCount", count);
}

void SpotLight::setUniforms(Shader &shader) const
{

    std::string prefix = std::format("spotLights[{}]", count);

    shader.setUniform(prefix + ".position", transform.position);
    shader.setUniform(prefix + ".direction", transform.rotation * glm::vec3(0.0f, 0.0f, 1.0f));
    shader.setUniform(prefix + ".cutOff", glm::cos(cutOff));
    shader.setUniform(prefix + ".outerCutOff", glm::cos(outerCutOff));
    shader.setUniform(prefix + ".ambient", ambient);
    shader.setUniform(prefix + ".diffuse", diffuse);
    shader.setUniform(prefix + ".specular", specular);
    shader.setUniform(prefix + ".constant", constant);
    shader.setUniform(prefix + ".linear", linear);
    shader.setUniform(prefix + ".quadratic", quadratic);

    count++;
    shader.setUniform("spotLightsCount", count);
}

int PointLight::count = 0;
int DirectionalLight::count = 0;
int SpotLight::count = 0;

void Light::resetCounters()
{
    PointLight::count = 0;
    DirectionalLight::count = 0;
    SpotLight::count = 0;
}