#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;           // Couleur RGBA actuelle
    glm::vec4 baseColor;       // Couleur d'origine (pour fade)
    float lifeTime;            // Durée de vie restante
    float originalLifeTime;    // Durée de vie d'origine
    float size;                // Taille en pixels
    bool active;
    uint16_t shapeId;          // Index dans ShapeRegistry

    // Fade settings
    bool shouldFade;           // Active le fade avancé
    float fadeStartRatio;      // À quel % de vie commence le fade (0-1)

    // Physique (drag linéaire, 1/s)
    float damping;

    // Gravité (contrôle artistique)
    float gravityScale;   // 0-1
    float updraft;        // m/s^2 (accélération verticale additionnelle)

    // ═══════════════════════════════════════════════════════════
    // PHASE BRANCHE (NOUVEAU)
    // ═══════════════════════════════════════════════════════════
    bool inBranchPhase;        // true pendant branchDuration
    float branchPhaseTime;     // Temps restant dans phase branche

    // Trail (ruban)
    bool  trailEnabled;
    float trailWidth;
    float trailDuration;
    float trailOpacity;
    float trailFalloffPow;
    float trailSamplePeriod;
    float trailSampleAccum;
    uint8_t trailHead;
    uint8_t trailCount;

    // Smoke / recursion
    float smokeAmount;
    int   recursionDepthRemaining;
    float recursionProb;

    Particle()
        : position(0.0f, 0.0f, 0.0f)
        , velocity(0.0f, 0.0f, 0.0f)
        , color(1.0f, 1.0f, 1.0f, 1.0f)
        , baseColor(1.0f, 1.0f, 1.0f, 1.0f)
        , lifeTime(1.0f)
        , originalLifeTime(1.0f)
        , size(8.0f)
        , active(false)
        , shapeId(0)
        , shouldFade(true)
        , fadeStartRatio(0.7f)
        , damping(0.0f)
        , gravityScale(0.35f)
        , updraft(0.5f)
        , inBranchPhase(false)
        , branchPhaseTime(0.0f)
        , trailEnabled(false)
        , trailWidth(0.02f)
        , trailDuration(0.35f)
        , trailOpacity(0.15f)
        , trailFalloffPow(2.0f)
        , trailSamplePeriod(1.0f/60.0f)
        , trailSampleAccum(0.0f)
        , trailHead(0)
        , trailCount(0)
        , smokeAmount(0.0f)
        , recursionDepthRemaining(0)
        , recursionProb(0.0f)
    {
    }
};