#include "ColorSchemePanel.h"
#include <imgui.h>

namespace ui {
    namespace panels {
        ColorSchemePanel::ColorSchemePanel(ColorScheme* scheme) noexcept
            : scheme(scheme)
            , snapshot()
            , onChanged(nullptr)
        {
            if (scheme) {
                TakeSnapshot();
            }
        }

        void ColorSchemePanel::SetColorScheme(ColorScheme* scheme) noexcept
        {
            this->scheme = scheme;
            if (scheme) {
                TakeSnapshot();
            }
        }

        void ColorSchemePanel::SetOnColorSchemeChangedCallback(ColorSchemeChangedCallback cb) noexcept
        {
            onChanged = std::move(cb);
        }

        void ColorSchemePanel::TakeSnapshot() noexcept
        {
            if (scheme) {
                snapshot = *scheme;
            }
        }

        bool ColorSchemePanel::DetectAndNotifyChanges() noexcept
        {
            if (!scheme) return false;

            // Comparaison simplifiée
            bool changed = (snapshot.type != scheme->type)
                || (snapshot.uniformColor != scheme->uniformColor)
                || (snapshot.gradientStart != scheme->gradientStart)
                || (snapshot.gradientEnd != scheme->gradientEnd)
                || (snapshot.palette.size() != scheme->palette.size())
                || (snapshot.fadeOverTime != scheme->fadeOverTime);

            if (changed) {
                TakeSnapshot();
                if (onChanged) {
                    onChanged(*scheme);
                }
                return true;
            }

            return false;
        }

        void ColorSchemePanel::Render() noexcept
        {
            if (!scheme) {
                ImGui::TextDisabled("Aucun color scheme sélectionné");
                return;
            }

            // Type selector
            const char* typeNames[] = { "Uniform", "Gradient", "Radial", "Random", "Per Branch", "Alternating" };
            int currentType = static_cast<int>(scheme->type);

            if (ImGui::Combo("Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames))) {
                scheme->type = static_cast<ColorDistributionType>(currentType);
            }

            ImGui::Separator();

            // Settings spécifiques au type
            switch (scheme->type) {
            case ColorDistributionType::Uniform:
                RenderUniformSettings();
                break;
            case ColorDistributionType::Gradient:
            case ColorDistributionType::Radial:
                RenderGradientSettings();
                break;
            case ColorDistributionType::Random:
            case ColorDistributionType::Alternating:
                RenderPaletteSettings();
                break;
            case ColorDistributionType::PerBranch:
                ImGui::TextWrapped("Mode Per Branch : définissez les couleurs manuellement pour chaque branche");
                break;
            }

            ImGui::Separator();

            // Options communes
            ImGui::Text("Options communes");

            ImGui::SliderFloat("Saturation variance", &scheme->saturationVariance, 0.0f, 1.0f);
            ImGui::SliderFloat("Brightness variance", &scheme->brightnessVariance, 0.0f, 1.0f);

            ImGui::Separator();

            // Fade settings
            ImGui::Checkbox("Fade over time", &scheme->fadeOverTime);
            if (scheme->fadeOverTime) {
                ImGui::SliderFloat("Fade start ratio", &scheme->fadeStartRatio, 0.0f, 1.0f);
                ImGui::TextDisabled("Le fade commence quand la particule atteint ce % de vie");
            }

            // Détecter changements
            DetectAndNotifyChanges();
        }

        void ColorSchemePanel::RenderUniformSettings() noexcept
        {
            ImGui::Text("Uniform Color");

            float color[4] = {
                scheme->uniformColor.r,
                scheme->uniformColor.g,
                scheme->uniformColor.b,
                scheme->uniformColor.a
            };

            if (ImGui::ColorEdit4("Color", color)) {
                scheme->uniformColor = glm::vec4(color[0], color[1], color[2], color[3]);
            }
        }

        void ColorSchemePanel::RenderGradientSettings() noexcept
        {
            ImGui::Text(scheme->type == ColorDistributionType::Gradient ? "Gradient Settings" : "Radial Settings");

            float start[4] = {
                scheme->gradientStart.r,
                scheme->gradientStart.g,
                scheme->gradientStart.b,
                scheme->gradientStart.a
            };

            float end[4] = {
                scheme->gradientEnd.r,
                scheme->gradientEnd.g,
                scheme->gradientEnd.b,
                scheme->gradientEnd.a
            };

            if (ImGui::ColorEdit4("Start Color", start)) {
                scheme->gradientStart = glm::vec4(start[0], start[1], start[2], start[3]);
            }

            if (ImGui::ColorEdit4("End Color", end)) {
                scheme->gradientEnd = glm::vec4(end[0], end[1], end[2], end[3]);
            }

            if (scheme->type == ColorDistributionType::Radial) {
                ImGui::TextDisabled("Centre = Start Color, Bord = End Color");
            }
        }

        void ColorSchemePanel::RenderPaletteSettings() noexcept
        {
            ImGui::Text("Palette");

            // Afficher les couleurs existantes
            for (size_t i = 0; i < scheme->palette.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));

                float color[4] = {
                    scheme->palette[i].r,
                    scheme->palette[i].g,
                    scheme->palette[i].b,
                    scheme->palette[i].a
                };

                char label[32];
                snprintf(label, sizeof(label), "Color %zu", i + 1);

                if (ImGui::ColorEdit4(label, color)) {
                    scheme->palette[i] = glm::vec4(color[0], color[1], color[2], color[3]);
                }

                ImGui::SameLine();
                if (ImGui::Button("X")) {
                    scheme->palette.erase(scheme->palette.begin() + i);
                    ImGui::PopID();
                    break;
                }

                ImGui::PopID();
            }

            // Bouton ajouter couleur
            if (ImGui::Button("+ Add Color")) {
                scheme->palette.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            }

            if (scheme->type == ColorDistributionType::Random) {
                ImGui::TextDisabled("Chaque branche tire une couleur aléatoire de la palette");
            }
            else {
                ImGui::TextDisabled("Les branches alternent entre les couleurs de la palette");
            }
        }
    }
}