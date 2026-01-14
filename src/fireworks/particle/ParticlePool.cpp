#include <glm/glm.hpp>

#include <cmath>
#include <random>

#include "ParticlePool.h"

static std::mt19937 s_poolRng(std::random_device{}());

ParticlePool::ParticlePool(size_t maxParticles)
    : lastSearchIndex(0)
{
    particles.resize(maxParticles);

    // Trail history buffer (SoA): capacity * kTrailSamples
    trailPositions.resize(maxParticles * ParticlePool::kTrailSamples, glm::vec3(0.0f));

    // Initialiser toutes les particules comme inactives
    for (auto& p : particles)
    {
        p.active = false;
    }
}

const glm::vec3* ParticlePool::GetTrailBuffer(int particleIndex) const
{
    const size_t idx = static_cast<size_t>(particleIndex);
    return &trailPositions[idx * ParticlePool::kTrailSamples];
}

glm::vec3* ParticlePool::GetTrailBuffer(int particleIndex)
{
    const size_t idx = static_cast<size_t>(particleIndex);
    return &trailPositions[idx * ParticlePool::kTrailSamples];
}

int ParticlePool::Allocate()
{
    const size_t capacity = particles.size();

    // Chercher depuis lastSearchIndex
    for (size_t i = lastSearchIndex; i < capacity; ++i)
    {
        if (!particles[i].active)
        {
            lastSearchIndex = i;
            return static_cast<int>(i);
        }
    }

    // Chercher depuis le début jusqu'à lastSearchIndex
    for (size_t i = 0; i < lastSearchIndex; ++i)
    {
        if (!particles[i].active)
        {
            lastSearchIndex = i;
            return static_cast<int>(i);
        }
    }

    // Pool plein
    return -1;
}

void ParticlePool::Free(int index)
{
    if (index >= 0 && index < static_cast<int>(particles.size()))
    {
        particles[index].active = false;
        // Leave trail buffer as-is. trailCount gates rendering.
        particles[index].trailCount = 0;
        particles[index].trailHead = 0;
        particles[index].trailSampleAccum = 0.0f;
    }
}

size_t ParticlePool::GetActiveCount() const
{
    size_t count = 0;
    for (const auto& p : particles)
    {
        if (p.active)
        {
            ++count;
        }
    }
    return count;
}

