#include "Light.hpp"

Light::Light(
    std::string name,
    glm::vec3 position,
    glm::vec3 direction,
    glm::vec3 colorAmbient,
    glm::vec3 colorDiffuse,
    glm::vec3 colorSpecular)
    :   mName{name}, 
        mPosition{position},
        mColorAmbient{colorAmbient},
        mColorDiffuse{colorDiffuse},
        mColorSpecular{colorSpecular}
{}