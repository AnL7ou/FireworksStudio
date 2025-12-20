#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

Camera::Camera()
	: position(0.0f, 0.0f, 3.0f)
	, front(0.0f, 0.0f, -1.0f)
	, up(0.0f, 1.0f, 0.0f)
	, nearPlane(0.1f)
	, farPlane(100.0f)
	, fov(45.0f)
{
}

Camera::~Camera()
{
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect) const
{
	if (aspect <= 0.0f)
		aspect = 1.0f;

	return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

glm::vec3 Camera::getPosition() const { return position; }
