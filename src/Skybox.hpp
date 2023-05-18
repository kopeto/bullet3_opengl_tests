#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <GL/glew.h>

#include "Texture.hpp"
#include "Shader.hpp"
#include "Camera.hpp"


struct Skybox
{
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    Texture texture;
    Shader skyboxShader;

    float cubeVertices[24] =
        {
            -1.0,
            -1.0,
            -1.0,
            1.0,
            -1.0,
            -1.0,
            -1.0,
            1.0,
            -1.0,
            1.0,
            1.0,
            -1.0,
            -1.0,
            -1.0,
            1.0,
            1.0,
            -1.0,
            1.0,
            -1.0,
            1.0,
            1.0,
            1.0,
            1.0,
            1.0,
    };

    unsigned int cubeIndices[36] =
        {
            0, 2, 1,
            1, 2, 3, // bottom
            4, 5, 6,
            5, 7, 6, // top
            4, 0, 5,
            5, 0, 1, // front
            6, 7, 2,
            7, 3, 2, // back
            6, 2, 0,
            4, 6, 0, // left
            7, 5, 1,
            7, 1, 3 // right
    };
    Skybox();
    Skybox(std::vector<std::string> faces );

    void config();
    void Draw(Camera& camera);
};

#endif