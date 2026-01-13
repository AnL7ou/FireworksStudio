#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "../template/ColorScheme.h"
#include "../template/GeneratedBranch.h"
#include "../template/BranchLayout.h"

// Évaluateur de couleurs : calcule la couleur de chaque branche selon le ColorScheme
class ColorSchemeEvaluator {
public:
    // Applique le color scheme à toutes les branches
    // Nécessite que les branches aient déjà leurs directions définies (pour Radial)
    static void ApplyColors(
        const ColorScheme& scheme,
        const BranchLayout& layout,  // Pour info spatiale (Grid position, etc.)
        std::vector<GeneratedBranch>& branches
    );

private:
    // Calcule la couleur d'une branche spécifique
    static glm::vec4 ComputeColor(
        const ColorScheme& scheme,
        int branchIndex,
        int totalBranches,
        int gridX,      // Position dans la grille (si applicable)
        int gridY,
        const glm::vec3& direction  // Pour Radial
    );

    // Applique les variances de saturation/luminosité
    static glm::vec4 ApplyVariances(
        glm::vec4 baseColor,
        float saturationVariance,
        float brightnessVariance
    );

    // Utilitaires HSV
    static glm::vec3 RGBtoHSV(const glm::vec3& rgb);
    static glm::vec3 HSVtoRGB(const glm::vec3& hsv);
};