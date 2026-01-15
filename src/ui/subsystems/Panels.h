#pragma once

#include "src/ui/EditorMode.h"

namespace ui::panels {
    class TemplateLibraryPanel;
    class TemplatePropertiesPanel;
    class LayoutEditorPanel;
    class ColorSchemePanel;
    class FireworkListPanel;
    class SceneViewPanel;
    class TimelinePanel;
}

class TemplateLibrary;
class Scene;
class Timeline;
class FireworkTemplate;
class BranchLayout;
class ColorScheme;

namespace ui::subsystems {

// Owns and renders all editor panels. UIManager delegates all panel lifecycle to this class.
class Panels {
public:
    Panels() = default;
    ~Panels();

    Panels(const Panels&) = delete;
    Panels& operator=(const Panels&) = delete;

    void DestroyAll();

    void CreateTemplatePanels(TemplateLibrary* library);
    void CreateScenePanels(Scene* scene, Timeline* timeline, TemplateLibrary* library);

    void Render(EditorMode mode);

    // Needed by application code that wires callbacks.
    ui::panels::TemplatePropertiesPanel* GetTemplatePanel() const noexcept { return templatePanel; }
    ui::panels::LayoutEditorPanel* GetLayoutPanel() const noexcept { return layoutPanel; }
    ui::panels::ColorSchemePanel* GetColorSchemePanel() const noexcept { return colorSchemePanel; }
    ui::panels::TemplateLibraryPanel* GetTemplateLibraryPanel() const noexcept { return templateLibraryPanel; }
    ui::panels::FireworkListPanel* GetFireworkListPanel() const noexcept { return fireworkListPanel; }
    ui::panels::SceneViewPanel* GetSceneViewPanel() const noexcept { return sceneViewPanel; }
    ui::panels::TimelinePanel* GetTimelinePanel() const noexcept { return timelinePanel; }

    // Shared selection across scene panels.
    int GetSelectedSceneEventIndex() const noexcept { return selectedSceneEventIndex; }
    void SetSelectedSceneEventIndex(int idx) noexcept { selectedSceneEventIndex = idx; }
    int* GetSelectedSceneEventIndexPtr() noexcept { return &selectedSceneEventIndex; }

    // Rebind template-related panels to the currently active template after non-UI template changes.
    void RefreshTemplatePanelsFromActive();

    // Contexts are used for refresh operations.
    void SetContexts(TemplateLibrary* library, Scene* scene, Timeline* timeline);

private:
    void renderTemplate();
    void renderScene();

    TemplateLibrary* templateLibraryCtx = nullptr;
    Scene* sceneCtx = nullptr;
    Timeline* timelineCtx = nullptr;

    // Template editor panels
    ui::panels::TemplatePropertiesPanel* templatePanel = nullptr;
    ui::panels::LayoutEditorPanel* layoutPanel = nullptr;
    ui::panels::ColorSchemePanel* colorSchemePanel = nullptr;
    ui::panels::TemplateLibraryPanel* templateLibraryPanel = nullptr;

    // Scene editor panels
    ui::panels::FireworkListPanel* fireworkListPanel = nullptr;
    ui::panels::SceneViewPanel* sceneViewPanel = nullptr;
    ui::panels::TimelinePanel* timelinePanel = nullptr;

    int selectedSceneEventIndex = -1;
};

} // namespace ui::subsystems
