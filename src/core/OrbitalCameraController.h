#pragma once

#include <GLFW/glfw3.h>

#include "InputRouter.h"
#include "../rendering/Camera.h"

class OrbitalCameraController : public IMouseListener {
public:
	explicit OrbitalCameraController(Camera& cam);

	void OnMouseButton(GLFWwindow*, int button, int action, int mods) override;
	void OnCursorPos(GLFWwindow*, double x, double y) override;
	void OnScroll(GLFWwindow*, double xoff, double yoff) override;

private:
	Camera& camera;
	bool firstMouse = true;
	float lastX = 0, lastY = 0;
	enum class Mode { None, Orbit, Pan, Dolly } mode = Mode::None;

	float orbitSensitivity = 1.0f;
	float panSensitivity = 1.0f;
	float dollySensitivity = 1.0f;
	float zoomSensitivity = 1.0f;
};
