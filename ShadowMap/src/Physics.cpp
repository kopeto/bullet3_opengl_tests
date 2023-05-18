#include "Physics.hpp"

#include <unordered_map>

#define MAX(X, Y) ((X > Y) ? X : Y)
#define MIN(X, Y) ((X < Y) ? X : Y)

GE::Physics::Physics()
{
    broadphase = new btDbvtBroadphase();
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));
}

void GE::Physics::step(float dt)
{
    ++frame;
    dynamicsWorld->stepSimulation(dt);
}

void GE::Physics::updateBodies()
{
    btAlignedObjectArray<btRigidBody *> rigidBodies = dynamicsWorld->getNonStaticRigidBodies();
    for (int i = 0; i < rigidBodies.size(); ++i)
    {
        if (rigidBodies[i]->getWorldTransform().getOrigin().getY() < -100.0f)
        {
            btRigidBody *rb = rigidBodies[i];
            Entity *entA = static_cast<Entity *>(rb->getUserPointer());
            dynamicsWorld->removeRigidBody(rigidBodies[i]);
            entA->model = nullptr;
            // delete rb;
            // rb = nullptr;
        }
    }
}

void GE::Physics::Collision()
{
    btDispatcher *dp = dynamicsWorld->getDispatcher();
    const int numManifolds = dp->getNumManifolds();
    for (int m = 0; m < numManifolds; ++m)
    {
        btPersistentManifold *man = dp->getManifoldByIndexInternal(m);
        const btRigidBody *obA = static_cast<const btRigidBody *>(man->getBody0());
        const btRigidBody *obB = static_cast<const btRigidBody *>(man->getBody1());
        const Entity *entA = static_cast<const Entity *>(obA->getUserPointer());
        const Entity *entB = static_cast<const Entity *>(obB->getUserPointer());
        // use user pointers to determine if objects are eligible for destruction.
        // ...
        const int numc = man->getNumContacts();
        float totalImpact = 0.0f;
        float threshold = 1000.0f;
        for (int c = 0; c < numc; ++c)
            totalImpact += man->getContactPoint(c).m_appliedImpulse;
        if (totalImpact > threshold)
        {
            printf("[%4u][%u - %u] Power: %.3f \n", frame, entA->m_id, entB->m_id, totalImpact);
        }
    }
}

void GE::Physics::addRigidBOX(Entity &entity, const glm::vec3 pos, const glm::vec3 sizes, const glm::vec3 velocity, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags)
{
    btCollisionShape *shape = new btBoxShape(btVector3(sizes.x, sizes.y, sizes.z));
    btMatrix3x3 initRotation(rotation[0][0], rotation[0][1], rotation[0][2],
                             rotation[1][0], rotation[1][1], rotation[1][2],
                             rotation[2][0], rotation[2][1], rotation[2][2]);

    btTransform initTransform(initRotation, btVector3(pos.x, pos.y, pos.z));
    btDefaultMotionState *MotionState = new btDefaultMotionState(initTransform);
    float mass;
    if (!(flags & btCollisionObject::CF_STATIC_OBJECT))
        mass = sizes.x * sizes.y * sizes.z;
    else
        mass = 0.0f;
    btVector3 Inertia(0, 0, 0);
    shape->calculateLocalInertia(mass, Inertia);
    btRigidBody::btRigidBodyConstructionInfo RigicBodyCI(mass, MotionState, shape, Inertia);

    entity.body = new btRigidBody(RigicBodyCI);
    entity.body->setRestitution(.30f);
    entity.body->setFriction(2.0f);
    entity.body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
    entity.body->setCcdMotionThreshold(1e-7);
    entity.body->setCcdSweptSphereRadius(0.50);
    entity.body->setCollisionFlags(flags);
    entity.body->setUserPointer(&entity);
    dynamicsWorld->addRigidBody(entity.body);
}

