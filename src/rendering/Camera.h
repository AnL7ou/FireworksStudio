#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));

    // Rendering API
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspect) const;

    glm::vec3 getPosition() const;
    glm::vec3 getFocus() const;
    float getFov() const;

    void setClipPlanes(float nearP, float farP);
    void setPitchLimits(float minDeg, float maxDeg);
    void setDistanceLimits(float minD, float maxD);

    // Controls (controller provides scaling)
    void orbit(float dx, float dy);
    void pan(float dx, float dy);
    void dolly(float delta);
    void zoom(float deltaDeg);

    // Controller compatibility (vendor-style naming)
    void track(float offset);
    void pedestal(float offset);

private:
    // Core state
    glm::vec3 focus;
    glm::vec3 position;
    glm::vec3 worldUp;

    // Orientation (degrees)
    float yawDeg;
    float pitchDeg;

    // Distance to focus
    float distance;

    // Optics
    float fovDeg;
    float nearPlane;
    float farPlane;

    // Limits
    float minPitchDeg;
    float maxPitchDeg;
    float minDistance;
    float maxDistance;

    // Derived basis
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    void updateVectors();
    static float clampf(float v, float lo, float hi);
};
