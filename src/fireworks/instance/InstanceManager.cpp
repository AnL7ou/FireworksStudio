#include <algorithm>

#include "InstanceManager.h"

InstanceManager::InstanceManager()
{
}

InstanceManager::~InstanceManager()
{
    Clear();
}

void InstanceManager::AddInstance(FireworkInstance* instance)
{
    if (instance) {
        instances.push_back(instance);
    }
}

void InstanceManager::Update(float currentTime, float deltaTime, ParticlePool& pool)
{
    // Update toutes les instances
    for (auto* instance : instances) {
        if (instance) {
            instance->Update(currentTime, deltaTime, pool);
        }
    }
}

void InstanceManager::CleanupCompleted()
{
    // Supprimer les instances déclenchées depuis longtemps
    // (on pourrait vérifier si toutes leurs particules sont mortes)

    auto it = instances.begin();
    while (it != instances.end()) {
        FireworkInstance* instance = *it;

        // Critère simple : instance déclenchée depuis plus de 10 secondes
        if (instance && instance->IsTriggered()) {
            // TODO: vérifier que toutes les particules sont mortes
            // Pour l'instant, on garde toutes les instances
            ++it;
        }
        else {
            ++it;
        }
    }

    // Version simple : ne rien supprimer automatiquement
    // L'utilisateur peut Clear() manuellement
}

void InstanceManager::Clear()
{
    for (auto* instance : instances) {
        delete instance;
    }
    instances.clear();
}