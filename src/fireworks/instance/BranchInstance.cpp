#include "BranchInstance.h"
#include "../simulation/BranchGenerator.h"

BranchInstance::BranchInstance()
    : descriptor(nullptr)
    , physicsProfile(nullptr)
    , position(0.0f)
    , spawned(false)
    , timeAlive(0.0f)
{
}

BranchInstance::BranchInstance(
    const GeneratedBranch* branchDesc,
    const PhysicsProfile* physics,
    const glm::vec3& worldPos
)
    : descriptor(branchDesc)
    , physicsProfile(physics)
    , position(worldPos)
    , spawned(false)
    , timeAlive(0.0f)
{
}

BranchInstance::~BranchInstance()
{
}

void BranchInstance::Update(float deltaTime, ParticlePool& pool)
{
    if (!spawned) return;

    timeAlive += deltaTime;

    // Les particules se mettent à jour toutes seules dans le pool
    // Ici on pourrait ajouter des effets spécifiques à la branche
}

void BranchInstance::Spawn(ParticlePool& pool)
{
    if (spawned || !descriptor || !physicsProfile) return;

    // Utiliser BranchGenerator pour créer les particules
    particleIndices = BranchGenerator::EmitBranch(
        *descriptor,
        *physicsProfile,
        position,
        pool
    );

    spawned = true;
}

bool BranchInstance::IsComplete() const
{
    if (!spawned) return false;

    // Vérifier si toutes les particules sont mortes
    // Pour simplifier, on considère que c'est complet après un certain temps
    // ou quand toutes les particules sont inactives

    return timeAlive > (descriptor ? descriptor->lifetime : 10.0f);
}

void BranchInstance::Release(ParticlePool& pool)
{
    for (int idx : particleIndices) {
        pool.Free(idx);
    }
    particleIndices.clear();
    spawned = false;
}