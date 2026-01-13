#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "PhysicsProfile.h"
#include "BranchLayout.h"
#include "ColorScheme.h"
#include "BranchDescriptor.h"
#include "GeneratedBranch.h"

class FireworkTemplate {
public:
    std::string name;

    // ═══════════════════════════════════════════════════════════
    // ZONE D'ÉVENTAIL (angles sur la sphère)
    // ═══════════════════════════════════════════════════════════
    float zoneAzimuthMin;      // Azimuth minimum (degrés) [-180, 180]
    float zoneAzimuthMax;      // Azimuth maximum (degrés) [-180, 180]
    float zoneElevationMin;    // Elevation minimum (degrés) [-90, 90]
    float zoneElevationMax;    // Elevation maximum (degrés) [-90, 90]

    // ═══════════════════════════════════════════════════════════
    // ORIENTATION MONDIALE (rotation de l'éventail)
    // ═══════════════════════════════════════════════════════════
    glm::vec3 worldRotation;   // Rotation (pitch, yaw, roll) en degrés

    // ═══════════════════════════════════════════════════════════
    // PROFILS DE CONFIGURATION
    // ═══════════════════════════════════════════════════════════
    PhysicsProfile physics;
    BranchLayout layout;         // UNIQUEMENT Grid maintenant
    ColorScheme colorScheme;
    BranchDescriptor branchTemplate;

    // ═══════════════════════════════════════════════════════════
    // BRANCHES GÉNÉRÉES
    // ═══════════════════════════════════════════════════════════
    std::vector<GeneratedBranch> generatedBranches;

public:
    FireworkTemplate();
    explicit FireworkTemplate(const std::string& name);
    ~FireworkTemplate() = default;

    // Régénère toutes les branches selon layout + zone + orientation
    void RegenerateBranches();

    // Getters
    size_t GetBranchCount() const { return generatedBranches.size(); }
    int GetTotalParticleCount() const;

    // Presets
    static FireworkTemplate Chrysanthemum();
    static FireworkTemplate Palm();
    static FireworkTemplate Willow();
    static FireworkTemplate Ring();
    static FireworkTemplate Sphere();
};