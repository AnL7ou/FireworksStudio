#include "BranchLayoutGenerator.h"
#include <random>
#include <cmath>
#include <glm/glm.hpp>

static std::mt19937 s_rng(std::random_device{}());

void BranchLayoutGenerator::Generate(
    const BranchLayout& layout,
    std::vector<GeneratedBranch>& outBranches
)
{
    outBranches.clear();
    GenerateGrid(layout, outBranches);
}

void BranchLayoutGenerator::GenerateGrid(
    const BranchLayout& layout,
    std::vector<GeneratedBranch>& out
)
{
    const int n = layout.gridX * layout.gridY;
    if (n <= 0) return;

    out.reserve(n);

    // ═══════════════════════════════════════════════════════════
    // FIBONACCI SPHERE dans la zone contrainte
    // ═══════════════════════════════════════════════════════════
    const float PHI = (1.0f + std::sqrt(5.0f)) / 2.0f;
    const float PI = 3.14159265358979323846f;

    // Contraintes de zone
    const float azMin = layout.constraintAzimuthMin;
    const float azMax = layout.constraintAzimuthMax;
    const float elMin = layout.constraintElevationMin;
    const float elMax = layout.constraintElevationMax;

    // Convertir les limites d'elevation en coordonnées Y [-1, 1]
    float yMin = std::sin(glm::radians(elMin));
    float yMax = std::sin(glm::radians(elMax));

    // Convertir azimuth en radians
    float azMinRad = glm::radians(azMin);
    float azMaxRad = glm::radians(azMax);

    for (int i = 0; i < n; ++i) {
        // ═══════════════════════════════════════════════════════
        // Fibonacci dans l'intervalle Y contraint
        // ═══════════════════════════════════════════════════════
        // n can be 1 (e.g. grid 1x1). Avoid division by zero.
        float t = (n > 1) ? (static_cast<float>(i) / static_cast<float>(n - 1)) : 0.0f;

        // Y dans [yMin, yMax] au lieu de [-1, 1]
        float y = yMin + t * (yMax - yMin);

        // Rayon du cercle à cette hauteur
        float radius = std::sqrt(std::max(0.0f, 1.0f - y * y));

        // Angle theta avec offset pour la zone azimuth
        float theta = 2.0f * PI * i / PHI;

        // ═══════════════════════════════════════════════════════
        // Mapper theta dans la zone azimuth [azMin, azMax]
        // ═══════════════════════════════════════════════════════
        // Normaliser theta dans [0, 2π]
        while (theta < 0.0f) theta += 2.0f * PI;
        while (theta >= 2.0f * PI) theta -= 2.0f * PI;

        // Mapper vers la zone azimuth
        float azimuthSpan = azMaxRad - azMinRad;
        if (azimuthSpan < 2.0f * PI) {
            // Zone partielle : mapper proportionnellement
            float thetaNorm = theta / (2.0f * PI);  // [0, 1]
            theta = azMinRad + thetaNorm * azimuthSpan;
        }
        // Sinon zone complète (360°) : garder theta tel quel

        // ═══════════════════════════════════════════════════════
        // Coordonnées cartésiennes
        // ═══════════════════════════════════════════════════════
        float x = std::cos(theta) * radius;
        float z = std::sin(theta) * radius;

        glm::vec3 dir = glm::normalize(glm::vec3(x, y, z));

        // ═══════════════════════════════════════════════════════
        // Appliquer randomness (optionnel)
        // ═══════════════════════════════════════════════════════
        if (layout.randomness > 0.0f) {
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

            // Perturber légèrement
            glm::vec3 perturbation(
                dist(s_rng) * layout.randomness * 0.1f,
                dist(s_rng) * layout.randomness * 0.1f,
                dist(s_rng) * layout.randomness * 0.1f
            );

            dir = glm::normalize(dir + perturbation);
        }

        GeneratedBranch branch;
        branch.direction = dir;

        out.push_back(branch);
    }

    // ✅ On a exactement N branches, toutes dans la zone !
}