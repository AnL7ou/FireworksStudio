#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/fwd.hpp>

class Camera {
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float nearPlane;
    float farPlane;
    float fov;

public:
    Camera();
    ~Camera();
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;
    glm::vec3 getPosition() const;
};