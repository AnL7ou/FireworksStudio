#pragma once

#include <glengine/orbitalCamera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera : public GLEngine::OrbitalCamera {
public:
    Camera(glm::vec3 pos, glm::vec3 _focus, glm::vec3 _up)
        : GLEngine::OrbitalCamera(pos, _focus, _up)
        , nearPlane(0.1f)
        , farPlane(100.0f)
    {
    }

    glm::mat4 getViewMatrix() const
    {
        return GLEngine::OrbitalCamera::getViewMatrix();
    }

    glm::mat4 getProjectionMatrix(float aspect) const
    {
        return glm::perspective(glm::radians(getFov()), aspect, nearPlane, farPlane);
    }

    glm::vec3 getPosition() const
    {
        return GLEngine::OrbitalCamera::getPosition();
    }

protected:
    float nearPlane;
    float farPlane;
};
