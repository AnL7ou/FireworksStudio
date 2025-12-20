#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <string>
#include <imgui.h>
#include <cstring>
#include <iostream>

#include "../../fireworks/FireworkTemplate.h"
#include "../../fireworks/ShapeRegistry.h"
#include "../../fireworks/Shape.h"

class ShapeRegistry;

class TemplatePropertiesPanel {
public:
    using ExplosionTestCallback = std::function<void(const FireworkTemplate&)>;
    using TemplateChangedCallback = std::function<void(const FireworkTemplate&)>;

    TemplatePropertiesPanel(FireworkTemplate* tmpl) noexcept;
    ~TemplatePropertiesPanel() = default;

    void SetTemplate(FireworkTemplate* tmpl) noexcept;
    void SetOnExplosionTestCallback(ExplosionTestCallback cb) noexcept;
    void SetOnTemplateChangedCallback(TemplateChangedCallback cb) noexcept;

    // Permet d'associer le registry utilisé pour enregistrer/obtenir des shapes
    void SetShapeRegistry(ShapeRegistry* registry) noexcept;

    void Render() noexcept;

private:
    void takeSnapshot() noexcept;
    bool detectAndNotifyChanges() noexcept;

private:
    FireworkTemplate* fireworkTemplate;
    FireworkTemplate fireworkTemplateSnapshot;

    ExplosionTestCallback onTest;
    TemplateChangedCallback onChanged;

    ShapeRegistry* shapeRegistry;

    // buffer pour chemin custom
    char customPathBuf[512];
};