#include <engine/game.h>
#include <engine/mesh.h>
#include <engine/light.h>
#include <engine/material.h>

#include <cassert>

#include "data/primitives.hpp"
#include "data/materials.hpp"
#include "freelookCamera.h"

class Cube : public Object
{
public:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Shader> singleColorShader;

    void onNotification(Notification type) override
    {
        switch (type)
        {
        case Notification::UPDATE:
            update();
            break;
        case Notification::DRAW:
            draw();
            break;
        }
    }

private:
    void update()
    {

        // transform.rotation *= glm::angleAxis(deltaTime, glm::vec3(0.5f, 1.0f, 0.2f));
        transform.rotation = glm::normalize(transform.rotation);
    }

    void setupShader(Shader &currentShader, const glm::mat4 &modelMatrix) const
    {
        Game &game = getGame();
        assert(game.activeCamera != nullptr);

        currentShader.use();

        game.activeCamera->setUniforms(currentShader);

        Light::resetCounters();
        for (Light *light : game.lights)
            light->setUniforms(currentShader);

        currentShader.setUniform("model", modelMatrix);
        currentShader.setUniform("normalMatrix", glm::transpose(glm::inverse(modelMatrix)));

        material->setUniforms(currentShader);
    }

    void draw() const
    {
        if (singleColorShader)
        {
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            setupShader(*shader, transform.getMatrix());

            mesh->draw(*shader);

            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            glDisable(GL_DEPTH_TEST);

            setupShader(*singleColorShader, glm::scale(transform.getMatrix(), glm::vec3(1.2f)));

            mesh->draw(*singleColorShader);

            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            setupShader(*shader, glm::scale(transform.getMatrix(), glm::vec3(1.2f)));
            mesh->draw(*shader);
        }

        glStencilMask(0x00);
    }
};

class ModelRenderer : public Object
{
public:
    std::shared_ptr<Model> model;
    std::shared_ptr<Material> material;
    std::shared_ptr<Shader> shader;

    void onNotification(Notification type) override
    {
        switch (type)
        {
        case Notification::UPDATE:
            update();
            break;
        case Notification::DRAW:
            draw();
            break;
        }
    }

private:
    void update()
    {
        // transform.rotation *= glm::angleAxis(deltaTime, glm::vec3(0.5f, 1.0f, 0.2f));
        transform.rotation = glm::normalize(transform.rotation);
    }

    void draw() const
    {
        Game &game = getGame();
        assert(game.activeCamera != nullptr);

        shader->use();

        game.activeCamera->setUniforms(*shader);

        Light::resetCounters();
        for (Light *light : game.lights)
            light->setUniforms(*shader);

        const glm::mat4 &modelMatrix = transform.getMatrix();
        shader->setUniform("model", modelMatrix);
        shader->setUniform("normalMatrix", glm::transpose(glm::inverse(modelMatrix)));

        material->setUniforms(*shader);

        model->draw(*shader);
    }
};

class MyLight : public PointLight
{
public:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    std::shared_ptr<Shader> shader;
    glm::vec3 initialPosition;

    void onNotification(Notification type) override
    {
        PointLight::onNotification(type);

        switch (type)
        {
        case Notification::START:
            onStart();
            break;
        case Notification::UPDATE:
            update();
            break;
        case Notification::DRAW:
            draw();
            break;
        }
    }

private:
    void onStart()
    {
        initialPosition = transform.position;
    }

    void update()
    {
        Game &game = getGame();
        float time = game.getTime();
        float deltaTime = game.getDeltaTime();

        // glm::vec3 lightColor;
        // lightColor.x = sin(time * 2.0f);
        // lightColor.y = sin(time * 0.7f);
        // lightColor.z = sin(time * 1.3f);

        // diffuse = lightColor * glm::vec3(0.5f);
        // ambient = diffuse * glm::vec3(0.2f);
        // material->emission = lightColor;

        // transform.position = initialPosition + glm::vec3(glm::cos(time), glm::sin(time), 0.0f) * 2.0f;
        transform.rotation = glm::angleAxis(deltaTime, glm::vec3(0.0f, 1.0f, 0.0f)) * transform.rotation;
        transform.rotation = glm::normalize(transform.rotation);
    }

