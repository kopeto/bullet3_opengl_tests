#include "Entity.hpp"

Ball::Ball() {}

Ball::Ball(glm::vec3 _pos, glm::vec3 _vel, float _radius)
    : velocity{_vel}, radius{_radius}
{
    position = _pos;
    std::printf("Entity<Ball> ID:%u\n", m_id);
}

Ball::~Ball()
{
    std::printf("Entity<Ball> ID:%u deleted | ", m_id);
}

void Ball::updateModelTransform()
{
    btTransform t = body->getWorldTransform();
    position = {t.getOrigin().getX(), t.getOrigin().getY(), t.getOrigin().getZ()};
    btQuaternion rot = t.getRotation();
    rotation = glm::rotate(glm::mat4(1.0), rot.getAngle(), glm::vec3(rot.getAxis().getX(), rot.getAxis().getY(), rot.getAxis().getZ()));
    velocity = {body->getLinearVelocity().getX(), body->getLinearVelocity().getY(), body->getLinearVelocity().getX()};
}

glm::mat4 Ball::getModelTransformationMatrix() const
{
    glm::vec3 scale{radius};
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), position);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), scale);

    return translationMatrix * rotation * scaleMatrix;
}


Donut::Donut(glm::vec3 _pos, glm::vec3 _vel, float _radius)
    : velocity{_vel}, radius{_radius}
{
    position = _pos;
    std::printf("Entity<Donut> ID:%u\n", m_id);
}

Donut::~Donut()
{
    std::printf("Entity<Donut> ID:%u deleted | ", m_id);
}

void Donut::updateModelTransform()
{
    btTransform t = body->getWorldTransform();
    position = {t.getOrigin().getX(), t.getOrigin().getY(), t.getOrigin().getZ()};
    btQuaternion rot = t.getRotation();
    rotation = glm::rotate(glm::mat4(1.0), rot.getAngle(), glm::vec3(rot.getAxis().getX(), rot.getAxis().getY(), rot.getAxis().getZ()));
    velocity = {body->getLinearVelocity().getX(), body->getLinearVelocity().getY(), body->getLinearVelocity().getX()};
}

glm::mat4 Donut::getModelTransformationMatrix() const
{
    glm::vec3 scale{radius};
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), position);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), scale);

    return translationMatrix * rotation * scaleMatrix;
}

Ground::Ground(glm::vec3 _position, glm::vec2 _dimensions, glm::mat4 _init_rotation)
    : dimensions{_dimensions}
{
    rotation = _init_rotation;
    position = _position;
    std::printf("Entity<Ground> ID:%u\n", m_id);
}

Ground::~Ground()
{
    // std::printf("Entity<Ground> ID:%u deleted | ", m_id);
}

void Ground::updateModelTransform()
{
    btTransform t = body->getWorldTransform();
    position = {t.getOrigin().getX(), t.getOrigin().getY(), t.getOrigin().getZ()};
    btQuaternion rot = t.getRotation();
    rotation = glm::rotate(glm::mat4(1.0), rot.getAngle(), glm::vec3(rot.getAxis().getX(), rot.getAxis().getY(), rot.getAxis().getZ()));
}

glm::mat4 Ground::getModelTransformationMatrix() const
{
    glm::vec3 scale{dimensions.x, 1.0f, dimensions.y};
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), position);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), scale);

    return translationMatrix * rotation * scaleMatrix;
}

ThrowingCube::ThrowingCube(glm::vec3 _pos, glm::vec3 _dim, glm::vec3 _vel, glm::mat4 _init_rotation)
    : velocity{_vel}, dimensions{_dim}
{
    rotation = _init_rotation;
    position = _pos;
    std::printf("Entity<ThrowingCube> ID:%u\n", m_id);
}

ThrowingCube::~ThrowingCube()
{
    std::printf("Entity<ThrowingCube> ID:%u deleted | ", m_id);
}

void ThrowingCube::updateModelTransform()
{
    btTransform t = body->getWorldTransform();
    position = {t.getOrigin().getX(), t.getOrigin().getY(), t.getOrigin().getZ()};
    btQuaternion rot = t.getRotation();
    rotation = glm::rotate(glm::mat4(1.0), rot.getAngle(), glm::vec3(rot.getAxis().getX(), rot.getAxis().getY(), rot.getAxis().getZ()));
    velocity = {body->getLinearVelocity().getX(), body->getLinearVelocity().getY(), body->getLinearVelocity().getX()};
}

glm::mat4 ThrowingCube::getModelTransformationMatrix() const
{
    glm::vec3 scale{dimensions.x, dimensions.y, dimensions.z};
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), position);
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0), scale);

    return translationMatrix * rotation * scaleMatrix;
}