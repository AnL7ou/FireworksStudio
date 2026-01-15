#pragma once

#include <glm/glm.hpp>
#include "../particle/Particle.h"
#include "../template/PhysicsProfile.h"

// Moteur physique pour les particules
// Applique gravité, drag, turbulence, etc.
class ParticlePhysics {
public:
    // Intègre la physique sur une particule
    static void Integrate(
        Particle& particle,
        const PhysicsProfile& physics,
        float deltaTime
    );

    // Applique uniquement la gravité
    static void ApplyGravity(
        Particle& particle,
        float gravity,
        float deltaTime
    );

    // Applique uniquement le drag
    static void ApplyDrag(
        Particle& particle,
        float dragCoefficient,
        float deltaTime
    );

    // Applique la turbulence (bruit)
    static void ApplyTurbulence(
        Particle& particle,
        float strength,
        float frequency,
        float time,
        float deltaTime
    );

private:
    // Génère du bruit 3D (simplex noise simplifié)
    static glm::vec3 ComputeNoise3D(const glm::vec3& pos, float time);

    // Fonction de bruit 1D simple
    static float Noise1D(float x);
};