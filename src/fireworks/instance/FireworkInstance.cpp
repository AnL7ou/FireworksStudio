#include <iostream>

#include "FireworkInstance.h"
#include "../simulation/BranchGenerator.h"

FireworkInstance::FireworkInstance()
    : fireworkTemplate(nullptr)
    , position(0.0f)
    , triggerTime(0.0f)
    , triggered(false)
    , finished(false)
{
}

FireworkInstance::FireworkInstance(const FireworkTemplate* tmpl, const glm::vec3& pos, float trigTime)
    : fireworkTemplate(tmpl)
    , position(pos)
    , triggerTime(trigTime)
    , triggered(false)
    , finished(false)
{
    (void)fireworkTemplate;
}

FireworkInstance::~FireworkInstance()
{
}

void FireworkInstance::Update(float currentTime, float deltaTime, ParticlePool& pool)
{
    if (finished || !fireworkTemplate) {
        return;
    }

    const float dt = (deltaTime > 0.0f) ? deltaTime : 0.0f;

    // Déclenchement : initialiser les émetteurs, mais ne pas tout burst systématiquement.
    if (!triggered && currentTime >= triggerTime) {
        std::cout << "[FireworkInstance] Triggering " << fireworkTemplate->name
            << " with " << fireworkTemplate->GetBranchCount() << " branches\n";

        emitters.clear();
        emitters.reserve(fireworkTemplate->generatedBranches.size());
        for (const auto& b : fireworkTemplate->generatedBranches) {
            BranchEmitter e;
            e.branch = &b;
            e.emitted = 0;
            e.accum = 0.0f;
            // 0 = burst. Otherwise, distribute spawns over emissionDuration.
            if (b.emissionDuration > 0.0f && b.particlesPerBranch > 0) {
                e.interval = b.emissionDuration / static_cast<float>(b.particlesPerBranch);
                if (e.interval < 1.0f / 240.0f) e.interval = 1.0f / 240.0f;
            } else {
                e.interval = 0.0f;
            }
            e.done = false;
            emitters.push_back(e);
        }

        triggered = true;
    }

    if (!triggered) return;

    // Update emission
    bool allDone = true;
    for (auto& e : emitters) {
        if (!e.branch || e.done) continue;
        const auto& b = *e.branch;

        // Burst mode
        if (e.interval <= 0.0f) {
            if (e.emitted == 0) {
                BranchGenerator::EmitBranch(b, fireworkTemplate->physics, position, pool);
                e.emitted = b.particlesPerBranch;
            }
            e.done = true;
            continue;
        }

        // Timed emission
        e.accum += dt;
        while (e.emitted < b.particlesPerBranch && e.accum >= e.interval) {
            e.accum -= e.interval;
            const int idx = BranchGenerator::EmitParticle(b, fireworkTemplate->physics, position, pool, e.emitted, b.particlesPerBranch);
            if (idx < 0) {
                // pool full: stop trying this frame
                break;
            }
            e.emitted++;
        }

        if (e.emitted >= b.particlesPerBranch) {
            e.done = true;
        } else {
            allDone = false;
        }
    }

    if (allDone) {
        finished = true;
        std::cout << "[FireworkInstance] Emission finished. Total particles active: "
            << pool.GetActiveCount() << "\n";
    }
}