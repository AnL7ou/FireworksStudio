#pragma once

namespace ui::widgets {

// Small wrapper around common ImGui color picking patterns.
// This is optional sugar; existing panels can keep using ImGui directly.
struct ColorPicker {
    // Returns true if the color changed.
    static bool EditRGBA(const char* label, float rgba[4]);
};

} // namespace ui::widgets
