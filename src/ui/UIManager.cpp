#include "UIManager.h"

#include <imgui.h>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "panels/TemplatePropertyPanel.h"

#include <iostream>

struct UIManager::Impl {
    GLFWwindow* window = nullptr;
    std::unique_ptr<TemplatePropertiesPanel> templatePanel;
};

UIManager::UIManager() noexcept
    : panelImpl(new Impl())
{
}

UIManager::~UIManager() noexcept
{
    delete panelImpl;
    panelImpl = nullptr;
}

bool UIManager::Initialize(GLFWwindow* window)
{
    if (!window)
        return false;

    panelImpl->window = window;

    // Create ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    // Backends init (GLFW + OpenGL3)
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        std::cerr << "UIManager::Initialize - ImGui_ImplGlfw_InitForOpenGL failed\n";
        return false;
    }

    // GLSL version — correspond à votre contexte (ici 330 core)
    const char* glsl_version = "#version 330";
    if (!ImGui_ImplOpenGL3_Init(glsl_version))
    {
        std::cerr << "UIManager::Initialize - ImGui_ImplOpenGL3_Init failed\n";
        return false;
    }

    return true;
}

void UIManager::NewFrame() noexcept
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::Render() noexcept
{
    // Render any panels
    if (panelImpl->templatePanel)
    {
        // Le panneau écrit ses widgets ImGui
        panelImpl->templatePanel->Render();
    }

    // Finish ImGui frame and draw
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Shutdown() noexcept
{
    // destroy panels first
    panelImpl->templatePanel.reset();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::CreateTemplatePanel(FireworkTemplate* tmpl) noexcept
{
    panelImpl->templatePanel = std::make_unique<TemplatePropertiesPanel>(tmpl);
}

TemplatePropertiesPanel* UIManager::GetTemplatePanel() noexcept
{
    return panelImpl->templatePanel.get();
}