#pragma once

#include <functional>

#include "src/ui/EditorMode.h"
#include "FileController.h"

namespace ui::subsystems {

// Renders the main menu bar and exposes all decisions through callbacks.
class MenuBar {
public:
    struct Callbacks {
        std::function<void(EditorMode)> onModeChanged;
        std::function<void()> onNewTemplate;
        std::function<void()> onNewScene;
        std::function<void(FileController::Action)> onFileAction;
        std::function<void()> onExit;
    };

    explicit MenuBar(Callbacks cb);

    void Render(EditorMode currentMode, bool* showDemoWindow);

private:
    Callbacks callbacks;
};

} // namespace ui::subsystems
