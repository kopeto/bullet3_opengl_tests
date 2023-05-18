#include "Light.hpp"

Light::Light(
    glm::vec3 position,
    float _near,
    float _far,
    glm::vec3 direction,
    glm::vec3 colorAmbient,
    glm::vec3 colorDiffuse,
    glm::vec3 colorSpecular)
    : mPosition{position},
      near{_near},
      far{_far},
      mDirection{direction},
      mColorAmbient{colorAmbient},
      mColorDiffuse{colorDiffuse},
      mColorSpecular{colorSpecular}
{
}

glm::mat4 Light::getProjection()
{
    float near = 0.1f;
    float far = 80.0f;

    return glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near, far);
}

glm::mat4 Light::getView()
{
    return glm::lookAt(mPosition, mDirection, {0.0f, 1.0f, 0.0f});
}

glm::mat4 Light::getSpaceMatrix()
{
    return getProjection() * getView();
}
