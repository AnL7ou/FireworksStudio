#include "UIManager.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "panels/template_editor/TemplatePropertiesPanel.h"
#include "panels/template_editor/LayoutEditorPanel.h"
#include "panels/template_editor/ColorSchemePanel.h"
#include "panels/template_editor/TemplateLibraryPanel.h"

#include "panels/scene_editor/FireworkListPanel.h"
#include "panels/scene_editor/SceneViewPanel.h"
#include "panels/scene_editor/TimelinePanel.h"

#include "../fireworks/template/FireworkTemplate.h"
#include "../fireworks/template/TemplateLibrary.h"

#include "../scene/Scene.h"
#include "../scene/Timeline.h"

#include "EditorMode.h"

UIManager::UIManager()
    : window(nullptr)
    , initialized(false)
    , templatePanel(nullptr)
    , layoutPanel(nullptr)
    , colorSchemePanel(nullptr)
    , templateLibraryPanel(nullptr)
    , fireworkListPanel(nullptr)
    , sceneViewPanel(nullptr)
    , timelinePanel(nullptr)
    , showDemoWindow(false)
    , mode(EditorMode::Template)
    , selectedSceneEventIndex(-1)
{
}

UIManager::~UIManager()
{
    Shutdown();
}

bool UIManager::Initialize(GLFWwindow* w)
{
    if (initialized) return true;

    window = w;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    initialized = true;
    return true;
}

void UIManager::Shutdown()
{
    if (!initialized) return;

    // Delete panels
    delete templatePanel;
    templatePanel = nullptr;

    delete layoutPanel;
    layoutPanel = nullptr;

    delete colorSchemePanel;
    colorSchemePanel = nullptr;

    delete templateLibraryPanel;
    templateLibraryPanel = nullptr;

    delete fireworkListPanel;
    fireworkListPanel = nullptr;

    delete sceneViewPanel;
    sceneViewPanel = nullptr;

    delete timelinePanel;
    timelinePanel = nullptr;

    // Shutdown ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    initialized = false;
}

void UIManager::NewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::Render()
{
    RenderMainMenuBar();

    if (mode == EditorMode::Template) {
        if (templateLibraryPanel) {
            ImGui::Begin("Template Library");
            templateLibraryPanel->Render();
            ImGui::End();
        }
        if (templatePanel) {
            ImGui::Begin("Template Properties");
            templatePanel->Render();
            ImGui::End();
        }
        if (layoutPanel) {
            ImGui::Begin("Branch Layout");
            layoutPanel->Render();
            ImGui::End();
        }
        if (colorSchemePanel) {
            ImGui::Begin("Color Scheme");
            colorSchemePanel->Render();
            ImGui::End();
        }
    } else {
        if (fireworkListPanel) {
            ImGui::Begin("Scene Events");
            fireworkListPanel->Render();
            ImGui::End();
        }
        if (sceneViewPanel) {
            ImGui::Begin("Scene Inspector");
            sceneViewPanel->Render();
            ImGui::End();
        }
        if (timelinePanel) {
            ImGui::Begin("Timeline");
            timelinePanel->Render();
            ImGui::End();
        }
    }

    // Demo window (optional)
    if (showDemoWindow) {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::CreateTemplatePanels(TemplateLibrary* library)
{
    if (templateLibraryPanel) {
        delete templateLibraryPanel;
    }
    templateLibraryPanel = new ui::panels::TemplateLibraryPanel(library);

    // Create / refresh dependent panels based on active template
    FireworkTemplate* active = library ? library->GetActive() : nullptr;

    if (templatePanel) delete templatePanel;
    templatePanel = new ui::panels::TemplatePropertiesPanel(active);

    if (layoutPanel) delete layoutPanel;
    layoutPanel = active ? new ui::panels::LayoutEditorPanel(&active->layout) : nullptr;
    if (layoutPanel && active) {
        layoutPanel->SetOnLayoutChangedCallback([active](const BranchLayout&) {
            active->RegenerateBranches();
        });
    }

    if (colorSchemePanel) delete colorSchemePanel;
    colorSchemePanel = active ? new ui::panels::ColorSchemePanel(&active->colorScheme) : nullptr;
    if (colorSchemePanel && active) {
        colorSchemePanel->SetOnColorSchemeChangedCallback([active](const ColorScheme&) {
            active->RegenerateBranches();
        });
    }

    if (templateLibraryPanel) {
        templateLibraryPanel->SetOnSelectionChanged([this, library](int activeId) {
            FireworkTemplate* a = library ? library->Get(activeId) : nullptr;
            if (templatePanel) templatePanel->SetTemplate(a);
            if (layoutPanel) delete layoutPanel;
            layoutPanel = a ? new ui::panels::LayoutEditorPanel(&a->layout) : nullptr;
            if (layoutPanel && a) {
                layoutPanel->SetOnLayoutChangedCallback([a](const BranchLayout&) { a->RegenerateBranches(); });
            }
            if (colorSchemePanel) delete colorSchemePanel;
            colorSchemePanel = a ? new ui::panels::ColorSchemePanel(&a->colorScheme) : nullptr;
            if (colorSchemePanel && a) {
                colorSchemePanel->SetOnColorSchemeChangedCallback([a](const ColorScheme&) { a->RegenerateBranches(); });
            }
        });
    }
}

void UIManager::CreateScenePanels(Scene* scene, Timeline* timeline, TemplateLibrary* library)
{
    if (fireworkListPanel) delete fireworkListPanel;
    if (sceneViewPanel) delete sceneViewPanel;
    if (timelinePanel) delete timelinePanel;

    selectedSceneEventIndex = -1;
    fireworkListPanel = new ui::panels::FireworkListPanel(scene, library);
    sceneViewPanel = new ui::panels::SceneViewPanel(scene, library);
    timelinePanel = new ui::panels::TimelinePanel(scene, timeline, library);

    // Share the same selection across the list + timeline.
    if (timelinePanel) timelinePanel->SetSelectedEventIndexPtr(&selectedSceneEventIndex);
    if (fireworkListPanel) fireworkListPanel->SetSelectedEventIndexPtr(&selectedSceneEventIndex);

    fireworkListPanel->SetOnEventSelected([this](int idx) {
        selectedSceneEventIndex = idx;
        if (sceneViewPanel) sceneViewPanel->SetSelectedEvent(idx);
    });
}

void UIManager::RenderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Mode")) {
            bool isTemplate = (mode == EditorMode::Template);
            bool isScene = (mode == EditorMode::Scene);
            if (ImGui::MenuItem("Template Editor", nullptr, isTemplate)) {
                mode = EditorMode::Template;
            }
            if (ImGui::MenuItem("Scene Editor", nullptr, isScene)) {
                mode = EditorMode::Scene;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Template")) {
                // TODO
            }
            if (ImGui::MenuItem("Load Template")) {
                // TODO
            }
            if (ImGui::MenuItem("Save Template")) {
                // TODO
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Show Demo Window", nullptr, &showDemoWindow);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // TODO
            }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void UIManager::RenderDockSpace()
{
    // Fonction vide si pas de support docking
    // On utilise juste des fenêtres ImGui normales
}