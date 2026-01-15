#include "LayoutEditorPanel.h"
#include <imgui.h>

namespace ui {
    namespace panels {

        LayoutEditorPanel::LayoutEditorPanel(BranchLayout* layout) noexcept
            : layout(layout)
            , snapshot()
            , onChanged(nullptr)
        {
            if (layout) {
                TakeSnapshot();
            }
        }

        void LayoutEditorPanel::SetLayout(BranchLayout* layout) noexcept
        {
            this->layout = layout;
            if (layout) {
                TakeSnapshot();
            }
        }

        void LayoutEditorPanel::SetOnLayoutChangedCallback(LayoutChangedCallback cb) noexcept
        {
            onChanged = std::move(cb);
        }

        void LayoutEditorPanel::TakeSnapshot() noexcept
        {
            if (layout) {
                snapshot = *layout;
            }
        }

        bool LayoutEditorPanel::DetectAndNotifyChanges() noexcept
        {
            if (!layout) return false;

            bool changed = (snapshot.gridX != layout->gridX)
                || (snapshot.gridY != layout->gridY)
                || (snapshot.staggered != layout->staggered)
                || (snapshot.randomness != layout->randomness);

            if (changed) {
                TakeSnapshot();
                if (onChanged) {
                    onChanged(*layout);
                }
                return true;
            }

            return false;
        }

        void LayoutEditorPanel::Render() noexcept
        {
            if (!layout) {
                ImGui::TextDisabled("Aucun layout sélectionné");
                return;
            }

            ImGui::Text("Layout: Grid uniquement");
            ImGui::Separator();

            // ═══════════════════════════════════════════════════════════
            // GRID PARAMETERS
            // ═══════════════════════════════════════════════════════════
            ImGui::Text("Grille de branches");

            ImGui::DragInt("Branches X", &layout->gridX, 0.5f, 1, 100);
            if (layout->gridX < 1) layout->gridX = 1;

            ImGui::DragInt("Branches Y", &layout->gridY, 0.5f, 1, 100);
            if (layout->gridY < 1) layout->gridY = 1;

            ImGui::Text("Total: %d branches", layout->GetTotalBranchCount());

            ImGui::Separator();

            // ═══════════════════════════════════════════════════════════
            // OPTIONS
            // ═══════════════════════════════════════════════════════════
            ImGui::Text("Options");

            ImGui::Checkbox("Quinconce", &layout->staggered);
            ImGui::TextDisabled("Décalage alterné des lignes");

            ImGui::SliderFloat("Randomness", &layout->randomness, 0.0f, 1.0f, "%.2f");
            ImGui::TextDisabled("Variance aléatoire des positions");

            // Détection des changements
            DetectAndNotifyChanges();
        }

    } // namespace panels
} // namespace ui