void ParticlePool::Update(float deltaTime)
{
    const glm::vec3 baseGravity(0.0f, -9.81f, 0.0f);
    const float dt = (deltaTime > 0.0f) ? deltaTime : 0.0f;

    for (size_t i = 0; i < particles.size(); ++i)
    {
        Particle& p = particles[i];
        if (!p.active) continue;

        p.lifeTime -= dt;
        if (p.lifeTime <= 0.0f)
        {
            // Death event: spawn optional secondary effects (smoke / recursion).
            // Note: this is intentionally simple and "art-first".
            const glm::vec3 deathPos = p.position;

            // Smoke as particles (renderer already supports particles).
            float smoke = std::max(0.0f, std::min(1.0f, p.smokeAmount));
            if (smoke > 0.0f) {
                const int count = static_cast<int>(3 + 12 * smoke);
                std::uniform_real_distribution<float> d(-1.0f, 1.0f);
                for (int s = 0; s < count; ++s) {
                    int si = Allocate();
                    if (si < 0) break;
                    Particle& sp = Get(si);
                    sp.active = true;
                    sp.position = deathPos;
                    sp.velocity = glm::vec3(d(s_poolRng), 0.6f + 0.4f * std::abs(d(s_poolRng)), d(s_poolRng)) * (0.25f + 0.35f * smoke);
                    sp.damping = 1.5f + 2.5f * smoke;
                    sp.gravityScale = 0.05f;
                    sp.updraft = 0.8f;
                    sp.size = 10.0f + 18.0f * smoke;
                    sp.lifeTime = sp.originalLifeTime = 0.8f + 1.6f * smoke;
                    sp.shapeId = p.shapeId;
                    sp.trailEnabled = false;
                    sp.shouldFade = true;
                    sp.fadeStartRatio = 1.0f;
                    sp.baseColor = sp.color = glm::vec4(0.65f, 0.65f, 0.70f, 0.10f + 0.18f * smoke);
                    sp.smokeAmount = 0.0f;
                    sp.recursionDepthRemaining = 0;
                    sp.recursionProb = 0.0f;
                    sp.trailCount = 0;
                    sp.trailHead = 0;
                    sp.trailSampleAccum = 0.0f;
                }
            }

            // Recursion: spawn a small sparkle burst at death.
            if (p.recursionDepthRemaining > 0) {
                std::uniform_real_distribution<float> d01(0.0f, 1.0f);
                if (d01(s_poolRng) < std::max(0.0f, std::min(1.0f, p.recursionProb))) {
                    std::uniform_real_distribution<float> d(-1.0f, 1.0f);
                    const int count = 6;
                    for (int s = 0; s < count; ++s) {
                        int ci = Allocate();
                        if (ci < 0) break;
                        Particle& cp = Get(ci);
                        cp.active = true;
                        cp.position = deathPos;
                        cp.velocity = glm::normalize(glm::vec3(d(s_poolRng), d(s_poolRng), d(s_poolRng))) * (3.0f + 4.0f * d01(s_poolRng));
                        cp.damping = 6.0f;
                        cp.gravityScale = 0.25f;
                        cp.updraft = 0.2f;
                        cp.size = std::max(2.0f, p.size * 0.5f);
                        cp.lifeTime = cp.originalLifeTime = 0.6f;
                        cp.shapeId = p.shapeId;
                        cp.trailEnabled = p.trailEnabled;
                        cp.trailWidth = p.trailWidth;
                        cp.trailDuration = p.trailDuration;
                        cp.trailOpacity = p.trailOpacity;
                        cp.trailFalloffPow = p.trailFalloffPow;
                        cp.trailSamplePeriod = p.trailSamplePeriod;
                        cp.trailSampleAccum = 0.0f;
                        cp.trailHead = 0;
                        cp.trailCount = 0;
                        cp.shouldFade = true;
                        cp.fadeStartRatio = 1.0f;
                        cp.baseColor = cp.color = p.baseColor;
                        cp.color.a = std::min(1.0f, p.color.a);
                        cp.smokeAmount = 0.0f;
                        cp.recursionDepthRemaining = p.recursionDepthRemaining - 1;
                        cp.recursionProb = p.recursionProb;
                    }
                }
            }

            p.active = false;
            p.trailCount = 0;
            continue;
        }


        // Free phase integration: dv/dt = g - k v
        glm::vec3 g = baseGravity * p.gravityScale;
        g.y += p.updraft;
        float k = (p.damping >= 0.0f) ? p.damping : 0.0f;

        if (k > 1e-6f)
        {
            float e = std::exp(-k * dt);
            p.velocity = p.velocity * e + (g / k) * (1.0f - e);
        }
        else
        {
            p.velocity += g * dt;
        }

        p.position += p.velocity * dt;

        // Trail sampling (discrete, fixed ring buffer)
        if (p.trailEnabled && p.trailDuration > 0.0f && p.trailSamplePeriod > 0.0f)
        {
            p.trailSampleAccum += dt;
            // Ensure we at least keep the newest point up to date when dt is very small
            if (p.trailCount == 0)
            {
                p.trailHead = 0;
                p.trailCount = 1;
                GetTrailBuffer(static_cast<int>(i))[0] = p.position;
                p.trailSampleAccum = 0.0f;
            }
            while (p.trailSampleAccum >= p.trailSamplePeriod)
            {
                p.trailSampleAccum -= p.trailSamplePeriod;
                p.trailHead = static_cast<uint8_t>((p.trailHead + 1u) % ParticlePool::kTrailSamples);
                glm::vec3* buf = GetTrailBuffer(static_cast<int>(i));
                buf[p.trailHead] = p.position;
                if (p.trailCount < ParticlePool::kTrailSamples)
                {
                    p.trailCount++;
                }
            }
        }
        else
        {
            p.trailCount = 0;
        }

        // Fade
        if (p.shouldFade)
        {
            float lifeRatio = p.lifeTime / p.originalLifeTime;
            if (lifeRatio < p.fadeStartRatio)
            {
                float fadeProgress = lifeRatio / p.fadeStartRatio;
                p.color = p.baseColor;
                p.color.a = fadeProgress;
            }
        }
        else
        {
            p.color.a = std::sqrt(p.lifeTime / p.originalLifeTime);
        }
    }
}

void ParticlePool::ClearAll()
{
    // Mark everything inactive and reset per-particle trail state.
    for (auto& p : particles) {
        p.active = false;
        p.trailCount = 0;
        p.trailHead = 0;
        p.trailSampleAccum = 0.0f;
    }
    std::fill(trailPositions.begin(), trailPositions.end(), glm::vec3(0.0f));
    lastSearchIndex = 0;
}