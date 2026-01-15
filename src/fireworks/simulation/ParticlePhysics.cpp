#include <cmath>

#include "ParticlePhysics.h"

void ParticlePhysics::Integrate(
    Particle& particle,
    const PhysicsProfile& physics,
    float deltaTime
)
{
    if (!particle.active) return;

    // Gravité
    ApplyGravity(particle, physics.gravity, deltaTime);

    // Drag (résistance de l'air)
    ApplyDrag(particle, physics.dragCoefficient, deltaTime);

    // Turbulence
    static float globalTime = 0.0f;
    globalTime += deltaTime;
    ApplyTurbulence(
        particle,
        physics.turbulenceStrength,
        physics.turbulenceFrequency,
        globalTime,
        deltaTime
    );

    // Clamp vitesse maximale
    float speed = glm::length(particle.velocity);
    if (speed > physics.maxSpeed) {
        particle.velocity = glm::normalize(particle.velocity) * physics.maxSpeed;
    }

    // Mise à jour position
    particle.position += particle.velocity * deltaTime;
}

void ParticlePhysics::ApplyGravity(
    Particle& particle,
    float gravity,
    float deltaTime
)
{
    particle.velocity.y -= gravity * deltaTime;
}

void ParticlePhysics::ApplyDrag(
    Particle& particle,
    float dragCoefficient,
    float deltaTime
)
{
    // Drag exponentiel : v *= (1 - drag)^dt
    float dragFactor = std::pow(1.0f - dragCoefficient, deltaTime);
    particle.velocity *= dragFactor;
}

void ParticlePhysics::ApplyTurbulence(
    Particle& particle,
    float strength,
    float frequency,
    float time,
    float deltaTime
)
{
    if (strength <= 0.0f) return;

    // Calculer le bruit à la position actuelle
    glm::vec3 noiseInput = particle.position * frequency + glm::vec3(time * 0.1f);
    glm::vec3 noise = ComputeNoise3D(noiseInput, time);

    // Appliquer comme force
    particle.velocity += noise * strength * deltaTime;
}

glm::vec3 ParticlePhysics::ComputeNoise3D(const glm::vec3& pos, float time)
{
    // Bruit pseudo-aléatoire simple basé sur sin/cos
    // Pour un vrai bruit, utiliser Perlin ou Simplex noise

    float x = Noise1D(pos.x + time * 0.1f);
    float y = Noise1D(pos.y + time * 0.13f + 100.0f);
    float z = Noise1D(pos.z + time * 0.17f + 200.0f);

    return glm::vec3(x, y, z) * 2.0f - 1.0f;  // Range [-1, 1]
}

float ParticlePhysics::Noise1D(float x)
{
    // Hash function simple
    float n = std::sin(x * 12.9898f + 78.233f) * 43758.5453f;
    return n - std::floor(n);
}