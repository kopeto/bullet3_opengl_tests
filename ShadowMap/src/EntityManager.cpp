#include "EntityManager.hpp"


GE::EntityManager::EntityManager(size_t size)
{
    Entities.reserve(size);
}

GE::EntityManager::~EntityManager()
{
    for (Entity *e : Entities)
    {
        delete e;
    }
}

void GE::EntityManager::updateEntities()
{
    for(Entity* e: Entities)
    {
        if(e->position.y < -100.0f)
        {
            e->model = nullptr;
        }
    }
}

void GE::EntityManager::removeEntity(Entity* e)
{
    e->model = nullptr;
    e->body = nullptr;
}

const Model *GE::EntityManager::createModel(Model *model)
{
    return Models.emplace_back(model);
}
