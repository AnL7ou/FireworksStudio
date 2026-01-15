#include "Panels.h"

#include <imgui.h>

#include "src/ui/panels/template_editor/TemplatePropertiesPanel.h"
#include "src/ui/panels/template_editor/LayoutEditorPanel.h"
#include "src/ui/panels/template_editor/ColorSchemePanel.h"
#include "src/ui/panels/template_editor/TemplateLibraryPanel.h"

#include "src/ui/panels/scene_editor/FireworkListPanel.h"
#include "src/ui/panels/scene_editor/SceneViewPanel.h"
#include "src/ui/panels/scene_editor/TimelinePanel.h"

#include "src/fireworks/template/TemplateLibrary.h"
#include "src/fireworks/template/FireworkTemplate.h"

using namespace ui::subsystems;

Panels::~Panels() { DestroyAll(); }

void Panels::DestroyAll()
{
    delete templatePanel; templatePanel = nullptr;
    delete layoutPanel; layoutPanel = nullptr;
    delete colorSchemePanel; colorSchemePanel = nullptr;
    delete templateLibraryPanel; templateLibraryPanel = nullptr;

    delete fireworkListPanel; fireworkListPanel = nullptr;
    delete sceneViewPanel; sceneViewPanel = nullptr;
    delete timelinePanel; timelinePanel = nullptr;
}

void Panels::SetContexts(TemplateLibrary* library, Scene* scene, Timeline* timeline)
{
    templateLibraryCtx = library;
    sceneCtx = scene;
    timelineCtx = timeline;
}

void Panels::CreateTemplatePanels(TemplateLibrary* library)
{
    templateLibraryCtx = library;

    delete templateLibraryPanel;
    templateLibraryPanel = new ui::panels::TemplateLibraryPanel(library);

    FireworkTemplate* active = library ? library->GetActive() : nullptr;

    delete templatePanel;
    templatePanel = new ui::panels::TemplatePropertiesPanel(active);

    delete layoutPanel;
    layoutPanel = active ? new ui::panels::LayoutEditorPanel(&active->layout) : nullptr;
    if (layoutPanel && active) {
        layoutPanel->SetOnLayoutChangedCallback([active](const BranchLayout&) {
            active->RegenerateBranches();
        });
    }

    delete colorSchemePanel;
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

            delete layoutPanel;
            layoutPanel = a ? new ui::panels::LayoutEditorPanel(&a->layout) : nullptr;
            if (layoutPanel && a) {
                layoutPanel->SetOnLayoutChangedCallback([a](const BranchLayout&) { a->RegenerateBranches(); });
            }

            delete colorSchemePanel;
            colorSchemePanel = a ? new ui::panels::ColorSchemePanel(&a->colorScheme) : nullptr;
            if (colorSchemePanel && a) {
                colorSchemePanel->SetOnColorSchemeChangedCallback([a](const ColorScheme&) { a->RegenerateBranches(); });
            }
        });
    }
}

void Panels::CreateScenePanels(Scene* scene, Timeline* timeline, TemplateLibrary* library)
{
    sceneCtx = scene;
    timelineCtx = timeline;
    templateLibraryCtx = library;

    delete fireworkListPanel;
    delete sceneViewPanel;
    delete timelinePanel;

    selectedSceneEventIndex = -1;
    fireworkListPanel = new ui::panels::FireworkListPanel(scene, library);
    sceneViewPanel = new ui::panels::SceneViewPanel(scene, library);
    timelinePanel = new ui::panels::TimelinePanel(scene, timeline, library);

    if (timelinePanel) timelinePanel->SetSelectedEventIndexPtr(&selectedSceneEventIndex);
    if (fireworkListPanel) fireworkListPanel->SetSelectedEventIndexPtr(&selectedSceneEventIndex);

    if (fireworkListPanel) {
        fireworkListPanel->SetOnEventSelected([this](int idx) {
            selectedSceneEventIndex = idx;
            if (sceneViewPanel) sceneViewPanel->SetSelectedEvent(idx);
            if (timelinePanel) timelinePanel->FocusEvent(idx);
        });
    }
}

void Panels::RefreshTemplatePanelsFromActive()
{
    if (!templateLibraryCtx) return;

    FireworkTemplate* a = templateLibraryCtx->Get(templateLibraryCtx->GetActiveId());
    if (templatePanel) templatePanel->SetTemplate(a);

    delete layoutPanel; layoutPanel = nullptr;
    layoutPanel = a ? new ui::panels::LayoutEditorPanel(&a->layout) : nullptr;
    if (layoutPanel && a) {
        layoutPanel->SetOnLayoutChangedCallback([a](const BranchLayout&) { a->RegenerateBranches(); });
    }

    delete colorSchemePanel; colorSchemePanel = nullptr;
    colorSchemePanel = a ? new ui::panels::ColorSchemePanel(&a->colorScheme) : nullptr;
    if (colorSchemePanel && a) {
        colorSchemePanel->SetOnColorSchemeChangedCallback([a](const ColorScheme&) { a->RegenerateBranches(); });
    }
}

void Panels::Render(EditorMode mode)
{
    if (mode == EditorMode::Template) renderTemplate();
    else renderScene();
}

void Panels::renderTemplate()
{
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
}

void Panels::renderScene()
{
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
