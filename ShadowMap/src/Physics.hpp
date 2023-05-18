#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <glm/glm.hpp>

#include "Model.hpp"

#include "EntityManager.hpp"

namespace GE
{
    struct Physics
    {
        Physics();

        btBroadphaseInterface *broadphase;
        btDefaultCollisionConfiguration *collisionConfiguration;
        btCollisionDispatcher *dispatcher;
        btSequentialImpulseConstraintSolver *solver;
        btDiscreteDynamicsWorld *dynamicsWorld;
        unsigned int frame = 0;

        void step(float deltaTime);
        void Collision();
        void updateBodies();

        void addRigidBOX( Entity &entity, const glm::vec3 pos, const glm::vec3 sizes, const glm::vec3 velocity, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags);
        void addSphereBOX( Entity &entity, const glm::vec3 pos, const float radius, const glm::vec3 velocity, btCollisionObject::CollisionFlags flags);
        void add2DBOX( Entity &entity, const glm::vec3 pos, const glm::vec2 dimensions, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags);
        void addRigidBoxFromModel( Entity &entity, const Model *model, glm::vec3 pos, const glm::vec3 velocity, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags);
        void addRigidBoxFromModel( Entity &entity, std::string model_name, const float* points, int n_points, const glm::vec3 pos, const glm::vec3 velocity, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags);

    };

} // namespace GE

#endif