    void draw() const
    {
        Game &game = getGame();
        assert(game.activeCamera != nullptr);

        shader->use();

        game.activeCamera->setUniforms(*shader);
        setUniforms(*shader);

        const glm::mat4 &modelMatrix = transform.getMatrix();
        shader->setUniform("model", modelMatrix);
        shader->setUniform("normalMatrix", glm::transpose(glm::inverse(modelMatrix)));

        material->setUniforms(*shader);

        mesh->draw(*shader);

        Transform arrowHead = transform;
        arrowHead.position += arrowHead.forward() * 0.3f;
        arrowHead.scale.x *= 0.5;
        arrowHead.scale.y *= 0.5;
        const glm::mat4 &arrowHeadMatrix = arrowHead.getMatrix();
        shader->setUniform("model", arrowHeadMatrix);
        shader->setUniform("normalMatrix", glm::transpose(glm::inverse(arrowHeadMatrix)));

        mesh->draw(*shader);
    }
};

int main()
{
    Game game;
    {
        std::shared_ptr<Shader> shader = std::make_shared<Shader>("./shaders/vertex.vs", "./shaders/fragment.fs");
        std::shared_ptr<Shader> singleColorShader = std::make_shared<Shader>("./shaders/vertex.vs", "./shaders/singleColor.fs");
        std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(cubeVertices, cubeIndices);
        std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(planeVertices, planeIndices);
        std::shared_ptr<Mesh> grassMesh = std::make_shared<Mesh>(grassVertices, grassIndices);
        grassMesh->cullFaces = false;
        // std::shared_ptr<Model> suzanneModel = std::make_shared<Model>("./assets/suzanne.glb");
        // std::shared_ptr<Model> girlModel = std::make_shared<Model>("./assets/girl/scene.gltf");
        std::shared_ptr<Model> justAGirlModel = std::make_shared<Model>("./assets/just_a_girl/scene.gltf");
        std::shared_ptr<Model> shibaModel = std::make_shared<Model>("./assets/shiba/scene.gltf");
        std::shared_ptr<Texture> containerTexture = std::make_shared<Texture>("./textures/container.png", TextureType::SPECULAR);
        std::shared_ptr<Texture> containerSpecularTexture = std::make_shared<Texture>("./textures/container_specular.png", TextureType::SPECULAR);
        std::shared_ptr<Texture> grassTexture = std::make_shared<Texture>("./textures/grass.png", TextureType::DIFFUSE, false);
        std::shared_ptr<Texture> windowTexture = std::make_shared<Texture>("./textures/window.png", TextureType::DIFFUSE);

        // Camera
        std::unique_ptr<FreelookCamera> camera = std::make_unique<FreelookCamera>();

        camera->fov = 70.0f;
        camera->transform.position.y = 2.0f;
        camera->transform.position.z = -2.0f;
        camera->transform.rotation *= glm::angleAxis(glm::pi<float>(), camera->transform.up());
        camera->objectName = "Camera";

        game.addObject(std::move(camera));

        // Plane
        std::unique_ptr<Cube> plane = std::make_unique<Cube>();
        plane->material = std::make_shared<Material>(
            Material{
                .specular = glm::vec3(0.2f),
                .shininess = 0.2f,
            });

        plane->mesh = planeMesh;
        plane->shader = shader;
        plane->transform.position = glm::vec3(0.0f, -0.25f, 0.0f);
        plane->transform.scale = glm::vec3(50.0f);
        game.addObject(std::move(plane));

        {
            std::unique_ptr<ModelRenderer> suzanne = std::make_unique<ModelRenderer>();
            suzanne->material = std::make_shared<Material>(
                Material{
                    .shininess = 1.0f,
                });

            suzanne->model = justAGirlModel;
            suzanne->shader = shader;
            suzanne->transform.position = glm::vec3(-1.0f, 0.25f, 0.0f);
            suzanne->transform.rotation = glm::quat(glm::vec3(-glm::pi<float>() / 2.0f, 0.0f, 0.0f));
            suzanne->transform.scale = glm::vec3(0.02f);
            game.addObject(std::move(suzanne));
        }

        {
            std::unique_ptr<Cube> box = std::make_unique<Cube>();
            box->material = std::make_shared<Material>(
                Material{
                    .specular = glm::vec3(1.0f),
                    .shininess = 1.0f,
                    .diffuseMap = containerTexture,
                    .specularMap = containerSpecularTexture,
                });

            box->mesh = cubeMesh;
            box->shader = shader;
            box->singleColorShader = singleColorShader;
            box->transform.position = glm::vec3(-3.0f, 0.25f, 0.0f);
            box->transform.rotation = glm::quat(glm::vec3(-glm::pi<float>() / 2.0f, 0.0f, 0.0f));
            box->transform.scale = glm::vec3(0.25);
            game.addObject(std::move(box));
        }

        {
            std::unique_ptr<Cube> window = std::make_unique<Cube>();
            window->objectName = "Window";
            window->material = std::make_shared<Material>(
                Material{
                    .specular = glm::vec3(1.0f),
                    .shininess = 1.0f,
                    .diffuseMap = windowTexture,
                });

            window->mesh = planeMesh;
            window->shader = shader;
            window->transform.position = glm::vec3(-5.0f, 0.5f, 2.0f);
            window->transform.scale = glm::vec3(0.5f);
            window->transform.rotation = glm::quat(glm::radians(glm::vec3(45.0f, -45.0f, 0.0f)));
            game.addObject(std::move(window));
        }

        {
            std::unique_ptr<Cube> grass = std::make_unique<Cube>();
            grass->material = std::make_shared<Material>(
                Material{
                    .specular = glm::vec3(1.0f),
                    .shininess = 1.0f,
                    .diffuseMap = grassTexture,
                });

            grass->mesh = grassMesh;
            grass->shader = shader;
            grass->transform.position = glm::vec3(-5.0f, 0.5f, 0.0f);
            grass->transform.scale = glm::vec3(0.5f);
            game.addObject(std::move(grass));
        }

        for (int i = 0; i < materials.size(); i++)
        {
            Material &material = materials[i];

            std::unique_ptr<ModelRenderer> cube = std::make_unique<ModelRenderer>();

            int x = i % 4;
            int y = i / 4;

            cube->transform.rotation = glm::quat(glm::vec3(-glm::pi<float>() / 2.0f, 0.0f, 0.0f));
            cube->material = std::make_shared<Material>(material);
            cube->objectName = material.name;
            cube->model = shibaModel;
            cube->shader = shader;
            cube->transform.scale = glm::vec3(0.25f);
            cube->transform.position = glm::vec3((float)x, 0.0f, (float)y);

            game.addObject(std::move(cube));
        }

        // Light
        for (int i = 0; i < 3; i++)
        {
            std::unique_ptr<MyLight> light = std::make_unique<MyLight>();
            light->transform.position = glm::vec3(i * 5.0f - 5.0f, 2.0f, i * 5.0f - 5.0f);
            light->transform.scale = glm::vec3(0.1f);
            // light->transform.rotation = glm::quat(glm::radians(glm::vec3(40.0f, 180.0f, 0.0f)));
            light->ambient = glm::vec3(0.2f);
            light->diffuse = glm::vec3(1.0f);
            light->specular = glm::vec3(1.0f);
            light->quadratic = 1.0f;
            light->linear = 0.0f;
            light->objectName = "Light";
            light->material = std::make_shared<Material>(Material{
                .emission = glm::vec3(0.2f)});
            light->mesh = cubeMesh;
            light->shader = shader;

            game.addObject(std::move(light));
        }

        {
            std::unique_ptr<DirectionalLight> light = std::make_unique<DirectionalLight>();
            light->transform.position = glm::vec3(0.0f, 2.0f, 0.0f);
            light->transform.rotation = glm::quat(glm::radians(glm::vec3(40.0f, 180.0f, 40.0f)));
            light->ambient = glm::vec3(0.5f);
            light->diffuse = glm::vec3(0.5f);
            light->specular = glm::vec3(0.5f);
            light->objectName = "DirectionalLight";

            game.addObject(std::move(light));
        }

        for (int i = 0; i <= 5; i++)
        {
            std::unique_ptr<SpotLight> light = std::make_unique<SpotLight>();
            light->transform.position = glm::vec3(i * 5.0f - 10.0f, 2.0f, 0.0f);
            light->transform.rotation = glm::quat(glm::radians(glm::vec3(90.0f, 0.0f, 0.0f)));
            light->ambient = glm::vec3(0.5f);
            light->diffuse = glm::vec3(0.5f);
            light->specular = glm::vec3(0.5f);
            light->outerCutOff = glm::radians(45.0f);
            light->cutOff = glm::radians((float)i * 45.0f / 5.0f);

            light->objectName = "SpotLight";

            game.addObject(std::move(light));
        }
    }

    game.run();

    return 0;
}