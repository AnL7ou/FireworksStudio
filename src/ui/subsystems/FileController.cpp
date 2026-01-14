#include "FileController.h"

#include <imgui.h>

using namespace ui::subsystems;

FileController::FileController(PerformCallback cb)
    : perform(std::move(cb))
{
}

void FileController::RequestFromMenu(Action action)
{
    // Close the File menu popup, then open the modal next frame.
    ImGui::CloseCurrentPopup();
    request(action);
}

void FileController::request(Action action)
{
    pendingAction = action;

    const bool isTemplate = (action == Action::SaveTemplate) || (action == Action::LoadTemplate);
    const bool isSave = (action == Action::SaveTemplate) || (action == Action::SaveScene);

    const char* wantedExt = isTemplate ? ".fwasset" : ".fwscene";
    const char* defaultPath = isTemplate ? "assets/template.fwasset" : "scenes/scene.fwscene";

    const char* title = "File";
    switch (action) {
    case Action::SaveTemplate: title = "Save Template"; break;
    case Action::LoadTemplate: title = "Load Template"; break;
    case Action::SaveScene:    title = "Save Scene"; break;
    case Action::LoadScene:    title = "Load Scene"; break;
    }

    requestedKind = isSave ? ui::widgets::FileDialogModal::Kind::Save
                           : ui::widgets::FileDialogModal::Kind::Load;
    requestedTitle = title;
    requestedDefaultPath = defaultPath;
    requestedExt = wantedExt;
    openRequested = true;
}

void FileController::UpdateDeferredOpen()
{
    if (!openRequested) return;
    openRequested = false;

    dialog.Open(requestedKind,
                requestedTitle.c_str(),
                requestedDefaultPath.c_str(),
                requestedExt.c_str(),
                [this](const std::string& path) {
                    if (perform) perform(pendingAction, path);
                });
}

void FileController::Render() { dialog.Render(); }
