#include "Render.hpp"

// CONSTRUCTOR
GE::Render::Render(EntityManager &entMan, int _w, int _h) : src_W{_w}, src_H{_h}, entityManager{entMan} {}
//

void GE::Render::RenderScene(Shader *shader, Camera &camera, Light &light) const
{
    for (const auto e : entityManager.Entities)
    {
        DrawEntity(e, shader, camera, light);
    }
}

void GE::Render::DrawEntity(Entity *e, Shader *shader, Camera &camera, Light &light) const
{
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = camera.GetProjectionMatrix(src_W, src_H);

    // TODO: Draw MODELS WITH NO PHYSICALS
    if (e /* && e->body */ && e->model)
    {
        static Shader* redShader = new Shader("shaders/vertexshader.vs", "shaders/redColorFragmentShader.fs");
        e->updateModelTransform();
        if(e->selected)
            shader =  redShader;

        shader->use();
        shader->setInt  ("objectId", e->m_id);
        shader->setInt  ("drawId", e->selected? 5353 : 3535);
        shader->setVec2("resolution", glm::vec2(src_W, src_H));
        shader->setVec3("lightPos", light.mPosition);
        shader->setVec3("viewPos", camera.Position);
        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setMat4("model", e->getModelTransformationMatrix());
        shader->setMat4("lightSpaceMatrix", light.getSpaceMatrix());

        e->model->Draw(*shader);
    }
}