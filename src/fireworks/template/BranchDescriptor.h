#pragma once

#include <cstdint>

// Template de branche : définit les paramètres de base d'une branche
// AVANT application du layout et du color scheme.
// Ce descriptor sera copié et instancié pour chaque branche générée.
struct BranchDescriptor {
    // ═══ CINÉMATIQUE (vitesse + drag) ═══
    float initialSpeed;          // Vitesse initiale (m/s)
    float speedVariance;         // 0-1 : variance relative de vitesse par particule
    float damping;               // Drag linéaire (1/s)
    float dampingVariance;       // 0-1 : variance relative de damping par particule

    // ═══ ENVIRONNEMENT (contrôle artistique) ═══
    float gravityScale;          // 0-1 : échelle de gravité (0 = pas de chute)
    float updraft;               // Accélération verticale additionnelle (m/s^2), réduit la chute

    // ═══ SPREAD ═══
    float angularSpread;         // Écart angulaire entre particules d'une même branche (degrés)

    // ═══ PARTICULES ═══
    int particlesPerBranch;      // Nombre de particules émises par branche

    // ═══ ÉMISSION (temps) ═══
    // 0 = burst instantané (comportement actuel). >0 = émission étalée dans le temps,
    // utile pour obtenir des "têtes" + "queues" plus lisibles (trail-like) et éviter
    // une dispersion purement radiale.
    float emissionDuration;      // Durée d'émission (secondes)

    // ═══ VISUEL ═══
    float particleSize;          // Taille de base des particules (pixels)
    float sizeVariance;          // 0-1 : variance de taille par particule
    float lifetime;              // Durée de vie (secondes)
    uint16_t shapeId;            // Index dans ShapeRegistry

    // ═══ FAMILLE VISUELLE ═══
    // "Comet" : une tête lisible + traînée forte.
    // "Sparkle" : plusieurs sparks, jitter, traînée faible.
    enum class VisualMode : int { Comet = 0, Sparkle = 1 };
    VisualMode visualMode = VisualMode::Comet;

    // ═══ TRAIL (ruban) ═══
    bool  trailEnabled     = false;
    float trailWidth       = 0.02f;
    float trailDuration    = 0.35f;
    float trailOpacity     = 0.15f; // facteur d'opacité (0..1), séparé de la couleur de particule
    float trailFalloffPow  = 2.0f;  // courbe d'atténuation sur la longueur (>=1)

    // ═══ VITESSE : densifier la "tête" ═══
    // Beaucoup de particules proches de la vitesse max : biais vers l'avant.
    float frontPortion     = 0.85f; // proportion de particules "avant" (0..1)
    float frontSpeedBias   = 4.0f;  // >1 : plus grand = plus concentré vers l'avant
    float backSpeedScale   = 0.55f; // multiplicateur de vitesse pour la "queue" (0..1)

    // ═══ SPARKLE tweaks ═══
    float sparkleSpeedJitter = 0.15f; // jitter relatif sur la vitesse en mode Sparkle
    float sparkleSpreadMult  = 1.5f;  // multiplicateur de spread en mode Sparkle

    // ═══ EXTENSIONS (non câblées ici) ═══
    float smokeAmount     = 0.25f;
    int   recursionDepth  = 0;
    float recursionProb   = 0.0f;

    // ═══ FADE (piloté par ColorScheme) ═══
    bool shouldFade = false;
    float fadeStartRatio = 0.5f;

    BranchDescriptor()
        : initialSpeed(18.0f)
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