#include "MenuBar.h"

#include <imgui.h>

using namespace ui::subsystems;

MenuBar::MenuBar(Callbacks cb)
    : callbacks(std::move(cb))
{
}

void MenuBar::Render(EditorMode mode, bool* showDemoWindow)
{
    if (!ImGui::BeginMainMenuBar()) return;

    if (ImGui::BeginMenu("Mode")) {
        const bool isTemplate = (mode == EditorMode::Template);
        const bool isScene = (mode == EditorMode::Scene);
        if (ImGui::MenuItem("Template Editor", nullptr, isTemplate)) {
            if (callbacks.onModeChanged) callbacks.onModeChanged(EditorMode::Template);
        }
        if (ImGui::MenuItem("Scene Editor", nullptr, isScene)) {
            if (callbacks.onModeChanged) callbacks.onModeChanged(EditorMode::Scene);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("File")) {
        if (mode == EditorMode::Template) {
            if (ImGui::MenuItem("New Template")) {
                if (callbacks.onNewTemplate) callbacks.onNewTemplate();
            }
            if (ImGui::MenuItem("Load Template...")) {
                if (callbacks.onFileAction) callbacks.onFileAction(FileController::Action::LoadTemplate);
            }
            if (ImGui::MenuItem("Save Template...")) {
                if (callbacks.onFileAction) callbacks.onFileAction(FileController::Action::SaveTemplate);
            }
        } else {
            if (ImGui::MenuItem("New Scene")) {
                if (callbacks.onNewScene) callbacks.onNewScene();
            }
            if (ImGui::MenuItem("Load Scene...")) {
                if (callbacks.onFileAction) callbacks.onFileAction(FileController::Action::LoadScene);
            }
            if (ImGui::MenuItem("Save Scene...")) {
                if (callbacks.onFileAction) callbacks.onFileAction(FileController::Action::SaveScene);
            }
        }

        ImGui::Separator();
        if (ImGui::MenuItem("Exit")) {
            if (callbacks.onExit) callbacks.onExit();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        if (showDemoWindow) ImGui::MenuItem("Show Demo Window", nullptr, showDemoWindow);
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
