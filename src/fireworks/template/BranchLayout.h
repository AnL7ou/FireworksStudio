#pragma once

#include <glm/glm.hpp>

struct BranchLayout {
    // ═══════════════════════════════════════════════════════════
    // GRID PARAMETERS (uniquement Grid maintenant)
    // ═══════════════════════════════════════════════════════════
    int gridX;                    // Nombre de branches horizontales
    int gridY;                    // Nombre de branches verticales

    // ═══════════════════════════════════════════════════════════
    // CONTRAINTES (copiées depuis FireworkTemplate)
    // ═══════════════════════════════════════════════════════════
    float constraintAzimuthMin;      // Azimuth min (degrés)
    float constraintAzimuthMax;      // Azimuth max (degrés)
    float constraintElevationMin;    // Elevation min (degrés)
    float constraintElevationMax;    // Elevation max (degrés)

    // ═══════════════════════════════════════════════════════════
    // OPTIONS COMMUNES
    // ═══════════════════════════════════════════════════════════
    bool staggered;               // Décalage alterné (quinconce)
    float randomness;             // Variance aléatoire (0-1)

    BranchLayout()
        : gridX(8)
        , gridY(8)
        , constraintAzimuthMin(-180.0f)
        , constraintAzimuthMax(180.0f)
        , constraintElevationMin(0.0f)
        , constraintElevationMax(90.0f)
        , staggered(false)
        , randomness(0.0f)
    {
    }

    int GetTotalBranchCount() const {
        return gridX * gridY;
    }
};