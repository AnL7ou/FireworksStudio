#pragma once

#include <functional>
#include <string>

#include "src/ui/widgets/FileDialogModal.h"

namespace ui::subsystems {

// Handles file-related UI (open/save dialog) and delegates the actual operation to a callback.
// The callback is typically wired to serialization in UIManager.
class FileController {
public:
    enum class Action {
        SaveTemplate,
        LoadTemplate,
        SaveScene,
        LoadScene,
    };

    using PerformCallback = std::function<void(Action, const std::string& path)>;

    explicit FileController(PerformCallback cb);

    // Must be called from inside the File menu item handler.
    // It will close the current menu popup and request opening the modal next frame.
    void RequestFromMenu(Action action);

    // Call every frame after the menu bar is rendered.
    void UpdateDeferredOpen();
    void Render();

private:
    void request(Action action);

    ui::widgets::FileDialogModal dialog;
    PerformCallback perform;

    Action pendingAction = Action::LoadScene;

    bool openRequested = false;
    ui::widgets::FileDialogModal::Kind requestedKind = ui::widgets::FileDialogModal::Kind::Load;
    std::string requestedTitle;
    std::string requestedDefaultPath;
    std::string requestedExt;
};

} // namespace ui::subsystems
