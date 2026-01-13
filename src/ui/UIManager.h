#pragma once

#include <GLFW/glfw3.h>

// Forward declarations
class FireworkTemplate;
class BranchLayout;
class ColorScheme;

namespace ui {
    namespace panels {
        class TemplateLibraryPanel;
        class TemplatePropertiesPanel;
        class LayoutEditorPanel;
        class ColorSchemePanel;

        class FireworkListPanel;
        class SceneViewPanel;
        class TimelinePanel;
    }
}

// Mode enum
enum class EditorMode;

class UIManager {
public:
    UIManager();
    ~UIManager();

    bool Initialize(GLFWwindow* window);
    void Shutdown();

    void NewFrame();
    void Render();

    // Panel creation
    void CreateTemplatePanels(class TemplateLibrary* library);
    void CreateScenePanels(class Scene* scene, class Timeline* timeline, class TemplateLibrary* library);

    // Mode
    void SetMode(EditorMode m) { mode = m; }
    EditorMode GetMode() const { return mode; }

    // Getters
    ui::panels::TemplatePropertiesPanel* GetTemplatePanel() const { return templatePanel; }
    ui::panels::LayoutEditorPanel* GetLayoutPanel() const { return layoutPanel; }
    ui::panels::ColorSchemePanel* GetColorSchemePanel() const { return colorSchemePanel; }

    ui::panels::TemplateLibraryPanel* GetTemplateLibraryPanel() const { return templateLibraryPanel; }

    ui::panels::FireworkListPanel* GetFireworkListPanel() const { return fireworkListPanel; }
    ui::panels::SceneViewPanel* GetSceneViewPanel() const { return sceneViewPanel; }
    ui::panels::TimelinePanel* GetTimelinePanel() const { return timelinePanel; }

private:
    void RenderMainMenuBar();
    void RenderDockSpace();

private:
    GLFWwindow* window;
    bool initialized;

    // Panels
    ui::panels::TemplatePropertiesPanel* templatePanel;
    ui::panels::LayoutEditorPanel* layoutPanel;
    ui::panels::ColorSchemePanel* colorSchemePanel;

    ui::panels::TemplateLibraryPanel* templateLibraryPanel;

    // Scene editor panels
    ui::panels::FireworkListPanel* fireworkListPanel;
    ui::panels::SceneViewPanel* sceneViewPanel;
    ui::panels::TimelinePanel* timelinePanel;

    // UI state
    bool showDemoWindow;
    EditorMode mode;

    // Shared selection between scene panels
    int selectedSceneEventIndex;
};