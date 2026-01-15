#include "TemplatePropertiesPanel.h"

#include <cstring>
#include <iostream>

#include <imgui.h>

#include "../../../fireworks/template/BranchDescriptor.h"

namespace ui {
namespace panels {

TemplatePropertiesPanel::TemplatePropertiesPanel(FireworkTemplate* tmpl) noexcept
    : fireworkTemplate(tmpl)
    , fireworkTemplateSnapshot()
    , onTest(nullptr)
    , onChanged(nullptr)
{
    if (fireworkTemplate) {
        takeSnapshot();
    }
}

void TemplatePropertiesPanel::SetTemplate(FireworkTemplate* tmpl) noexcept
{
    fireworkTemplate = tmpl;
    if (fireworkTemplate) {
        takeSnapshot();
    }
}

void TemplatePropertiesPanel::SetOnExplosionTestCallback(ExplosionTestCallback cb) noexcept
{
    onTest = std::move(cb);
}

void TemplatePropertiesPanel::SetOnTemplateChangedCallback(TemplateChangedCallback cb) noexcept
{
    onChanged = std::move(cb);
}

void TemplatePropertiesPanel::takeSnapshot() noexcept
{
    if (fireworkTemplate) {
        fireworkTemplateSnapshot = *fireworkTemplate;
    }
}

bool TemplatePropertiesPanel::detectAndNotifyChanges() noexcept
{
    if (!fireworkTemplate) return false;

    bool changed = false;

    // Zone
    changed |= (fireworkTemplateSnapshot.zoneAzimuthMin != fireworkTemplate->zoneAzimuthMin);
    changed |= (fireworkTemplateSnapshot.zoneAzimuthMax != fireworkTemplate->zoneAzimuthMax);
    changed |= (fireworkTemplateSnapshot.zoneElevationMin != fireworkTemplate->zoneElevationMin);
    changed |= (fireworkTemplateSnapshot.zoneElevationMax != fireworkTemplate->zoneElevationMax);

    // Orientation
    changed |= (fireworkTemplateSnapshot.worldRotation != fireworkTemplate->worldRotation);

    // Grid
    changed |= (fireworkTemplateSnapshot.layout.gridX != fireworkTemplate->layout.gridX);
    changed |= (fireworkTemplateSnapshot.layout.gridY != fireworkTemplate->layout.gridY);
    changed |= (fireworkTemplateSnapshot.layout.staggered != fireworkTemplate->layout.staggered);
    changed |= (fireworkTemplateSnapshot.layout.randomness != fireworkTemplate->layout.randomness);

    // Branch physics
    changed |= (fireworkTemplateSnapshot.branchTemplate.initialSpeed != fireworkTemplate->branchTemplate.initialSpeed);
    changed |= (fireworkTemplateSnapshot.branchTemplate.speedVariance != fireworkTemplate->branchTemplate.speedVariance);
    changed |= (fireworkTemplateSnapshot.branchTemplate.damping != fireworkTemplate->branchTemplate.damping);
    changed |= (fireworkTemplateSnapshot.branchTemplate.dampingVariance != fireworkTemplate->branchTemplate.dampingVariance);
    changed |= (fireworkTemplateSnapshot.branchTemplate.gravityScale != fireworkTemplate->branchTemplate.gravityScale);
    changed |= (fireworkTemplateSnapshot.branchTemplate.updraft != fireworkTemplate->branchTemplate.updraft);

    // Visual / counts
    changed |= (fireworkTemplateSnapshot.branchTemplate.particlesPerBranch != fireworkTemplate->branchTemplate.particlesPerBranch);
    changed |= (fireworkTemplateSnapshot.branchTemplate.emissionDuration != fireworkTemplate->branchTemplate.emissionDuration);
    changed |= (fireworkTemplateSnapshot.branchTemplate.particleSize != fireworkTemplate->branchTemplate.particleSize);
    changed |= (fireworkTemplateSnapshot.branchTemplate.sizeVariance != fireworkTemplate->branchTemplate.sizeVariance);
    changed |= (fireworkTemplateSnapshot.branchTemplate.lifetime != fireworkTemplate->branchTemplate.lifetime);
    changed |= (fireworkTemplateSnapshot.branchTemplate.shapeId != fireworkTemplate->branchTemplate.shapeId);
    changed |= (fireworkTemplateSnapshot.branchTemplate.angularSpread != fireworkTemplate->branchTemplate.angularSpread);

    // Fade
    changed |= (fireworkTemplateSnapshot.branchTemplate.shouldFade != fireworkTemplate->branchTemplate.shouldFade);
    changed |= (fireworkTemplateSnapshot.branchTemplate.fadeStartRatio != fireworkTemplate->branchTemplate.fadeStartRatio);

    // Visual family / trails
    changed |= (fireworkTemplateSnapshot.branchTemplate.visualMode != fireworkTemplate->branchTemplate.visualMode);
    changed |= (fireworkTemplateSnapshot.branchTemplate.trailEnabled != fireworkTemplate->branchTemplate.trailEnabled);
    changed |= (fireworkTemplateSnapshot.branchTemplate.trailWidth != fireworkTemplate->branchTemplate.trailWidth);
    changed |= (fireworkTemplateSnapshot.branchTemplate.trailDuration != fireworkTemplate->branchTemplate.trailDuration);
    changed |= (fireworkTemplateSnapshot.branchTemplate.trailOpacity != fireworkTemplate->branchTemplate.trailOpacity);
    changed |= (fireworkTemplateSnapshot.branchTemplate.trailFalloffPow != fireworkTemplate->branchTemplate.trailFalloffPow);

    // Front density & sparkle tweaks
    changed |= (fireworkTemplateSnapshot.branchTemplate.frontPortion != fireworkTemplate->branchTemplate.frontPortion);
    changed |= (fireworkTemplateSnapshot.branchTemplate.frontSpeedBias != fireworkTemplate->branchTemplate.frontSpeedBias);
    changed |= (fireworkTemplateSnapshot.branchTemplate.backSpeedScale != fireworkTemplate->branchTemplate.backSpeedScale);
    changed |= (fireworkTemplateSnapshot.branchTemplate.sparkleSpeedJitter != fireworkTemplate->branchTemplate.sparkleSpeedJitter);
    changed |= (fireworkTemplateSnapshot.branchTemplate.sparkleSpreadMult != fireworkTemplate->branchTemplate.sparkleSpreadMult);

    // Extensions (even if runtime not wired yet, UI should be deterministic)
    changed |= (fireworkTemplateSnapshot.branchTemplate.smokeAmount != fireworkTemplate->branchTemplate.smokeAmount);
    changed |= (fireworkTemplateSnapshot.branchTemplate.recursionDepth != fireworkTemplate->branchTemplate.recursionDepth);
    changed |= (fireworkTemplateSnapshot.branchTemplate.recursionProb != fireworkTemplate->branchTemplate.recursionProb);

    if (!changed) return false;

    takeSnapshot();
    fireworkTemplate->RegenerateBranches();

    if (onChanged) {
        onChanged(*fireworkTemplate);
    }
    return true;
}

void TemplatePropertiesPanel::Render() noexcept
{
    if (!fireworkTemplate) {
        ImGui::TextDisabled("Aucun template sélectionné");
        return;
    }

    // Name (always visible)
    char nameBuf[256];
    std::strncpy(nameBuf, fireworkTemplate->name.c_str(), sizeof(nameBuf) - 1);
    nameBuf[sizeof(nameBuf) - 1] = '\0';
    if (ImGui::InputText("Nom", nameBuf, sizeof(nameBuf))) {
        fireworkTemplate->name = std::string(nameBuf);
    }

    ImGui::Separator();

    if (ImGui::BeginTabBar("##templateTabs"))
    {
        if (ImGui::BeginTabItem("Général"))
        {
            ImGui::Text("Zone d'émission");
            ImGui::SliderFloat("Azimuth min", &fireworkTemplate->zoneAzimuthMin, -180.0f, 180.0f, "%.1f°");
            ImGui::SliderFloat("Azimuth max", &fireworkTemplate->zoneAzimuthMax, -180.0f, 180.0f, "%.1f°");
            ImGui::SliderFloat("Elevation min", &fireworkTemplate->zoneElevationMin, -90.0f, 90.0f, "%.1f°");
            ImGui::SliderFloat("Elevation max", &fireworkTemplate->zoneElevationMax, -90.0f, 90.0f, "%.1f°");

            ImGui::Separator();

            ImGui::Text("Rotation globale");
            ImGui::TextDisabled("Astuce viewport: LMB drag = free, Shift = X, Ctrl = Y, Shift+Ctrl = Z");
            ImGui::SliderFloat("Pitch (X)", &fireworkTemplate->worldRotation.x, -180.0f, 180.0f, "%.1f°");
            ImGui::SliderFloat("Yaw (Y)", &fireworkTemplate->worldRotation.y, -180.0f, 180.0f, "%.1f°");
            ImGui::SliderFloat("Roll (Z)", &fireworkTemplate->worldRotation.z, -180.0f, 180.0f, "%.1f°");

            ImGui::Separator();

            ImGui::Text("Particules");
            ImGui::DragInt("Particules/branche", &fireworkTemplate->branchTemplate.particlesPerBranch, 1.0f, 1, 5000);
            if (fireworkTemplate->branchTemplate.particlesPerBranch < 1) fireworkTemplate->branchTemplate.particlesPerBranch = 1;

            ImGui::DragFloat("Emission duration (s)", &fireworkTemplate->branchTemplate.emissionDuration, 0.01f, 0.0f, 5.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.emissionDuration < 0.0f) fireworkTemplate->branchTemplate.emissionDuration = 0.0f;

            ImGui::DragFloat("Taille particule", &fireworkTemplate->branchTemplate.particleSize, 0.1f, 0.1f, 50.0f, "%.1f");
            if (fireworkTemplate->branchTemplate.particleSize < 0.1f) fireworkTemplate->branchTemplate.particleSize = 0.1f;

            ImGui::SliderFloat("Variance taille", &fireworkTemplate->branchTemplate.sizeVariance, 0.0f, 1.0f, "%.2f");

            ImGui::DragFloat("Lifetime (s)", &fireworkTemplate->branchTemplate.lifetime, 0.05f, 0.05f, 20.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.lifetime < 0.05f) fireworkTemplate->branchTemplate.lifetime = 0.05f;

            {
                int shape = static_cast<int>(fireworkTemplate->branchTemplate.shapeId);
                ImGui::DragInt("ShapeId", &shape, 1.0f, 0, 1024);
                if (shape < 0) shape = 0;
                if (shape > 65535) shape = 65535;
                fireworkTemplate->branchTemplate.shapeId = static_cast<uint16_t>(shape);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Physique"))
        {
            ImGui::Text("Grille de branches");
            ImGui::DragInt("Branches X", &fireworkTemplate->layout.gridX, 0.5f, 1, 100);
            if (fireworkTemplate->layout.gridX < 1) fireworkTemplate->layout.gridX = 1;
            ImGui::DragInt("Branches Y", &fireworkTemplate->layout.gridY, 0.5f, 1, 100);
            if (fireworkTemplate->layout.gridY < 1) fireworkTemplate->layout.gridY = 1;
            ImGui::Checkbox("Quinconce", &fireworkTemplate->layout.staggered);
            ImGui::SliderFloat("Randomness", &fireworkTemplate->layout.randomness, 0.0f, 1.0f, "%.2f");

            ImGui::Separator();

            ImGui::Text("Paramètres des branches");
            ImGui::DragFloat("Vitesse initiale (m/s)", &fireworkTemplate->branchTemplate.initialSpeed, 0.1f, 0.0f, 250.0f, "%.1f");
            if (fireworkTemplate->branchTemplate.initialSpeed < 0.0f) fireworkTemplate->branchTemplate.initialSpeed = 0.0f;
            ImGui::SliderFloat("Variance vitesse", &fireworkTemplate->branchTemplate.speedVariance, 0.0f, 1.0f, "%.2f");

            ImGui::DragFloat("Damping (1/s)", &fireworkTemplate->branchTemplate.damping, 0.05f, 0.0f, 30.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.damping < 0.0f) fireworkTemplate->branchTemplate.damping = 0.0f;
            ImGui::SliderFloat("Variance damping", &fireworkTemplate->branchTemplate.dampingVariance, 0.0f, 1.0f, "%.2f");

            ImGui::SliderFloat("Gravity scale", &fireworkTemplate->branchTemplate.gravityScale, 0.0f, 1.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.gravityScale < 0.0f) fireworkTemplate->branchTemplate.gravityScale = 0.0f;
            if (fireworkTemplate->branchTemplate.gravityScale > 1.0f) fireworkTemplate->branchTemplate.gravityScale = 1.0f;
            ImGui::DragFloat("Updraft (m/s^2)", &fireworkTemplate->branchTemplate.updraft, 0.05f, -5.0f, 5.0f, "%.2f");

            ImGui::DragFloat("Spread angulaire (°)", &fireworkTemplate->branchTemplate.angularSpread, 0.1f, 0.0f, 90.0f, "%.1f");
            if (fireworkTemplate->branchTemplate.angularSpread < 0.0f) fireworkTemplate->branchTemplate.angularSpread = 0.0f;

            {
                int mode = static_cast<int>(fireworkTemplate->branchTemplate.visualMode);
                const char* modes[] = { "Comet", "Sparkle" };
                ImGui::Combo("Visual mode", &mode, modes, IM_ARRAYSIZE(modes));
                if (mode < 0) mode = 0;
                if (mode > 1) mode = 1;
                fireworkTemplate->branchTemplate.visualMode = static_cast<BranchDescriptor::VisualMode>(mode);
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Trails"))
        {
            // Fade
            ImGui::Text("Fade");
            ImGui::Checkbox("Fade", &fireworkTemplate->branchTemplate.shouldFade);
            ImGui::SliderFloat("Fade start ratio", &fireworkTemplate->branchTemplate.fadeStartRatio, 0.0f, 1.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.fadeStartRatio < 0.0f) fireworkTemplate->branchTemplate.fadeStartRatio = 0.0f;
            if (fireworkTemplate->branchTemplate.fadeStartRatio > 1.0f) fireworkTemplate->branchTemplate.fadeStartRatio = 1.0f;

            ImGui::Separator();

            ImGui::Text("Trails");
            ImGui::Checkbox("Trail", &fireworkTemplate->branchTemplate.trailEnabled);
            ImGui::DragFloat("Trail width", &fireworkTemplate->branchTemplate.trailWidth, 0.1f, 0.0f, 50.0f, "%.1f");
            if (fireworkTemplate->branchTemplate.trailWidth < 0.0f) fireworkTemplate->branchTemplate.trailWidth = 0.0f;
            ImGui::DragFloat("Trail duration", &fireworkTemplate->branchTemplate.trailDuration, 0.05f, 0.0f, 20.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.trailDuration < 0.0f) fireworkTemplate->branchTemplate.trailDuration = 0.0f;
            ImGui::SliderFloat("Trail opacity", &fireworkTemplate->branchTemplate.trailOpacity, 0.0f, 1.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.trailOpacity < 0.0f) fireworkTemplate->branchTemplate.trailOpacity = 0.0f;
            if (fireworkTemplate->branchTemplate.trailOpacity > 1.0f) fireworkTemplate->branchTemplate.trailOpacity = 1.0f;
            ImGui::DragFloat("Trail falloff", &fireworkTemplate->branchTemplate.trailFalloffPow, 0.05f, 1.0f, 8.0f, "%.2f");
            if (fireworkTemplate->branchTemplate.trailFalloffPow < 1.0f) fireworkTemplate->branchTemplate.trailFalloffPow = 1.0f;

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Style"))
        {
            ImGui::Text("Densité / style");
            ImGui::SliderFloat("Front portion", &fireworkTemplate->branchTemplate.frontPortion, 0.0f, 1.0f, "%.2f");
            ImGui::DragFloat("Front speed bias", &fireworkTemplate->branchTemplate.frontSpeedBias, 0.1f, 1.0f, 32.0f, "%.2f");
            ImGui::SliderFloat("Back speed scale", &fireworkTemplate->branchTemplate.backSpeedScale, 0.0f, 1.0f, "%.2f");

            if (fireworkTemplate->branchTemplate.visualMode == BranchDescriptor::VisualMode::Sparkle) {
                ImGui::Separator();
                ImGui::Text("Sparkle");
                ImGui::SliderFloat("Sparkle speed jitter", &fireworkTemplate->branchTemplate.sparkleSpeedJitter, 0.0f, 1.0f, "%.2f");
                ImGui::DragFloat("Sparkle spread mult", &fireworkTemplate->branchTemplate.sparkleSpreadMult, 0.05f, 0.0f, 10.0f, "%.2f");
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Extensions"))
        {
            ImGui::Text("Extensions (WIP)");
            ImGui::SliderFloat("Smoke amount", &fireworkTemplate->branchTemplate.smokeAmount, 0.0f, 1.0f, "%.2f");
            ImGui::DragInt("Recursion depth", &fireworkTemplate->branchTemplate.recursionDepth, 1.0f, 0, 8);
            ImGui::SliderFloat("Recursion prob", &fireworkTemplate->branchTemplate.recursionProb, 0.0f, 1.0f, "%.2f");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Stats"))
        {
            ImGui::Text("Aides test / lecture");
            const float g = 9.81f;
            float k = fireworkTemplate->branchTemplate.damping;
            if (k > 0.0f) {
                float tHalf = 0.69314718f / k;
                float vTerm = g / k;
                ImGui::Text("t1/2 vitesse: %.2f s", tHalf);
                ImGui::Text("Vitesse terminale (Y): %.1f m/s", vTerm);
            } else {
                ImGui::Text("t1/2 vitesse: inf");
                ImGui::Text("Vitesse terminale (Y): inf");
            }

            ImGui::Separator();

            ImGui::Text("Statistiques");
            ImGui::Text("Branches générées: %zu", fireworkTemplate->GetBranchCount());
            ImGui::Text("Particules totales: %d", fireworkTemplate->GetTotalParticleCount());

            ImGui::Separator();

            if (ImGui::Button("Test Explosion")) {
                if (onTest) {
                    onTest(*fireworkTemplate);
                } else {
                    std::cout << "[TemplatePropertiesPanel] Test Explosion: no callback\n";
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    (void)detectAndNotifyChanges();
}

} // namespace panels
} // namespace ui
