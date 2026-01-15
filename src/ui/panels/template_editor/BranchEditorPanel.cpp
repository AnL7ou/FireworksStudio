#include "BranchEditorPanel.h"

#include <imgui.h>

using namespace ui::panels;

void BranchEditorPanel::Render()
{
    ImGui::TextUnformatted("Branch editor (not implemented)");
    if (!tmpl) {
        ImGui::TextUnformatted("No active template.");
        return;
    }
    ImGui::TextUnformatted("This panel is reserved for future per-branch controls.");
}
