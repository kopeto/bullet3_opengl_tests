#include "Physics.hpp"

#define MAX(X, Y) ((X > Y) ? X : Y)
#define MIN(X, Y) ((X < Y) ? X : Y)

Physics::Physics()
{
    broadphase              = new btDbvtBroadphase();
    collisionConfiguration  = new btDefaultCollisionConfiguration();
    dispatcher              = new btCollisionDispatcher(collisionConfiguration);
    solver                  = new btSequentialImpulseConstraintSolver();
    dynamicsWorld           = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    
    dynamicsWorld->setGravity(btVector3(0, -100, 0));
}

btRigidBody* Physics::addRigidBox(glm::vec3 pos, glm::vec3 sizes, btCollisionObject::CollisionFlags flags)
{
    btCollisionShape *shape            = new btBoxShape(btVector3(sizes.x, sizes.y, sizes.z));
    btDefaultMotionState *MotionState  = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z)));
    float mass;
    if( ! ( flags & btCollisionObject::CF_STATIC_OBJECT ) )
        mass = sizes.x * sizes.y * sizes.z;
    else
        mass = 0.0f;
    btVector3 Inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, Inertia);
    btRigidBody::btRigidBodyConstructionInfo RigicBodyCI(mass, MotionState, shape, Inertia);
    btRigidBody *body = new btRigidBody(RigicBodyCI);
    body->setRestitution(.30f);
    body->setFriction(2.0f);
    body->setCollisionFlags(flags);
    dynamicsWorld->addRigidBody(body);

    return body;
}

btRigidBody* Physics::addSphere(glm::vec3 pos, float radius,glm::vec3 velocity, btCollisionObject::CollisionFlags flags)
{
    btCollisionShape *sphereShape = new btSphereShape(radius);
    btTransform initTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z));
    btDefaultMotionState *sphereMotionState = new btDefaultMotionState(initTransform);

    float density = 5.0f;
    btScalar mass = density * 4.0/3.0 * glm::pi<float>() * radius * radius;

    btVector3 sphereInertia(10.0, 10.0, 10.0);
    sphereShape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigicBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);
    btRigidBody* body = new btRigidBody(sphereRigicBodyCI);
    body->setRestitution(0.80f);
    body->setFriction(12.0f);
    body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
    dynamicsWorld->addRigidBody(body);
    return body;
}

btRigidBody* Physics::addRigidBoxFromModel(Model& model, glm::vec3 pos, btCollisionObject::CollisionFlags flags)
{

    // SHAPE
    float *vertex_positions;
    unsigned int hitbox_vertices = model.GetRawPositions(&vertex_positions);
    btConvexHullShape *shape = new btConvexHullShape();
    float max_x=-99999.0;
    float min_x=99999.0;
    float max_y=-99999.0;
    float min_y=99999.0;
    float max_z=-99999.0;
    float min_z=99999.0;
    
    for (unsigned int i = 0; i < hitbox_vertices; i += 3)
    {
        shape->addPoint(btVector3(vertex_positions[i], vertex_positions[i + 1], vertex_positions[i + 2]));
        max_x = MAX(vertex_positions[i], max_x);
        min_x = MIN(vertex_positions[i], min_x);
        max_y = MAX(vertex_positions[i+1], max_y);
        min_y = MIN(vertex_positions[i+1], min_y);
        max_z = MAX(vertex_positions[i+2], max_z);
        min_z = MIN(vertex_positions[i+2], min_z);
    }

    btDefaultMotionState *MotionState  = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z)));
    float mass;
    if( ! ( flags & btCollisionObject::CF_STATIC_OBJECT ) )
        mass = (max_x-min_y) * (max_y-min_y) * (max_z-min_z);
    else
        mass = 0.0f;
    btVector3 Inertia(1, 1, 1);
    shape->calculateLocalInertia(mass, Inertia);
    btRigidBody::btRigidBodyConstructionInfo RigicBodyCI(mass, MotionState, shape, Inertia);
    btRigidBody *body = new btRigidBody(RigicBodyCI);
    body->setRestitution(0.80f);
    body->setFriction(0.9f);
    body->setCollisionFlags(flags);
    dynamicsWorld->addRigidBody(body);

    return body;
}
