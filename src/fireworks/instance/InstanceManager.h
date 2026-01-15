#pragma once

#include <vector>
#include <memory>
#include "FireworkInstance.h"
#include "../particle/ParticlePool.h"

// Gestionnaire d'instances de feux d'artifice
// Gère plusieurs FireworkInstance simultanément
class InstanceManager {
public:
    InstanceManager();
    ~InstanceManager();

    // Ajoute une nouvelle instance
    void AddInstance(FireworkInstance* instance);

    // Update toutes les instances actives
    void Update(float currentTime, float deltaTime, ParticlePool& pool);

    // Supprime les instances complètes
    void CleanupCompleted();

    // Supprime toutes les instances
    void Clear();

    // Getters
    size_t GetActiveCount() const { return instances.size(); }
    const std::vector<FireworkInstance*>& GetInstances() const { return instances; }

private:
    std::vector<FireworkInstance*> instances;
};