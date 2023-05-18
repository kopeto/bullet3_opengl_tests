#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.hpp"

namespace GE
{

    template <class EntityType, class Entity>
    concept Derived = std::is_base_of_v<Entity, EntityType>;

    struct Entity
    {
        Entity() { m_id = id++; };
        virtual ~Entity()
        {
            printf("Entity ID:%u deleted\n", m_id);
            // model = nullptr;
            // if(body)
            //     dynamicsWorld->removeRigidBody(body);
            // delete body->getMotionState();
            // delete body->getCollisionShape();
            // Entity *entA = static_cast<Entity *>(rb->getUserPointer());
        }

        static inline unsigned int id{0};
        unsigned int m_id;
        const Model *model = nullptr;
        btRigidBody *body = nullptr;

        glm::vec3 position;
        glm::mat4 rotation;

        virtual void        updateModelTransform() = 0;
        virtual glm::mat4   getModelTransformationMatrix() const = 0;

        bool selected = false;
    };

    struct EntityManager
    {
        std::vector<Entity *> Entities;
        std::vector<Model *> Models;

        Entity* selected = nullptr;

        EntityManager(size_t size = 100);
        ~EntityManager();

        void updateEntities();
        void removeEntity(Entity *e);

        const Model *createModel(Model *model);

        template <Derived<Entity> EntityType, typename... Ts>
        EntityType &createEntity(const Model *model, Ts... vars)
        {
            EntityType *e = new EntityType(vars...);
            e->model = model;
            Entities.push_back(e);
            return *e;
        }
    };
} // namespace GE

#endif