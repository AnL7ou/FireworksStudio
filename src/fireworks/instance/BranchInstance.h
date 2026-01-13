#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "../template/GeneratedBranch.h"
#include "../template/PhysicsProfile.h"
#include "../particle/ParticlePool.h"

// Gère le cycle de vie d'UNE branche spécifique
// (Différent de FireworkInstance qui gère TOUTES les branches d'un template)
class BranchInstance {
public:
    BranchInstance();
    BranchInstance(
        const GeneratedBranch* branchDesc,
        const PhysicsProfile* physics,
        const glm::vec3& worldPos
    );
    ~BranchInstance();

    // Update la branche (met à jour ses particules)
    void Update(float deltaTime, ParticlePool& pool);

    // Spawn les particules de cette branche
    void Spawn(ParticlePool& pool);

    // Vérifie si la branche est terminée (toutes particules mortes)
    bool IsComplete() const;

    // Libère les particules (les rend au pool)
    void Release(ParticlePool& pool);

private:
    const GeneratedBranch* descriptor;
    const PhysicsProfile* physicsProfile;
    glm::vec3 position;

    std::vector<int> particleIndices;  // Indices des particules dans le pool
    bool spawned;
    float timeAlive;
};