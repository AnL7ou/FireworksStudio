#include "ColorPicker.h"

#include <imgui.h>

using namespace ui::widgets;

bool ColorPicker::EditRGBA(const char* label, float rgba[4])
{
    // Uses a compact color button + picker popup.
    return ImGui::ColorEdit4(label, rgba, ImGuiColorEditFlags_Float);
}
