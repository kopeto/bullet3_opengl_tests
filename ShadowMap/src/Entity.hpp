#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <glm/glm.hpp>

#include "Model.hpp"
#include "EntityManager.hpp"

struct Ball : public GE::Entity
{
    Ball();
    Ball(glm::vec3 _pos, glm::vec3 vel, float radius);
    ~Ball() override;

    void updateModelTransform() override;
    glm::mat4 getModelTransformationMatrix() const override;

    glm::vec3 velocity;
    float radius;
    bool selected = false;

    void config();
};

struct Ground : public GE::Entity
{
    Ground(glm::vec3 _pos, glm::vec2 _dimensions, glm::mat4 _init_rotation = glm::mat4(0));
    ~Ground() override;

    void updateModelTransform() override;
    glm::mat4 getModelTransformationMatrix() const override;

    glm::vec2 dimensions;
};

struct ThrowingCube : public GE::Entity
{
    ThrowingCube(glm::vec3 _pos, glm::vec3 _dim, glm::vec3 vel, glm::mat4 _init_rotation = glm::mat4(0));
    ~ThrowingCube() override;

    void updateModelTransform() override;
    glm::mat4 getModelTransformationMatrix() const override;

    glm::vec3 velocity;
    glm::vec3 dimensions;
    bool selected = false;
};

struct Donut : public GE::Entity
{
    Donut(glm::vec3 _pos, glm::vec3 vel, float radius);
    ~Donut() override;

    void updateModelTransform() override;
    glm::mat4 getModelTransformationMatrix() const override;

    glm::vec3 velocity;
    float radius;
    bool selected = false;
};

#endif