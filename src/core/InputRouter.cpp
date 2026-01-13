#include "InputRouter.h"

#include "backends/imgui_impl_glfw.h"

void InputRouter::DispatchMouseButton(GLFWwindow* w, int b, int a, int m)
{
    ImGui_ImplGlfw_MouseButtonCallback(w, b, a, m);
    for (auto* l : listeners) l->OnMouseButton(w, b, a, m);
}

void InputRouter::DispatchCursorPos(GLFWwindow* w, double x, double y)
{
    ImGui_ImplGlfw_CursorPosCallback(w, x, y);
    for (auto* l : listeners) l->OnCursorPos(w, x, y);
}

void InputRouter::DispatchScroll(GLFWwindow* w, double xoff, double yoff)
{
    ImGui_ImplGlfw_ScrollCallback(w, xoff, yoff);
    for (auto* l : listeners) l->OnScroll(w, xoff, yoff);
}
