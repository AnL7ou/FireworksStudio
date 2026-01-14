#include "TemplateLibraryPanel.h"

#include <imgui.h>

#include "src/fireworks/template/TemplateLibrary.h"

namespace ui {
namespace panels {

TemplateLibraryPanel::TemplateLibraryPanel(TemplateLibrary* lib)
    : library(lib)
    , onSelectionChanged(nullptr)
    , uiSelectedIndex(0)
{
}

void TemplateLibraryPanel::SetLibrary(TemplateLibrary* lib)
{
    library = lib;
    uiSelectedIndex = 0;
}

void TemplateLibraryPanel::SetOnSelectionChanged(SelectionChangedCallback cb)
{
    onSelectionChanged = std::move(cb);
}

void TemplateLibraryPanel::Render()
{
    if (!library || library->Count() == 0) {
        ImGui::TextDisabled("Aucun template dans la librairie");
        if (ImGui::Button("Ajouter les presets")) {
            if (library) {
                library->SeedPresets();
            }
        }
        return;
    }

    ImGui::Text("Templates: %zu", library->Count());

    const auto& names = library->GetNames();

    // Keep UI selection in sync with the library's active template.
    // This matters when the active template changes via menu actions (e.g. Load Template)
    // rather than through this panel's listbox interactions.
    if (auto* active = library->Get(library->GetActiveId())) {
        for (int i = 0; i < static_cast<int>(names.size()); ++i) {
            if (names[i] == active->name) {
                uiSelectedIndex = i;
                break;
            }
        }
    }
    if (uiSelectedIndex < 0) uiSelectedIndex = 0;
    if (uiSelectedIndex >= static_cast<int>(names.size())) uiSelectedIndex = static_cast<int>(names.size()) - 1;

    if (ImGui::BeginListBox("##templates", ImVec2(-FLT_MIN, 140))) {
        for (int i = 0; i < static_cast<int>(names.size()); ++i) {
            bool selected = (i == uiSelectedIndex);
            if (ImGui::Selectable(names[i].c_str(), selected)) {
                uiSelectedIndex = i;
                // Map UI index -> template ID
                // The library currently uses stable IDs but exposes names only; we rely on index ordering.
                // Keeping IDs and names aligned is an invariant of TemplateLibrary.
                int newActiveId = library->GetActiveId();
                // Find the id by scanning (cheap, small list)
                for (int candidateId = 1; candidateId < 100000; ++candidateId) {
                    const auto* t = library->Get(candidateId);
                    if (!t) continue;
                    // Compare pointer with expected by index
                }
                // Simpler: set active by index through names (unique per session) is acceptable for now.
                const std::string& pickedName = names[i];
                for (int candidateId = 1; candidateId < 100000; ++candidateId) {
                    auto* t = library->Get(candidateId);
                    if (t && t->name == pickedName) {
                        newActiveId = candidateId;
                        break;
                    }
                }
                library->SetActiveId(newActiveId);
                if (onSelectionChanged) {
                    onSelectionChanged(newActiveId);
                }
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Nouveau (preset Chrysanthemum)")) {
        int id = library->Add(std::make_unique<FireworkTemplate>(FireworkTemplate::Chrysanthemum()));
        library->SetActiveId(id);
        if (onSelectionChanged) {
            onSelectionChanged(id);
        }
    }
}

} // namespace panels
} // namespace ui
