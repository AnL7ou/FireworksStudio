#include "Camera.h"

#include <cmath>
#include <utility>

static constexpr float DEFAULT_FOV = 45.0f;

float Camera::clampf(float v, float lo, float hi)
{
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

Camera::Camera()
    : focus(0.0f, 0.0f, 2.0f)
    , worldUp(0.0f, 1.0f, 0.0f)
    , yawDeg(-90.0f)
    , pitchDeg(0.0f)
    , distance(1.0f)
    , fovDeg(DEFAULT_FOV)
    , nearPlane(0.1f)
    , farPlane(100.0f)
    , minPitchDeg(-89.0f)
    , maxPitchDeg(89.0f)
    , minDistance(0.05f)
    , maxDistance(500.0f)
{
    updateVectors();
}

Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 upVec)
    : focus(target)
    , worldUp(glm::normalize(upVec))
    , fovDeg(DEFAULT_FOV)
    , nearPlane(0.1f)
    , farPlane(100.0f)
    , minPitchDeg(-89.0f)
    , maxPitchDeg(89.0f)
    , minDistance(0.05f)
    , maxDistance(500.0f)
{
    glm::vec3 dir = glm::normalize(target - pos);
    pitchDeg = glm::degrees(std::asin(clampf(dir.y, -1.0f, 1.0f)));
    yawDeg = glm::degrees(std::atan2(dir.z, dir.x));

    distance = clampf(glm::length(pos - target), minDistance, maxDistance);
    updateVectors();
}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, focus, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const
{
    if (aspect <= 0.0f) aspect = 1.0f;
    return glm::perspective(glm::radians(fovDeg), aspect, nearPlane, farPlane);
}

glm::vec3 Camera::getPosition() const { return position; }
glm::vec3 Camera::getFocus() const { return focus; }
float Camera::getFov() const { return fovDeg; }

void Camera::setClipPlanes(float nearP, float farP)
{
    nearPlane = nearP;
    farPlane = farP;
}

void Camera::setPitchLimits(float minDeg, float maxDeg)
{
    if (minDeg > maxDeg) std::swap(minDeg, maxDeg);
    minPitchDeg = minDeg;
    maxPitchDeg = maxDeg;
    pitchDeg = clampf(pitchDeg, minPitchDeg, maxPitchDeg);
    updateVectors();
}

void Camera::setDistanceLimits(float minD, float maxD)
{
    if (minD > maxD) std::swap(minD, maxD);
    minDistance = minD;
    maxDistance = maxD;
    distance = clampf(distance, minDistance, maxDistance);
    updateVectors();
}

void Camera::orbit(float dx, float dy)
{
    yawDeg += dx;
    pitchDeg += dy;
    pitchDeg = clampf(pitchDeg, minPitchDeg, maxPitchDeg);
    updateVectors();
}

void Camera::pan(float dx, float dy)
{
    glm::vec3 t = right * dx + up * dy;
    focus += t;
    position += t;
}

void Camera::dolly(float delta)
{
    distance = clampf(distance + delta, minDistance, maxDistance);
    updateVectors();
}

void Camera::zoom(float deltaDeg)
{
    fovDeg = clampf(fovDeg + deltaDeg, 1.0f, 120.0f);
}

void Camera::track(float offset)
{
    // Move parallel to view plane along camera right
    glm::vec3 t = right * offset;
    focus += t;
    position += t;
}

void Camera::pedestal(float offset)
{
    // Move parallel to view plane along camera up
    glm::vec3 t = up * offset;
    focus += t;
    position += t;
}

void Camera::updateVectors()
{
    const float yaw = glm::radians(yawDeg);
    const float pitch = glm::radians(pitchDeg);

    forward = glm::normalize(glm::vec3(
        std::cos(yaw) * std::cos(pitch),
        std::sin(pitch),
        std::sin(yaw) * std::cos(pitch)
    ));

    right = glm::normalize(glm::cross(forward, worldUp));
    up = glm::normalize(glm::cross(right, forward));

    position = focus - forward * distance;
}
