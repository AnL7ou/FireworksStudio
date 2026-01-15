#pragma once

#include "src/core/InputRouter.h"

class UIManager;
class TemplateLibrary;

// Rotate the active firework template directly from the 3D view (Template mode).
//
// Controls:
// - LMB drag: free rotation (Yaw with mouse X, Pitch with mouse Y)
// - Shift + LMB drag: Pitch (X) only
// - Ctrl  + LMB drag: Yaw (Y) only
// - Shift + Ctrl + LMB drag: Roll (Z) only
//
// Notes:
// - Only active in Template mode.
// - Ignores input when ImGui is capturing the mouse.
// - Alt is reserved for the orbital camera controller.
class TemplateRotationController final : public IMouseListener {
public:
    TemplateRotationController(UIManager& ui, TemplateLibrary& library);

    void OnMouseButton(GLFWwindow* window, int button, int action, int mods) override;
    void OnCursorPos(GLFWwindow* window, double x, double y) override;

private:
    UIManager& ui;
    TemplateLibrary& library;

    bool rotating = false;
    float lastX = 0.0f;
    float lastY = 0.0f;

    float sensitivityDegPerPixel = 0.25f;

    void applyDelta(GLFWwindow* window, float dx, float dy);
};
