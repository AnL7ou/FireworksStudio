#pragma once

#include <GLFW/glfw3.h>

// Small RAII wrapper around Dear ImGui initialization / shutdown.
// It keeps UIManager lean by extracting backend boilerplate.
namespace ui::subsystems {

class ImGuiLayer {
public:
    ImGuiLayer() = default;
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer&) = delete;
    ImGuiLayer& operator=(const ImGuiLayer&) = delete;

    bool Initialize(GLFWwindow* window);
    void Shutdown();

    void NewFrame();
    void RenderDrawData();

    bool IsInitialized() const noexcept { return initialized; }

private:
    GLFWwindow* window = nullptr;
    bool initialized = false;
};

} // namespace ui::subsystems
