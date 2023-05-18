#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

struct Light
{
    std::string mName;
    glm::vec3 mPosition;
    float near = 0.1f;
    float far = 80.0f;
    glm::vec3 mDirection;
    glm::vec3 mColorAmbient;
    glm::vec3 mColorDiffuse;
    glm::vec3 mColorSpecular;

    Light() = default;
    
    Light(
        glm::vec3 position,
        float near = 0.1f,
        float far = 80.0f,
        glm::vec3 direction = {0, 0, 0},
        glm::vec3 colorAmbient = {0, 0, 0},
        glm::vec3 colorDiffuse = {0, 0, 0},
        glm::vec3 colorSpecular = {0, 0, 0});

    glm::mat4 getProjection();
    glm::mat4 getView();
    glm::mat4 getSpaceMatrix();
};

#endif