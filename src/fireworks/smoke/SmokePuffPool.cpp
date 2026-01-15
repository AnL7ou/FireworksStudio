#include "SmokePuffPool.h"

#include <algorithm>
#include <cmath>
#include <random>

static std::mt19937 s_smokeRng(std::random_device{}());

SmokePuffPool::SmokePuffPool(size_t capacity)
{
    puffs.resize(capacity);
}

void SmokePuffPool::Spawn(const glm::vec3& pos, float amount)
{
    amount = std::max(0.0f, std::min(1.0f, amount));
    if (amount <= 0.0f) return;

    // Find a slot (round-robin overwrite is fine for smoke)
    SmokePuff& s = puffs[last];
    last = (last + 1) % puffs.size();

    std::uniform_real_distribution<float> d(-1.0f, 1.0f);

    s.active = true;
    s.age = 0.0f;
    s.life = 1.0f + 2.0f * amount;
    s.size = 0.15f + 0.55f * amount;
    s.opacity = 0.03f + 0.12f * amount;
    s.damping = 1.5f + 3.0f * amount;

    s.position = pos;
    // slow drift + a little upward bias
    s.velocity = glm::vec3(d(s_smokeRng), 0.5f + 0.5f * std::abs(d(s_smokeRng)), d(s_smokeRng)) * (0.15f + 0.35f * amount);
}

void SmokePuffPool::Update(float dt)
{
    if (dt <= 0.0f) return;
    const glm::vec3 up(0.0f, 0.2f, 0.0f);

    for (auto& s : puffs) {
        if (!s.active) continue;

        s.age += dt;
        if (s.age >= s.life) {
            s.active = false;
            continue;
        }

        float k = std::max(0.0f, s.damping);
        if (k > 1e-6f) {
            float e = std::exp(-k * dt);
            s.velocity *= e;
        }

        // gentle rise
        s.velocity += up * dt;
        s.position += s.velocity * dt;
    }
}