void GE::Physics::addSphereBOX(Entity &entity, const glm::vec3 pos, const float radius, const glm::vec3 velocity, btCollisionObject::CollisionFlags flags)
{
    btCollisionShape *sphereShape = new btSphereShape(radius);

    btTransform initTransform(btQuaternion(0, 0, 0, 1), btVector3(pos.x, pos.y, pos.z));
    btDefaultMotionState *sphereMotionState = new btDefaultMotionState(initTransform);

    float density = 1.0f;
    btScalar mass = density * 4.0 / 3.0 * glm::pi<float>() * radius * radius;

    btVector3 sphereInertia(10.0, 10.0, 10.0);
    sphereShape->calculateLocalInertia(mass, sphereInertia);
    btRigidBody::btRigidBodyConstructionInfo sphereRigicBodyCI(mass, sphereMotionState, sphereShape, sphereInertia);

    entity.body = new btRigidBody(sphereRigicBodyCI);
    entity.body->setRestitution(0.80f);
    entity.body->setFriction(1.0f);
    entity.body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
    entity.body->setCollisionFlags(flags);
    entity.body->setUserPointer(&entity);
    entity.body->setCcdMotionThreshold(1e-7);
    entity.body->setCcdSweptSphereRadius(0.50);
    dynamicsWorld->addRigidBody(entity.body);
}

void GE::Physics::add2DBOX(Entity &entity, const glm::vec3 pos, const glm::vec2 dimensions, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags)
{
    btBox2dShape *shape = new btBox2dShape(btVector3(dimensions.x, 0.0f, dimensions.y));
    btMatrix3x3 initRotation(rotation[0][0], rotation[0][1], rotation[0][2],
                             rotation[1][0], rotation[1][1], rotation[1][2],
                             rotation[2][0], rotation[2][1], rotation[2][2]);

    btTransform initTransform(initRotation, btVector3(pos.x, pos.y, pos.z));
    btDefaultMotionState *MotionState = new btDefaultMotionState(initTransform);

    float mass;
    if (!(flags & btCollisionObject::CF_STATIC_OBJECT))
        mass = dimensions.x * dimensions.y;
    else
        mass = 0.0f;

    btVector3 inertia(10.0, 10.0, 10.0);
    shape->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo RigicBodyCI(mass, MotionState, shape, inertia);
    entity.body = new btRigidBody(RigicBodyCI);
    entity.body->setRestitution(0.40f);
    entity.body->setFriction(1.0f);
    entity.body->setUserPointer(&entity);
    entity.body->setCollisionFlags(flags);

    dynamicsWorld->addRigidBody(entity.body);
}

void GE::Physics::addRigidBoxFromModel(Entity &entity, const Model *model, const glm::vec3 pos, const glm::vec3 velocity, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags)
{
    // SHAPE
    static std::unordered_map<const Model *, btConvexHullShape *> models_parsed;
    float max_x = -99999.0;
    float min_x = 99999.0;
    float max_y = -99999.0;
    float min_y = 99999.0;
    float max_z = -99999.0;
    float min_z = 99999.0;

    if (models_parsed.find(model) == models_parsed.end())
    {
        float *vertex_positions;
        unsigned int hitbox_vertices = model->GetRawPositions(&vertex_positions);
        btConvexHullShape *new_shape = new btConvexHullShape();
        new_shape->optimizeConvexHull();

        for (unsigned int i = 0; i < hitbox_vertices; i += 3)
        {
            new_shape->addPoint(btVector3(vertex_positions[i], vertex_positions[i + 1], vertex_positions[i + 2]));
            max_x = MAX(vertex_positions[i], max_x);
            min_x = MIN(vertex_positions[i], min_x);
            max_y = MAX(vertex_positions[i + 1], max_y);
            min_y = MIN(vertex_positions[i + 1], min_y);
            max_z = MAX(vertex_positions[i + 2], max_z);
            min_z = MIN(vertex_positions[i + 2], min_z);
        }

        models_parsed.emplace(model, new_shape);
    }

    btConvexHullShape *shape = models_parsed.at(model);

    btMatrix3x3 initRotation(rotation[0][0], rotation[0][1], rotation[0][2],
                             rotation[1][0], rotation[1][1], rotation[1][2],
                             rotation[2][0], rotation[2][1], rotation[2][2]);
    btTransform initTransform(initRotation, btVector3(pos.x, pos.y, pos.z));
    btDefaultMotionState *MotionState = new btDefaultMotionState(initTransform);

    float mass;
    if (!(flags & btCollisionObject::CF_STATIC_OBJECT))
    {
        btVector3 c;
        float r;
        shape->getBoundingSphere(c, r);
        float volume = 4.0f / 3.0f * 3.14f * r * r;
        float density = 1.0;
        mass = volume * density;
    }
    else
        mass = 0.0f;

    btVector3 Inertia(1, 1, 1);
    shape->calculateLocalInertia(mass, Inertia);

    btRigidBody::btRigidBodyConstructionInfo RigicBodyCI(mass, MotionState, shape, Inertia);

    entity.body = new btRigidBody(RigicBodyCI);
    entity.body->setRestitution(0.80f);
    entity.body->setFriction(0.9f);
    entity.body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
    entity.body->setUserPointer(&entity);
    entity.body->setCcdMotionThreshold(1e-7);
    entity.body->setCcdSweptSphereRadius(0.50);
    entity.body->setCollisionFlags(flags);
    dynamicsWorld->addRigidBody(entity.body);
}

