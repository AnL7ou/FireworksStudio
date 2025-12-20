#include "TemplatePropertyPanel.h"

TemplatePropertiesPanel::TemplatePropertiesPanel(FireworkTemplate* tmpl) noexcept
    : fireworkTemplate(tmpl)
    , fireworkTemplateSnapshot()
    , onTest(nullptr)
    , onChanged(nullptr)
    , shapeRegistry(nullptr)
{
    customPathBuf[0] = '\0';
    if (fireworkTemplate)
        takeSnapshot();
}

void TemplatePropertiesPanel::SetTemplate(FireworkTemplate* tmpl) noexcept
{
    fireworkTemplate = tmpl;
    if (fireworkTemplate)
        takeSnapshot();
}

void TemplatePropertiesPanel::SetOnExplosionTestCallback(ExplosionTestCallback cb) noexcept
{
    onTest = std::move(cb);
}

void TemplatePropertiesPanel::SetOnTemplateChangedCallback(TemplateChangedCallback cb) noexcept
{
    onChanged = std::move(cb);
}

void TemplatePropertiesPanel::SetShapeRegistry(ShapeRegistry* registry) noexcept
{
    shapeRegistry = registry;
}

void TemplatePropertiesPanel::takeSnapshot() noexcept
{
    if (fireworkTemplate)
        fireworkTemplateSnapshot = *fireworkTemplate;
}

bool TemplatePropertiesPanel::detectAndNotifyChanges() noexcept
{
    if (!fireworkTemplate)
        return false;

    if (fireworkTemplateSnapshot.name != fireworkTemplate->name
        || fireworkTemplateSnapshot.particlesCount != fireworkTemplate->particlesCount
        || fireworkTemplateSnapshot.particleLifeTime != fireworkTemplate->particleLifeTime
        || fireworkTemplateSnapshot.explosionRadius != fireworkTemplate->explosionRadius
        || fireworkTemplateSnapshot.baseColor != fireworkTemplate->baseColor
        || fireworkTemplateSnapshot.baseSize != fireworkTemplate->baseSize
        || fireworkTemplateSnapshot.speedMin != fireworkTemplate->speedMin
        || fireworkTemplateSnapshot.speedMax != fireworkTemplate->speedMax
        || fireworkTemplateSnapshot.particleShapeId != fireworkTemplate->particleShapeId
        || fireworkTemplateSnapshot.particleTextureLayerIndex != fireworkTemplate->particleTextureLayerIndex)
    {
        // Mettre à jour le snapshot puis appeler le callback si présent
        takeSnapshot();
        if (onChanged)
            onChanged(*fireworkTemplate);
        return true;
    }
    return false;
}

