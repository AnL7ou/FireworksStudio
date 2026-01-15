#include "ScenePlacementController.h"

#include <algorithm>
#include <cmath>

#include <imgui.h>

#include "src/rendering/Camera.h"
#include "src/scene/Scene.h"
#include "src/ui/UIManager.h"
#include "src/ui/EditorMode.h"

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>

ScenePlacementController::ScenePlacementController(Camera& cam, UIManager& u, Scene& s)
    : camera(cam)
    , ui(u)
    , scene(s)
{
}

void ScenePlacementController::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    if (ui.GetMode() != EditorMode::Scene) return;
    if (ImGui::GetIO().WantCaptureMouse) return;

    const bool altDown = (mods & GLFW_MOD_ALT) != 0;
    if (altDown) return;

    if (button != GLFW_MOUSE_BUTTON_LEFT) return;

    if (action == GLFW_PRESS) {
        double mx = 0.0, my = 0.0;
        glfwGetCursorPos(window, &mx, &my);

        int idx = pickEvent(window, mx, my);
        if (idx >= 0) {
            setSelected(idx);
            dragging = true;
            draggedEvent = idx;
        }
    }

    if (action == GLFW_RELEASE) {
        dragging = false;
        draggedEvent = -1;
    }
}

void ScenePlacementController::OnCursorPos(GLFWwindow* window, double x, double y)
{
    if (!dragging) return;
    if (ui.GetMode() != EditorMode::Scene) return;
    if (ImGui::GetIO().WantCaptureMouse) return;

    auto& events = scene.GetEvents();
    if (draggedEvent < 0 || draggedEvent >= static_cast<int>(events.size())) return;

    glm::vec3 hit;
    if (!raycastToGround(window, x, y, hit)) return;

    // Keep the event on the ground plane. If you later want full 3D placement, change this.
    events[draggedEvent].position = glm::vec3(hit.x, 0.0f, hit.z);
}

int ScenePlacementController::pickEvent(GLFWwindow* window, double mx, double my) const
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0) return -1;

    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const glm::mat4 view = camera.getViewMatrix();
    const glm::mat4 proj = camera.getProjectionMatrix(aspect);

    // GLFW cursor origin is top-left. glm::project uses bottom-left.
    const float mouseX = static_cast<float>(mx);
    const float mouseY = static_cast<float>(my);

    const glm::vec4 viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));

    const auto& events = scene.GetEvents();
    int best = -1;
    float bestDist2 = 0.0f;

    constexpr float kPickRadiusPx = 14.0f;
    const float kPickRadius2 = kPickRadiusPx * kPickRadiusPx;

    for (int i = 0; i < static_cast<int>(events.size()); ++i) {
        const glm::vec3 p = events[i].position;
        glm::vec3 screen = glm::project(p, view, proj, viewport);
        float sx = screen.x;
        float sy = static_cast<float>(height) - screen.y;

        float dx = sx - mouseX;
        float dy = sy - mouseY;
        float d2 = dx * dx + dy * dy;
        if (d2 <= kPickRadius2) {
            if (best < 0 || d2 < bestDist2) {
                best = i;
                bestDist2 = d2;
            }
        }
    }

    return best;
}

bool ScenePlacementController::raycastToGround(GLFWwindow* window, double mx, double my, glm::vec3& outPos) const
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0) return false;

    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    const glm::mat4 view = camera.getViewMatrix();
    const glm::mat4 proj = camera.getProjectionMatrix(aspect);
    const glm::mat4 invVP = glm::inverse(proj * view);

    // NDC (x: -1..1, y: -1..1) with GLFW top-left origin.
    float x = (2.0f * static_cast<float>(mx) / static_cast<float>(width)) - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(my) / static_cast<float>(height));

    glm::vec4 nearP = invVP * glm::vec4(x, y, -1.0f, 1.0f);
    glm::vec4 farP  = invVP * glm::vec4(x, y,  1.0f, 1.0f);
    if (nearP.w == 0.0f || farP.w == 0.0f) return false;
    nearP /= nearP.w;
    farP  /= farP.w;

    glm::vec3 origin(nearP);
    glm::vec3 dir = glm::normalize(glm::vec3(farP - nearP));

    // Intersect with y=0 plane.
    const float eps = 1e-6f;
    if (std::abs(dir.y) < eps) return false;
    float t = (0.0f - origin.y) / dir.y;
    if (t < 0.0f) return false;

    outPos = origin + dir * t;
    return true;
}

void ScenePlacementController::setSelected(int idx) const
{
    ui.SetSelectedSceneEventIndex(idx);
}
