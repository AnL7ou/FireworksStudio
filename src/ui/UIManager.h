#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include <memory>

namespace ui {
    namespace subsystems {
        class ImGuiLayer;
        class FileController;
        class Panels;
        class MenuBar;
    }
}

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

    // High-level file operations triggered from the menu.
    enum class FileAction {
        SaveTemplate,
        LoadTemplate,
        SaveScene,
        LoadScene,
    };


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
    ui::panels::TemplatePropertiesPanel* GetTemplatePanel() const;
    ui::panels::LayoutEditorPanel* GetLayoutPanel() const;
    ui::panels::ColorSchemePanel* GetColorSchemePanel() const;

    ui::panels::TemplateLibraryPanel* GetTemplateLibraryPanel() const;

    ui::panels::FireworkListPanel* GetFireworkListPanel() const;
    ui::panels::SceneViewPanel* GetSceneViewPanel() const;
    ui::panels::TimelinePanel* GetTimelinePanel() const;

    // Scene selection helpers (shared across scene panels).
    int GetSelectedSceneEventIndex() const;
    void SetSelectedSceneEventIndex(int idx);

    // Convenience: switch to template editor and focus the template used by a scene event.
    void EditTemplateForSceneEvent(int sceneEventIndex);

private:
    // Called by subsystems
    void PerformFileAction(FileAction action, const std::string& path);

    GLFWwindow* window;

    // Context pointers (owned by Application)
    class TemplateLibrary* templateLibraryCtx = nullptr;
    class Scene* sceneCtx = nullptr;
    class Timeline* timelineCtx = nullptr;

    bool initialized;

    // UI state
    bool showDemoWindow;
    EditorMode mode;

    // UI subsystems
    std::unique_ptr<ui::subsystems::ImGuiLayer> imgui;
    std::unique_ptr<ui::subsystems::FileController> files;
    std::unique_ptr<ui::subsystems::Panels> panels;
    std::unique_ptr<ui::subsystems::MenuBar> menubar;
};