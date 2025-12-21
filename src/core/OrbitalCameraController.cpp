#include "OrbitalCameraController.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

OrbitalCameraController::OrbitalCameraController(Camera& cam)
    : camera(cam)
{
}

void OrbitalCameraController::OnMouseButton(GLFWwindow* /*window*/, int button, int action, int mods)
{
    // If the UI is actively using the mouse, do not start/stop camera drags.
    // This keeps the UI usable and prevents “camera fights”.
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    if (action == GLFW_RELEASE)
    {
        mode = Mode::None;
        firstMouse = true;
        return;
    }

    // On press: choose mode. You can tweak policy here.
    // Recommended policy:
    // - Alt + LMB : orbit
    // - RMB       : pan (track + pedestal)
    // - MMB       : dolly (truck in/out)
    const bool altDown = (mods & GLFW_MOD_ALT) != 0;

    if (button == GLFW_MOUSE_BUTTON_LEFT && altDown)
    {
        mode = Mode::Orbit;
        firstMouse = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        mode = Mode::Pan;
        firstMouse = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        mode = Mode::Dolly;
        firstMouse = true;
    }
    else
    {
        mode = Mode::None;
    }
}

void OrbitalCameraController::OnCursorPos(GLFWwindow* /*window*/, double x, double y)
{
    // Once a drag is active, we keep controlling the camera even if the cursor passes
    // over an ImGui window. If you prefer to stop immediately when ImGui captures,
    // add a WantCaptureMouse check here too.
    if (mode == Mode::None)
    {
        // Keep last positions updated so the next drag starts “clean”.
        lastX = static_cast<float>(x);
        lastY = static_cast<float>(y);
        return;
    }

    if (firstMouse)
    {
        lastX = static_cast<float>(x);
        lastY = static_cast<float>(y);
        firstMouse = false;
        return;
    }

    const float xpos = static_cast<float>(x);
    const float ypos = static_cast<float>(y);

    const float dx = xpos - lastX;
    const float dy = lastY - ypos; // invert Y to match common camera conventions

    lastX = xpos;
    lastY = ypos;

    switch (mode)
    {
    case Mode::Orbit:
        camera.orbit(dx * orbitSensitivity, dy * orbitSensitivity);
        break;

    case Mode::Pan:
        camera.track(dx * panSensitivity);
        camera.pedestal(dy * panSensitivity);
        break;

    case Mode::Dolly:
        camera.dolly(dy * dollySensitivity);
        break;

    default:
        break;
    }
}

void OrbitalCameraController::OnScroll(GLFWwindow* /*window*/, double /*xoff*/, double yoff)
{
    // Allow UI to consume scroll when hovered.
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    camera.zoom(static_cast<float>(yoff) * zoomSensitivity);
}