void GE::Physics::addRigidBoxFromModel(Entity &entity, std::string model_name, const float *points, int n_points, const glm::vec3 pos, const glm::vec3 velocity, const glm::mat4 rotation, btCollisionObject::CollisionFlags flags)
{
    static std::unordered_map<std::string, btConvexHullShape *> models_parsed;
    float max_x = -99999.0;
    float min_x = 99999.0;
    float max_y = -99999.0;
    float min_y = 99999.0;
    float max_z = -99999.0;
    float min_z = 99999.0;

    if (models_parsed.find(model_name) == models_parsed.end())
    {
        btConvexHullShape *new_shape = new btConvexHullShape();

        for (unsigned int i = 0; i < n_points; i += 3)
        {
            new_shape->addPoint(btVector3(points[i], points[i + 1], points[i + 2]));
            max_x = MAX(points[i], max_x);
            min_x = MIN(points[i], min_x);
            max_y = MAX(points[i + 1], max_y);
            min_y = MIN(points[i + 1], min_y);
            max_z = MAX(points[i + 2], max_z);
            min_z = MIN(points[i + 2], min_z);
        }
        new_shape->optimizeConvexHull();

        models_parsed.emplace(model_name, new_shape);
    }

    btConvexHullShape* shape = models_parsed.at(model_name);

    btMatrix3x3 initRotation(rotation[0][0], rotation[0][1], rotation[0][2],
                             rotation[1][0], rotation[1][1], rotation[1][2],
                             rotation[2][0], rotation[2][1], rotation[2][2]);
    btTransform initTransform(initRotation, btVector3(pos.x, pos.y, pos.z));
    btDefaultMotionState *MotionState = new btDefaultMotionState(initTransform);

    float mass;
    if (!(flags & btCollisionObject::CF_STATIC_OBJECT))
    {
        btVector3 c;
        float r;
        shape->getBoundingSphere(c, r);
        float volume = 4.0f / 3.0f * 3.14f * r * r;
        float density = 1.0;
        mass = volume * density;
    }
    else
        mass = 0.0f;

    btVector3 Inertia(1, 1, 1);
    shape->calculateLocalInertia(mass, Inertia);

    btRigidBody::btRigidBodyConstructionInfo RigicBodyCI(mass, MotionState, shape, Inertia);

    entity.body = new btRigidBody(RigicBodyCI);
    entity.body->setRestitution(0.80f);
    entity.body->setFriction(0.9f);
    entity.body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
    entity.body->setUserPointer(&entity);
    entity.body->setCcdMotionThreshold(1e-7);
    entity.body->setCcdSweptSphereRadius(0.50);
    entity.body->setCollisionFlags(flags);
    dynamicsWorld->addRigidBody(entity.body);
}
