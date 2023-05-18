#ifndef ENTYTYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <btBulletDynamicsCommon.h>

#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "Framebuffer.hpp"
#include "Physics.hpp"

template<typename T>
struct EntityManager
{
    inline static std::size_t     default_size = 100;
    std::vector<T*> Entities;

    EntityManager(size_t size = default_size)
    {
        Entities.reserve(size);
    };

    ~EntityManager()
    {
        for(const auto* e: Entities)
            delete e;
    };

    void update()
    {
        Entities.erase(std::remove_if(Entities.begin(), Entities.end(),[](auto &e){ return !e->body; }) ,Entities.end());
    };

    void DrawEntities( Physics& WorldPhysics, Camera& camera )
    {
        extern const float MAP_LIMITS;
        extern const unsigned int WIDTH;
        extern const unsigned int HEIGHT;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = camera.GetProjectionMatrix(WIDTH, HEIGHT);

        for (const auto &e : Entities)
        {
            if (e->body != nullptr)
            {
                btTransform t = e->body->getWorldTransform();
                btQuaternion rotation   = t.getRotation();
                btVector3 translate = t.getOrigin();
                glm::vec3 scale = e->scale;
                if (translate.getY() < -50.0f)
                {
                    delete e->body->getMotionState();
                    delete e->body->getCollisionShape();
                    WorldPhysics.dynamicsWorld->removeRigidBody(e->body);
                    e->body = nullptr;
                }
                else
                {
                    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0), rotation.getAngle(), glm::vec3(rotation.getAxis().getX(), rotation.getAxis().getY(), rotation.getAxis().getZ()));
                    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(translate.getX(), translate.getY(), translate.getZ()));
                    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), scale);
                    glm::mat4 model = translationMatrix * rotationMatrix * scaleMatrix;

                    e->default_shader->use();
                    e->default_shader->setVec2("resolution", glm::vec2(WIDTH, HEIGHT));
                    e->default_shader->setMat4("projection", projection);
                    e->default_shader->setMat4("view", view);
                    e->default_shader->setMat4("model", model);
                    e->default_shader->setVec3("cameraPos", camera.Position);
                    e->Draw();
                }
            }
        }
    };
};


#endif