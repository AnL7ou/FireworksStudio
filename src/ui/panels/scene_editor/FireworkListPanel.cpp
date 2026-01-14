#include "FireworkListPanel.h"

#include <imgui.h>

#include "src/scene/Scene.h"
#include "src/fireworks/template/TemplateLibrary.h"

namespace ui {
namespace panels {

FireworkListPanel::FireworkListPanel(Scene* s, TemplateLibrary* l)
    : scene(s)
    , library(l)
    , onSelected(nullptr)
    , onEditSelected(nullptr)
    , selectedIndex(-1)
    , selectedIndexPtr(nullptr)
{
}

void FireworkListPanel::Render()
{
    if (!scene) {
        ImGui::TextDisabled("Aucune scène");
        return;
    }

    auto& events = scene->GetEvents();
    ImGui::Text("Feux dans la scène: %zu", events.size());

    if (ImGui::Button("+")) {
        FireworkEvent e;
        // Each scene event should be editable independently.
        // So we clone the currently active template instead of referencing it directly.
        if (library) {
            int active = library->GetActiveId();
            e.templateId = (active >= 0) ? library->Clone(active) : -1;
        } else {
            e.templateId = -1;
        }
        e.triggerTime = 0.0f;
        e.position = glm::vec3(0.0f, 0.0f, 0.0f);
        e.label = "Firework";
        scene->AddEvent(e);
    }
    ImGui::SameLine();
    {
        const bool hasSelection = (selectedIndex >= 0 && selectedIndex < static_cast<int>(events.size()));
        ImGui::BeginDisabled(!hasSelection);
        if (ImGui::Button("Editer")) {
            if (onEditSelected) onEditSelected(selectedIndex);
        }
        ImGui::EndDisabled();
    }

    ImGui::Separator();

    // Sync from shared selection if provided
    if (selectedIndexPtr) selectedIndex = *selectedIndexPtr;

    if (ImGui::BeginListBox("##events", ImVec2(-FLT_MIN, 220))) {
        for (int i = 0; i < static_cast<int>(events.size()); ++i) {
            const auto& e = events[i];
            char buf[256];
            const char* tmplName = "<none>";
            if (library) {
                auto* t = library->Get(e.templateId);
                if (t) tmplName = t->name.c_str();
            }
            snprintf(buf, sizeof(buf), "[%02d] t=%.2fs  %s", i, e.triggerTime, tmplName);
            bool selected = (i == selectedIndex);
            if (ImGui::Selectable(buf, selected)) {
                selectedIndex = i;
                if (selectedIndexPtr) *selectedIndexPtr = i;
                if (onSelected) onSelected(i);
            }

            // Drag this event onto the timeline to move it.
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("SCENE_EVENT_INDEX", &i, sizeof(int));
                ImGui::TextUnformatted(buf);
                ImGui::EndDragDropSource();
            }
        }
        ImGui::EndListBox();
    }

    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(events.size())) {
        ImGui::Separator();
        if (ImGui::Button("Supprimer sélection")) {
            scene->RemoveEvent(static_cast<size_t>(selectedIndex));
            if (selectedIndex >= static_cast<int>(events.size())) {
                selectedIndex = static_cast<int>(events.size()) - 1;
            }
            if (selectedIndexPtr) *selectedIndexPtr = selectedIndex;
        }
    }
}

} // namespace panels
} // namespace ui
