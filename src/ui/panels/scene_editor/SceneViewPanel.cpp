#include "SceneViewPanel.h"

#include <imgui.h>

#include "src/scene/Scene.h"
#include "src/fireworks/template/TemplateLibrary.h"

namespace ui {
namespace panels {

SceneViewPanel::SceneViewPanel(Scene* s, TemplateLibrary* l)
    : scene(s)
    , library(l)
    , selectedEvent(-1)
{
}

void SceneViewPanel::Render()
{
    if (!scene) {
        ImGui::TextDisabled("Aucune scène");
        return;
    }

    auto& events = scene->GetEvents();
    if (selectedEvent < 0 || selectedEvent >= static_cast<int>(events.size())) {
        ImGui::TextDisabled("Aucun événement sélectionné");
        return;
    }

    auto& e = events[static_cast<size_t>(selectedEvent)];

    ImGui::Text("Événement #%d", selectedEvent);
    ImGui::Checkbox("Actif", &e.enabled);

    ImGui::InputFloat("Trigger time (s)", &e.triggerTime, 0.1f, 1.0f, "%.2f");
    ImGui::DragFloat3("Position", &e.position.x, 0.1f);

    // Template selection by ID
    if (library) {
        const auto* t = library->Get(e.templateId);
        ImGui::Text("Template: %s", t ? t->name.c_str() : "<none>");

        if (ImGui::Button("Utiliser template actif")) {
            e.templateId = library->GetActiveId();
        }
    }
}

} // namespace panels
} // namespace ui
