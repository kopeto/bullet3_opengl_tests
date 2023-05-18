#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "Model.hpp"

struct Physics
{
    Physics();

    btBroadphaseInterface               *broadphase;
    btDefaultCollisionConfiguration     *collisionConfiguration;
    btCollisionDispatcher               *dispatcher;
    btSequentialImpulseConstraintSolver *solver;
    btDiscreteDynamicsWorld             *dynamicsWorld;

    btRigidBody* addRigidBox( glm::vec3 pos, glm::vec3 sizes, btCollisionObject::CollisionFlags flags );
    btRigidBody* addRigidBoxFromModel( Model& model, glm::vec3 pos, btCollisionObject::CollisionFlags flags );
    btRigidBody* addSphere( glm::vec3 pos, float radius, glm::vec3 velocity, btCollisionObject::CollisionFlags flags );
};

#endif