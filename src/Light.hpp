#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include <string>

struct Light
{
    std::string mName;
    glm::vec3 mPosition;
    glm::vec3 mDirection;
    glm::vec3 mColorAmbient;
    glm::vec3 mColorDiffuse;
    glm::vec3 mColorSpecular;

    Light() = default;
    Light(
        std::string name,
        glm::vec3 position,
        glm::vec3 direction,
        glm::vec3 colorAmbient,
        glm::vec3 colorDiffuse,
        glm::vec3 colorSpecular);
};

#endif