void TemplatePropertiesPanel::Render() noexcept
{
    if (!fireworkTemplate)
    {
        ImGui::TextDisabled("Aucun template sélectionné");
        return;
    }

    // Name (ImGui InputText nécessite un buffer)
    char nameBuf[256];
    std::strncpy(nameBuf, fireworkTemplate->name.c_str(), sizeof(nameBuf) - 1);
    nameBuf[sizeof(nameBuf) - 1] = '\0';
    if (ImGui::InputText("Nom", nameBuf, sizeof(nameBuf)))
    {
        fireworkTemplate->name = std::string(nameBuf);
    }

    // Particles count
    int count = static_cast<int>(fireworkTemplate->particlesCount);
    if (ImGui::DragInt("Nombre de particules", &count, 1.0f, 1, 100000))
    {
        if (count < 1) count = 1;
        fireworkTemplate->particlesCount = static_cast<unsigned int>(count);
    }

    // Lifetime
    float life = fireworkTemplate->particleLifeTime;
    if (ImGui::DragFloat("Durée de vie (s)", &life, 0.01f, 0.01f, 100.0f, "%.2f"))
    {
        if (life < 0.01f) life = 0.01f;
        fireworkTemplate->particleLifeTime = life;
    }

    // Explosion radius
    float radius = fireworkTemplate->explosionRadius;
    if (ImGui::DragFloat("Rayon explosion", &radius, 0.001f, 0.0f, 100.0f, "%.3f"))
    {
        if (radius < 0.0f) radius = 0.0f;
        fireworkTemplate->explosionRadius = radius;
    }

    // Base size
    float bsize = fireworkTemplate->baseSize;
    if (ImGui::DragFloat("Taille particule", &bsize, 0.01f, 0.0f, 100.0f, "%.3f"))
    {
        if (bsize < 0.0f) bsize = 0.0f;
        fireworkTemplate->baseSize = bsize;
    }

    // Speed min/max
    float smin = fireworkTemplate->speedMin;
    float smax = fireworkTemplate->speedMax;
    if (ImGui::DragFloatRange2("Vitesse min / max", &smin, &smax, 0.01f, 0.0f, 1000.0f, "min: %.2f", "max: %.2f"))
    {
        if (smin < 0.0f) smin = 0.0f;
        if (smax < smin) smax = smin;
        fireworkTemplate->speedMin = smin;
        fireworkTemplate->speedMax = smax;
    }

    // Color (glm::vec4 -> float[4])
    float col[4] = { fireworkTemplate->baseColor.r, fireworkTemplate->baseColor.g, fireworkTemplate->baseColor.b, fireworkTemplate->baseColor.a };
    if (ImGui::ColorEdit4("Couleur", col))
    {
        fireworkTemplate->baseColor = glm::vec4(col[0], col[1], col[2], col[3]);
    }

    // ---------- Apparence des particules ----------
    ImGui::Separator();
    ImGui::Text("Apparence");

    // Builtin shapes combo (utilise BuiltinShape enum)
    const char* builtinNames[] = { "Disk", "Rice", "Star", "Spark", "Flower" };
    // Affiche le builtin sélectionné si possible
    int selectedBuiltin = -1;
    // Si le registry est disponible et que particleShapeId est dans la plage builtin, on mappe
    if (shapeRegistry)
    {
        uint16_t curId = fireworkTemplate->particleShapeId;
        if (curId < static_cast<uint16_t>(BuiltinShape::Count))
            selectedBuiltin = static_cast<int>(curId);
    }
    else
    {
        // si pas de registry, tenter d'interpréter l'id comme builtin (sécurité)
        uint16_t curId = fireworkTemplate->particleShapeId;
        if (curId < static_cast<uint16_t>(BuiltinShape::Count))
            selectedBuiltin = static_cast<int>(curId);
    }

    if (selectedBuiltin < 0) selectedBuiltin = 0;

    if (ImGui::Combo("Forme builtin", &selectedBuiltin, builtinNames, IM_ARRAYSIZE(builtinNames)))
    {
        // enregistrer le builtin dans le registry (ou écrire l'index directement si registry absent)
        if (shapeRegistry)
        {
            uint16_t id = shapeRegistry->RegisterBuiltin(static_cast<BuiltinShape>(selectedBuiltin));
            fireworkTemplate->particleShapeId = id;
        }
        else
        {
            fireworkTemplate->particleShapeId = static_cast<uint16_t>(selectedBuiltin);
        }
    }

    // Affiche le nom de la shape sélectionnée (via registry si dispo)
    if (shapeRegistry)
    {
        const Shape& s = shapeRegistry->GetShape(fireworkTemplate->particleShapeId);
        ImGui::Text("Shape actuelle : %s", s.displayName().c_str());
    }
    else
    {
        ImGui::Text("Shape actuelle ID : %u", static_cast<unsigned int>(fireworkTemplate->particleShapeId));
    }

    // Custom shape : chemin + bouton d'enregistrement
    ImGui::InputText("Chemin shape custom", customPathBuf, sizeof(customPathBuf));
    ImGui::SameLine();
    if (ImGui::Button("Charger shape"))
    {
        if (customPathBuf[0] != '\0')
        {
            if (shapeRegistry)
            {
                uint16_t id = shapeRegistry->RegisterCustom(std::string(customPathBuf));
                fireworkTemplate->particleShapeId = id;
            }
            else
            {
                std::cout << "[TemplatePropertiesPanel] Pas de ShapeRegistry assigné, impossible d'enregistrer custom shape\n";
            }
        }
    }

    int textureLayer = fireworkTemplate->particleTextureLayerIndex;
    if (ImGui::InputInt("Texture layer index", &textureLayer))
    {
        fireworkTemplate->particleTextureLayerIndex = textureLayer;
    }

    ImGui::Separator();

    // Test Explosion button
    if (ImGui::Button("Test Explosion"))
    {
        if (onTest)
        {
            onTest(*fireworkTemplate);
        }
        else
        {
            // Debug fallback
            std::cout << "[TemplatePropertiesPanel] Test Explosion pressed but no callback bound\n";
        }
    }

    // Détection des changements et notification
    detectAndNotifyChanges();
}