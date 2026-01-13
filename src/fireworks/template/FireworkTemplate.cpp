#include "FireworkTemplate.h"
#include "../simulation/BranchLayoutGenerator.h"
#include "../simulation/ColorSchemeEvaluator.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>

FireworkTemplate::FireworkTemplate()
    : name("Unnamed Firework")
    , zoneAzimuthMin(-180.0f)
    , zoneAzimuthMax(180.0f)
    , zoneElevationMin(0.0f)
    , zoneElevationMax(90.0f)
    , worldRotation(0.0f, 0.0f, 0.0f)
    , physics(PhysicsProfile::Artistic())
    , layout()
    , colorScheme()
    , branchTemplate()
{
}

FireworkTemplate::FireworkTemplate(const std::string& _name)
    : name(_name)
    , zoneAzimuthMin(-180.0f)
    , zoneAzimuthMax(180.0f)
    , zoneElevationMin(0.0f)
    , zoneElevationMax(90.0f)
    , worldRotation(0.0f, 0.0f, 0.0f)
    , physics(PhysicsProfile::Artistic())
    , layout()
    , colorScheme()
    , branchTemplate()
{
}

void FireworkTemplate::RegenerateBranches()
{
    // ═══════════════════════════════════════════════════════════
    // DEBUG
    // ═══════════════════════════════════════════════════════════
    std::cerr << "[RegenerateBranches] Called!" << std::endl;
    std::cerr << "  initialSpeed: " << branchTemplate.initialSpeed << std::endl;
    std::cerr << "  damping: " << branchTemplate.damping << std::endl;
    std::cerr << "  particlesPerBranch: " << branchTemplate.particlesPerBranch << std::endl;

    // ═══════════════════════════════════════════════════════════
    // ÉTAPE 1 : Copier les contraintes dans le layout
    // ═══════════════════════════════════════════════════════════
    layout.constraintAzimuthMin = zoneAzimuthMin;
    layout.constraintAzimuthMax = zoneAzimuthMax;
    layout.constraintElevationMin = zoneElevationMin;
    layout.constraintElevationMax = zoneElevationMax;

    // ═══════════════════════════════════════════════════════════
    // ÉTAPE 2 : Générer la grid (directions dans la zone sphérique)
    // ═══════════════════════════════════════════════════════════
    BranchLayoutGenerator::Generate(layout, generatedBranches);

    // ═══════════════════════════════════════════════════════════
    // ÉTAPE 3 : Appliquer la rotation mondiale
    // ═══════════════════════════════════════════════════════════
    // Créer matrice de rotation depuis angles d'Euler (pitch, yaw, roll)
    glm::mat4 rotationMatrix = glm::mat4(1.0f);

    // Ordre : Yaw (Y) → Pitch (X) → Roll (Z)
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(worldRotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(worldRotation.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(worldRotation.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll

    // Appliquer rotation à toutes les directions
    for (auto& branch : generatedBranches) {
        glm::vec4 rotatedDir = rotationMatrix * glm::vec4(branch.direction, 0.0f);
        branch.direction = glm::normalize(glm::vec3(rotatedDir));
    }

    // ═══════════════════════════════════════════════════════════
    // ÉTAPE 4 : Copier les paramètres de branche
    // ═══════════════════════════════════════════════════════════
    for (auto& branch : generatedBranches) {
        branch.initialSpeed = branchTemplate.initialSpeed;
        branch.speedVariance = branchTemplate.speedVariance;
        branch.damping = branchTemplate.damping;
        branch.dampingVariance = branchTemplate.dampingVariance;
        branch.gravityScale = branchTemplate.gravityScale;
        branch.updraft = branchTemplate.updraft;
        branch.angularSpread = branchTemplate.angularSpread;
        branch.particlesPerBranch = branchTemplate.particlesPerBranch;
        branch.emissionDuration = branchTemplate.emissionDuration;
        branch.particleSize = branchTemplate.particleSize;
        branch.sizeVariance = branchTemplate.sizeVariance;
        branch.lifetime = branchTemplate.lifetime;
        branch.shapeId = branchTemplate.shapeId;

        branch.visualMode = branchTemplate.visualMode;
        branch.trailEnabled = branchTemplate.trailEnabled;
        branch.trailWidth = branchTemplate.trailWidth;
        branch.trailDuration = branchTemplate.trailDuration;
        branch.trailOpacity = branchTemplate.trailOpacity;
        branch.trailFalloffPow = branchTemplate.trailFalloffPow;

        branch.frontPortion = branchTemplate.frontPortion;
        branch.frontSpeedBias = branchTemplate.frontSpeedBias;
        branch.backSpeedScale = branchTemplate.backSpeedScale;

        branch.sparkleSpeedJitter = branchTemplate.sparkleSpeedJitter;
        branch.sparkleSpreadMult = branchTemplate.sparkleSpreadMult;

        branch.smokeAmount = branchTemplate.smokeAmount;
        branch.recursionDepth = branchTemplate.recursionDepth;
        branch.recursionProb = branchTemplate.recursionProb;
    }

    // ═══════════════════════════════════════════════════════════
    // ÉTAPE 5 : Appliquer les couleurs
    // ═══════════════════════════════════════════════════════════
    ColorSchemeEvaluator::ApplyColors(colorScheme, layout, generatedBranches);
}

int FireworkTemplate::GetTotalParticleCount() const
{
    return static_cast<int>(generatedBranches.size()) * branchTemplate.particlesPerBranch;
}

// ═══════════════════════════════════════════════════════════════
//                          PRESETS
// ═══════════════════════════════════════════════════════════════

FireworkTemplate FireworkTemplate::Chrysanthemum()
{
    FireworkTemplate t("Chrysanthemum");

    // Hémisphère supérieur complet
    t.zoneAzimuthMin = -180.0f;
    t.zoneAzimuthMax = 180.0f;
    t.zoneElevationMin = 0.0f;
    t.zoneElevationMax = 90.0f;

    t.worldRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    t.layout.gridX = 12;
    t.layout.gridY = 8;

    t.branchTemplate.initialSpeed = 18.0f;
    t.branchTemplate.damping = 10.0f;
    t.branchTemplate.gravityScale = 0.35f;
    t.branchTemplate.updraft = 0.5f;
    t.branchTemplate.particlesPerBranch = 20;
    t.branchTemplate.particleSize = 8.0f;
    t.branchTemplate.lifetime = 3.0f;

    t.colorScheme.type = ColorDistributionType::Gradient;
    t.colorScheme.gradientStart = glm::vec4(1.0f, 0.8f, 0.0f, 1.0f);
    t.colorScheme.gradientEnd = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    t.physics = PhysicsProfile::Artistic();
    t.RegenerateBranches();

    return t;
}

FireworkTemplate FireworkTemplate::Palm()
{
    FireworkTemplate t("Palm");

    // Éventail étroit vers le haut
    t.zoneAzimuthMin = -60.0f;
    t.zoneAzimuthMax = 60.0f;
    t.zoneElevationMin = 60.0f;
    t.zoneElevationMax = 90.0f;

    t.worldRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    t.layout.gridX = 8;
    t.layout.gridY = 6;

    t.branchTemplate.initialSpeed = 18.0f;
    t.branchTemplate.damping = 10.0f;
    t.branchTemplate.gravityScale = 0.35f;
    t.branchTemplate.updraft = 0.5f;
    t.branchTemplate.particlesPerBranch = 30;
    t.branchTemplate.particleSize = 10.0f;
    t.branchTemplate.lifetime = 4.0f;

    t.colorScheme.type = ColorDistributionType::Uniform;
    t.colorScheme.uniformColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

    t.physics = PhysicsProfile::Realistic();
    t.RegenerateBranches();

    return t;
}

FireworkTemplate FireworkTemplate::Willow()
{
    FireworkTemplate t("Willow");

    // Large éventail, descend vers le bas
    t.zoneAzimuthMin = -180.0f;
    t.zoneAzimuthMax = 180.0f;
    t.zoneElevationMin = -30.0f;
    t.zoneElevationMax = 90.0f;

    t.worldRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    t.layout.gridX = 16;
    t.layout.gridY = 10;

    t.branchTemplate.initialSpeed = 18.0f;
    t.branchTemplate.damping = 10.0f;
    t.branchTemplate.gravityScale = 0.35f;
    t.branchTemplate.updraft = 0.5f;
    t.branchTemplate.particlesPerBranch = 40;
    t.branchTemplate.particleSize = 6.0f;
    t.branchTemplate.lifetime = 5.0f;

    t.colorScheme.type = ColorDistributionType::Uniform;
    t.colorScheme.uniformColor = glm::vec4(1.0f, 0.84f, 0.0f, 1.0f);

    t.physics.gravity = 15.0f;
    t.physics.gravityCurve = 0.9f;
    t.RegenerateBranches();

    return t;
}

FireworkTemplate FireworkTemplate::Ring()
{
    FireworkTemplate t("Ring");

    // Anneau horizontal
    t.zoneAzimuthMin = -180.0f;
    t.zoneAzimuthMax = 180.0f;
    t.zoneElevationMin = -5.0f;
    t.zoneElevationMax = 5.0f;

    t.worldRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    t.layout.gridX = 24;
    t.layout.gridY = 2;

    t.branchTemplate.initialSpeed = 6.000f;
    t.branchTemplate.damping = 10.0f;
    t.branchTemplate.gravityScale = 0.35f;
    t.branchTemplate.updraft = 0.5f;
    t.branchTemplate.particlesPerBranch = 25;
    t.branchTemplate.particleSize = 8.0f;
    t.branchTemplate.lifetime = 3.0f;

    t.colorScheme.type = ColorDistributionType::Alternating;
    t.colorScheme.palette.clear();
    t.colorScheme.palette.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    t.colorScheme.palette.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    t.colorScheme.palette.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

    t.physics = PhysicsProfile::Artistic();
    t.RegenerateBranches();

    return t;
}

FireworkTemplate FireworkTemplate::Sphere()
{
    FireworkTemplate t("Sphere");

    // Sphère complète
    t.zoneAzimuthMin = -180.0f;
    t.zoneAzimuthMax = 180.0f;
    t.zoneElevationMin = -90.0f;
    t.zoneElevationMax = 90.0f;

    t.worldRotation = glm::vec3(0.0f, 0.0f, 0.0f);

    t.layout.gridX = 20;
    t.layout.gridY = 12;

    t.branchTemplate.initialSpeed = 5.333f;
    t.branchTemplate.damping = 10.0f;
    t.branchTemplate.gravityScale = 0.35f;
    t.branchTemplate.updraft = 0.5f;
    t.branchTemplate.particlesPerBranch = 15;
    t.branchTemplate.particleSize = 10.0f;
    t.branchTemplate.lifetime = 2.5f;

    t.colorScheme.type = ColorDistributionType::Random;
    t.colorScheme.palette.clear();
    t.colorScheme.palette.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    t.colorScheme.palette.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
    t.colorScheme.palette.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    t.colorScheme.palette.push_back(glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
    t.colorScheme.palette.push_back(glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

    t.physics = PhysicsProfile::ZeroGravity();
    t.RegenerateBranches();

    return t;
}