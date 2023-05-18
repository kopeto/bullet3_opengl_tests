#ifndef RENDER_HPP
#define RENDER_HPP

#include <glm/glm.hpp>

#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Skybox.hpp"
#include "Framebuffer.hpp"
#include "Physics.hpp"
#include "Light.hpp"
#include "EntityManager.hpp"
#include "Entity.hpp"

namespace GE
{

    struct Render
    {
        Render() = delete;
        Render(EntityManager &entMan, int _w, int _h);

        void RenderScene(Shader *shader, Camera &camera, Light &light) const;

    private:
        int src_W, src_H;
        EntityManager &entityManager;

        void DrawEntity(Entity *e, Shader *shader, Camera &camera, Light &light) const;
    };
} // namespace GE

#endif