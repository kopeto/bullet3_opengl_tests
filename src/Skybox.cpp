#include "Skybox.hpp"

#include <glm/glm.hpp>


std::vector<std::string> default_skybox_faces =
    {
        "textures/skybox_right.jpeg",
        "textures/skybox_left.jpeg",
        "textures/skybox_top.jpeg",
        "textures/skybox_bottom.jpeg",
        "textures/skybox_front.jpeg",
        "textures/skybox_back.jpeg"};


Skybox::Skybox() 
    : texture{default_skybox_faces}, skyboxShader{"shaders/skybox.vs", "shaders/skybox.fs"}
{
    config();
}


Skybox::Skybox(std::vector<std::string> faces)
    : texture{faces}, skyboxShader{"shaders/skybox.vs", "shaders/skybox.fs"}
{
    config();
};

void Skybox::config()
{
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
    glBindVertexArray(0);
}

void Skybox::Draw(Camera &camera)
{
    extern const unsigned int WIDTH;
    extern const unsigned int HEIGHT;

    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    glm::mat4 projection = camera.GetProjectionMatrix(WIDTH, HEIGHT);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setMat4("view", view);

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    // draw SKYBOX
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(cubeIndices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}
