#include "UIManager.h"

#include <imgui.h>

#include "src/ui/subsystems/ImGuiLayer.h"
#include "src/ui/subsystems/FileController.h"
#include "src/ui/subsystems/Panels.h"
#include "src/ui/subsystems/MenuBar.h"

#include "src/ui/EditorMode.h"

#include "src/fireworks/template/TemplateLibrary.h"
#include "src/fireworks/template/FireworkTemplate.h"
#include "src/fireworks/asset/FireworkAsset.h"
#include "src/serialization/FireworkSerialization.h"

#include "src/scene/Scene.h"
#include "src/scene/Timeline.h"

#include "src/ui/panels/scene_editor/SceneViewPanel.h"
#include "src/ui/panels/scene_editor/TimelinePanel.h"
#include "src/ui/panels/scene_editor/FireworkListPanel.h"

UIManager::UIManager()
    : window(nullptr)
    , initialized(false)
    , showDemoWindow(false)
    , mode(EditorMode::Template)
{
}

UIManager::~UIManager() { Shutdown(); }

bool UIManager::Initialize(GLFWwindow* w)
{
    if (initialized) return true;

    window = w;

    imgui = std::make_unique<ui::subsystems::ImGuiLayer>();
    panels = std::make_unique<ui::subsystems::Panels>();

    // File controller delegates the actual operations back to UIManager.
    files = std::make_unique<ui::subsystems::FileController>(
        [this](ui::subsystems::FileController::Action action, const std::string& path) {
            // Convert subsystem action into the public UIManager enum.
            FileAction a = FileAction::LoadScene;
            switch (action) {
            case ui::subsystems::FileController::Action::SaveTemplate: a = FileAction::SaveTemplate; break;
            case ui::subsystems::FileController::Action::LoadTemplate: a = FileAction::LoadTemplate; break;
            case ui::subsystems::FileController::Action::SaveScene:    a = FileAction::SaveScene;    break;
            case ui::subsystems::FileController::Action::LoadScene:    a = FileAction::LoadScene;    break;
            }
            PerformFileAction(a, path);
        });

    menubar = std::make_unique<ui::subsystems::MenuBar>(ui::subsystems::MenuBar::Callbacks{
        /*onModeChanged*/ [this](EditorMode m) { mode = m; },
        /*onNewTemplate*/ [this]() {
            if (!templateLibraryCtx) return;
            auto t = std::make_unique<::FireworkTemplate>("Untitled Template");
            int id = templateLibraryCtx->Add(std::move(t));
            templateLibraryCtx->SetActiveId(id);
            if (panels) panels->RefreshTemplatePanelsFromActive();
        },
        /*onNewScene*/ [this]() {
            if (!sceneCtx) return;
            *sceneCtx = Scene("Untitled Scene");
            if (timelineCtx) timelineCtx->Reset();
            if (panels) panels->CreateScenePanels(sceneCtx, timelineCtx, templateLibraryCtx);
        },
        /*onFileAction*/ [this](ui::subsystems::FileController::Action a) {
            if (files) files->RequestFromMenu(a);
        },
        /*onExit*/ [this]() {
            if (window) glfwSetWindowShouldClose(window, GLFW_TRUE);
        },
    });

    if (!imgui->Initialize(window)) return false;

    initialized = true;
    return true;
}

void UIManager::Shutdown()
{
    if (!initialized) return;

    if (panels) panels->DestroyAll();
    panels.reset();
    menubar.reset();
    files.reset();

    if (imgui) imgui->Shutdown();
    imgui.reset();

    initialized = false;
}

void UIManager::NewFrame()
{
    if (imgui) imgui->NewFrame();
}

void UIManager::Render()
{
    if (!initialized) return;

    if (menubar) menubar->Render(mode, &showDemoWindow);

    // Important: must run after the menu bar is done.
    if (files) files->UpdateDeferredOpen();
    if (files) files->Render();

    if (panels) panels->Render(mode);

    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    if (imgui) imgui->RenderDrawData();
}

void UIManager::CreateTemplatePanels(TemplateLibrary* library)
{
    templateLibraryCtx = library;
    if (panels) {
        panels->SetContexts(templateLibraryCtx, sceneCtx, timelineCtx);
        panels->CreateTemplatePanels(library);
    }
}

