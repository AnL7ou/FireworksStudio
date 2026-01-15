#pragma once

#include <glm/glm.hpp>

#include "src/core/InputRouter.h"

class Camera;
class Scene;
class UIManager;

// Handles selection + placement of firework events directly in the 3D view.
// Policy:
// - Only active in Scene mode.
// - LMB click near an event marker selects it.
// - LMB drag moves the selected event on the ground plane (y=0).
// - Ignores input when ImGui is capturing the mouse (WantCaptureMouse) and when Alt is held
//   (reserved for camera orbit).
class ScenePlacementController : public IMouseListener {
public:
    ScenePlacementController(Camera& camera, UIManager& ui, Scene& scene);

    void OnMouseButton(GLFWwindow* window, int button, int action, int mods) override;
    void OnCursorPos(GLFWwindow* window, double x, double y) override;

private:
    Camera& camera;
    UIManager& ui;
    Scene& scene;

    bool dragging = false;
    int draggedEvent = -1;

    int pickEvent(GLFWwindow* window, double mx, double my) const;
    bool raycastToGround(GLFWwindow* window, double mx, double my, glm::vec3& outPos) const;
    void setSelected(int idx) const;
};
