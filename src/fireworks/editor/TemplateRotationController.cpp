#include "TemplateRotationController.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "src/ui/UIManager.h"
#include "src/ui/EditorMode.h"
#include "src/fireworks/template/TemplateLibrary.h"
#include "src/fireworks/template/FireworkTemplate.h"

TemplateRotationController::TemplateRotationController(UIManager& ui_, TemplateLibrary& library_)
    : ui(ui_), library(library_)
{
}

void TemplateRotationController::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    // Only active in Template mode.
    if (ui.GetMode() != EditorMode::Template)
        return;

    // Let ImGui consume the mouse.
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    // Alt is reserved for camera orbit.
    if ((mods & GLFW_MOD_ALT) != 0)
        return;

    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_PRESS)
    {
        FireworkTemplate* t = library.GetActive();
        if (!t) return;

        rotating = true;

        double mx = 0.0, my = 0.0;
        glfwGetCursorPos(window, &mx, &my);
        lastX = static_cast<float>(mx);
        lastY = static_cast<float>(my);

    }
    else if (action == GLFW_RELEASE)
    {
        rotating = false;
    }
}

void TemplateRotationController::OnCursorPos(GLFWwindow* window, double x, double y)
{
    if (!rotating)
        return;

    // If the UI starts capturing mid-drag (rare but possible), stop.
    if (ImGui::GetIO().WantCaptureMouse)
    {
        rotating = false;
        return;
    }

    const float mx = static_cast<float>(x);
    const float my = static_cast<float>(y);
    const float dx = mx - lastX;
    const float dy = my - lastY;

    lastX = mx;
    lastY = my;

    applyDelta(window, dx, dy);
}

void TemplateRotationController::applyDelta(GLFWwindow* window, float dx, float dy)
{
    FireworkTemplate* t = library.GetActive();
    if (!t) return;

    const bool shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    const bool ctrl  = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);

    // Axis locks via modifiers:
    // - Shift only  : X (pitch)
    // - Ctrl only   : Y (yaw)
    // - Shift+Ctrl  : Z (roll)
    // - None        : free (pitch+yaw)
    const float s = sensitivityDegPerPixel;

    // Apply increments (not absolute), so the interaction feels continuous.
    if (shift && ctrl)
    {
        t->worldRotation.z += dx * s;
    }
    else if (shift)
    {
        t->worldRotation.x += (-dy) * s;
    }
    else if (ctrl)
    {
        t->worldRotation.y += dx * s;
    }
    else
    {
        t->worldRotation.y += dx * s;
        t->worldRotation.x += (-dy) * s;
    }

    // Keep values bounded for nicer sliders / serialization.
    auto wrap = [](float a) {
        while (a > 180.0f) a -= 360.0f;
        while (a < -180.0f) a += 360.0f;
        return a;
    };
    t->worldRotation.x = wrap(t->worldRotation.x);
    t->worldRotation.y = wrap(t->worldRotation.y);
    t->worldRotation.z = wrap(t->worldRotation.z);

    // Ensure branches regenerate so the viewport updates immediately.
    t->RegenerateBranches();
}
