#pragma once

#include <vector>
#include <cstddef>
#include <glm/glm.hpp>
#include "Particle.h"


class ParticlePool {
public:
    // Fixed history size per particle (ring buffer). Keep small: memory scales with capacity.
    static constexpr int kTrailSamples = 16;

    explicit ParticlePool(size_t maxParticles = 10000);
    ~ParticlePool() = default;

    // Alloue une particule inactive et retourne son index
    // Retourne -1 si le pool est plein
    int Allocate();

    // Libère une particule (la marque comme inactive)
    void Free(int index);

    // Accès direct aux particules
    Particle& Get(int index) { return particles[index]; }
    const Particle& Get(int index) const { return particles[index]; }

    // Accès au vecteur complet (pour le rendu)
    std::vector<Particle>& GetAll() { return particles; }
    const std::vector<Particle>& GetAll() const { return particles; }

    // Statistiques
    size_t GetCapacity() const { return particles.size(); }
    size_t GetActiveCount() const;

    // Update toutes les particules actives
    void Update(float deltaTime);

    // Trail history access (for renderer)
    // Returns pointer to the first element of the ring buffer for a particle.
    const glm::vec3* GetTrailBuffer(int particleIndex) const;
    glm::vec3* GetTrailBuffer(int particleIndex);

private:
    std::vector<Particle> particles;
    size_t lastSearchIndex;  // Optimisation pour Allocate()

    // SoA trail positions: capacity * kTrailSamples
    std::vector<glm::vec3> trailPositions;
};