#pragma once

// Profil physique global appliqué à toutes les branches d'un FireworkTemplate
struct PhysicsProfile {
    // Gravité
    float gravity;                // Accélération vers le bas (9.8 = réaliste)
    float gravityCurve;           // 0-1 : 0 = compense pour garder branchLength, 1 = naturel

    // Drag (résistance de l'air)
    float dragCoefficient;        // 0-1 : résistance qui ralentit les particules

    // Turbulence (bruit procédural)
    float turbulenceStrength;     // Amplitude du bruit aléatoire
    float turbulenceFrequency;    // Vitesse de changement du bruit (Hz)

    // Limites de sécurité
    float maxSpeed;               // Clamp vitesse maximale (évite explosions infinies)
    float maxLifetime;            // Durée de vie maximale d'une particule (secondes)

    // Constructeur avec valeurs par défaut réalistes
    PhysicsProfile()
        : gravity(9.8f)
        , gravityCurve(0.5f)      // Hybride : un peu de compensation, un peu de naturel
        , dragCoefficient(0.1f)   // Légère résistance
        , turbulenceStrength(0.05f)
        , turbulenceFrequency(1.0f)
        , maxSpeed(100.0f)
        , maxLifetime(10.0f)
    {
    }

    // Presets utiles
    static PhysicsProfile Realistic() {
        PhysicsProfile p;
        p.gravity = 9.8f;
        p.gravityCurve = 1.0f;    // Pas de compensation
        p.dragCoefficient = 0.3f;
        return p;
    }

    static PhysicsProfile Artistic() {
        PhysicsProfile p;
        p.gravity = 5.0f;
        p.gravityCurve = 0.0f;    // Compensation totale
        p.dragCoefficient = 0.05f;
        return p;
    }

    static PhysicsProfile ZeroGravity() {
        PhysicsProfile p;
        p.gravity = 0.0f;
        p.dragCoefficient = 0.0f;
        p.turbulenceStrength = 0.0f;
        return p;
    }
};