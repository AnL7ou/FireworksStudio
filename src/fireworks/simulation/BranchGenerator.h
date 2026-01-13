#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../template/PhysicsProfile.h"
#include "../template/GeneratedBranch.h"
#include "../particle/ParticlePool.h"

// Générateur de branches : calcule les vitesses initiales et émet les particules
class BranchGenerator {
public:
    // Émet une branche complète dans le pool
    // Retourne les indices des particules allouées
    static std::vector<int> EmitBranch(
        const GeneratedBranch& branch,
        const PhysicsProfile& physics,
        const glm::vec3& worldPosition,
        ParticlePool& pool
    );

    // Émet une seule particule.
    // spawnIndex/totalSpawns permettent d'obtenir une "tête"/"queue" cohérente
    // quand l'émission est étalée dans le temps (ordre = progress).
    // Retourne l'index alloué, ou -1 si le pool est plein.
    static int EmitParticle(
        const GeneratedBranch& branch,
        const PhysicsProfile& physics,
        const glm::vec3& worldPosition,
        ParticlePool& pool,
        int spawnIndex,
        int totalSpawns
    );

private:
    // Calcule la vitesse initiale pour une particule de cette branche
    static glm::vec3 ComputeInitialVelocity(
        const GeneratedBranch& branch,
        const PhysicsProfile& physics,
        float randomSeed  // 0-1 pour variance
    );

    // Génère une direction aléatoire dans le cone angularSpread
    static glm::vec3 RandomInCone(const glm::vec3& direction, float halfAngleDeg);
};