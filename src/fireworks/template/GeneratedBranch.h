#pragma once

#include <glm/glm.hpp>
#include <cstdint>

#include "BranchDescriptor.h"

// Branche générée : résultat final après application du BranchLayout et ColorScheme.
// C'est cette structure qui sera utilisée pour spawner les particules en runtime.
struct GeneratedBranch {
    // ═══ SPATIAL (calculé par BranchLayoutGenerator) ═══
    glm::vec3 direction;          // Direction de tir (normalisée)

    // ═══ VISUEL (calculé par ColorSchemeEvaluator) ═══
    glm::vec4 color;              // Couleur finale de cette branche

    // ═══ CINÉMATIQUE (copié depuis BranchDescriptor) ═══
    float initialSpeed;
    float speedVariance;
    float damping;
    float dampingVariance;

    // ═══ ENVIRONNEMENT ═══
    float gravityScale;
    float updraft;

    // ═══ SPREAD ═══
    float angularSpread;

    // ═══ PARTICULES ═══
    int particlesPerBranch;

    // ═══ ÉMISSION (temps) ═══
    float emissionDuration;    // 0 = burst; >0 = étalé dans le temps

    // ═══ VISUEL ═══
    float particleSize;
    float sizeVariance;
    float lifetime;
    uint16_t shapeId;

    // ═══ FAMILLE VISUELLE / TRAIL ═══
    BranchDescriptor::VisualMode visualMode = BranchDescriptor::VisualMode::Comet;

    bool  trailEnabled     = false;
    float trailWidth       = 0.02f;
    float trailDuration    = 0.35f;
    float trailOpacity     = 0.15f;
    float trailFalloffPow  = 2.0f;

    // ═══ VITESSE : densifier la tête ═══
    float frontPortion     = 0.85f;
    float frontSpeedBias   = 4.0f;
    float backSpeedScale   = 0.55f;

    // ═══ SPARKLE tweaks ═══
    float sparkleSpeedJitter = 0.15f;
    float sparkleSpreadMult  = 1.5f;

    // ═══ EXTENSIONS (non câblées ici) ═══
    float smokeAmount     = 0.25f;
    int   recursionDepth  = 0;
    float recursionProb   = 0.0f;

    // ═══ FADE (copié depuis ColorScheme) ═══
    bool shouldFade = false;
    float fadeStartRatio = 0.5f;

    GeneratedBranch()
        : direction(0.0f, 1.0f, 0.0f)
        , color(1.0f)
        , initialSpeed(18.0f)
        , speedVariance(0.10f)
        , damping(10.0f)
        , dampingVariance(0.35f)
        , gravityScale(0.35f)
        , updraft(0.5f)
        , angularSpread(10.0f)
        , particlesPerBranch(40)
        , emissionDuration(0.0f)
        , particleSize(6.0f)
        , sizeVariance(0.3f)
        , lifetime(2.5f)
        , shapeId(0)
    {}
};