void UIManager::CreateScenePanels(Scene* scene, Timeline* timeline, TemplateLibrary* library)
{
    sceneCtx = scene;
    timelineCtx = timeline;
    templateLibraryCtx = library;
    if (panels) {
        panels->SetContexts(templateLibraryCtx, sceneCtx, timelineCtx);
        panels->CreateScenePanels(scene, timeline, library);
    }

    // Wire scene list "Editer" button: jump to template editor for the selected event.
    if (auto* list = GetFireworkListPanel()) {
        list->SetOnEditSelected([this](int eventIndex) {
            this->EditTemplateForSceneEvent(eventIndex);
        });
    }
}

ui::panels::TemplatePropertiesPanel* UIManager::GetTemplatePanel() const
{
    return panels ? panels->GetTemplatePanel() : nullptr;
}
ui::panels::LayoutEditorPanel* UIManager::GetLayoutPanel() const
{
    return panels ? panels->GetLayoutPanel() : nullptr;
}
ui::panels::ColorSchemePanel* UIManager::GetColorSchemePanel() const
{
    return panels ? panels->GetColorSchemePanel() : nullptr;
}
ui::panels::TemplateLibraryPanel* UIManager::GetTemplateLibraryPanel() const
{
    return panels ? panels->GetTemplateLibraryPanel() : nullptr;
}
ui::panels::FireworkListPanel* UIManager::GetFireworkListPanel() const
{
    return panels ? panels->GetFireworkListPanel() : nullptr;
}
ui::panels::SceneViewPanel* UIManager::GetSceneViewPanel() const
{
    return panels ? panels->GetSceneViewPanel() : nullptr;
}
ui::panels::TimelinePanel* UIManager::GetTimelinePanel() const
{
    return panels ? panels->GetTimelinePanel() : nullptr;
}

int UIManager::GetSelectedSceneEventIndex() const
{
    return panels ? panels->GetSelectedSceneEventIndex() : -1;
}

void UIManager::SetSelectedSceneEventIndex(int idx)
{
    if (panels) panels->SetSelectedSceneEventIndex(idx);
    if (auto* sv = GetSceneViewPanel()) sv->SetSelectedEvent(idx);
    if (auto* tl = GetTimelinePanel()) tl->FocusEvent(idx);
}

void UIManager::EditTemplateForSceneEvent(int sceneEventIndex)
{
    if (!sceneCtx || !templateLibraryCtx) return;
    auto& events = sceneCtx->GetEvents();
    if (sceneEventIndex < 0 || sceneEventIndex >= static_cast<int>(events.size())) return;

    int tmplId = events[sceneEventIndex].templateId;
    if (tmplId >= 0) {
        templateLibraryCtx->SetActiveId(tmplId);
        if (panels) panels->RefreshTemplatePanelsFromActive();
    }

    SetMode(EditorMode::Template);
}

void UIManager::PerformFileAction(FileAction action, const std::string& path)
{
    if (action == FileAction::SaveTemplate) {
        if (templateLibraryCtx) {
            int id = templateLibraryCtx->GetActiveId();
            ::FireworkTemplate* t = templateLibraryCtx->Get(id);
            if (t) {
                auto asset = std::make_shared<fireworks::FireworkAsset>();
                asset->setName(t->name);
                asset->setTemplate(std::make_shared<::FireworkTemplate>(*t));
                serialization::SaveFireworkAsset(*asset, path);
            }
        }
        return;
    }

    if (action == FileAction::LoadTemplate) {
        if (templateLibraryCtx) {
            auto asset = serialization::LoadFireworkAsset(path);
            if (asset && asset->templ()) {
                auto loaded = std::make_unique<::FireworkTemplate>(*asset->templ());
                int newId = templateLibraryCtx->Add(std::move(loaded));
                templateLibraryCtx->SetActiveId(newId);
                if (panels) panels->RefreshTemplatePanelsFromActive();
            }
        }
        return;
    }

    if (action == FileAction::SaveScene) {
        if (sceneCtx) serialization::SaveScene(*sceneCtx, path);
        return;
    }

    if (action == FileAction::LoadScene) {
        if (sceneCtx) {
            auto s = serialization::LoadScene(path);
            if (s) {
                *sceneCtx = *s;
                if (timelineCtx) timelineCtx->Reset();
                if (panels) panels->CreateScenePanels(sceneCtx, timelineCtx, templateLibraryCtx);
            }
        }
        return;
    }
}
