#include "PhysicsEditorPanel.h"

#include <imgui.h>

using namespace ui::panels;

void PhysicsEditorPanel::Render()
{
    ImGui::TextUnformatted("Physics editor (not implemented)");
    if (!tmpl) {
        ImGui::TextUnformatted("No active template.");
        return;
    }
    ImGui::TextUnformatted("Reserved for future physics parameters.");
}
