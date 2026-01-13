#include "BranchGenerator.h"
#include <random>
#include <cmath>
#include <iostream>

static std::mt19937 s_rng(std::random_device{}());

static float Biased01(float u, float bias)
{
    // bias > 1 : pousse vers 1.0 (tête)
    if (bias <= 1.0f) return u;
    return 1.0f - std::pow(1.0f - u, bias);
}

static void InitParticleFromBranch(
    Particle& p,
    const GeneratedBranch& branch,
    const glm::vec3& worldPosition,
    float orderedProgress01
)
{
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);

    // Local cone sampling (kept here to avoid exposing BranchGenerator internals)
    auto RandomInConeLocal = [&](const glm::vec3& direction, float halfAngleDeg) -> glm::vec3 {
        static const float TWO_PI = 2.0f * 3.14159265358979323846f;
        std::uniform_real_distribution<float> distAngle(0.0f, glm::radians(halfAngleDeg));
        std::uniform_real_distribution<float> distAzimuth(0.0f, TWO_PI);

        float angle = distAngle(s_rng);
        float azimuth = distAzimuth(s_rng);

        glm::vec3 perpendicular;
        if (std::abs(direction.y) < 0.9f) {
            perpendicular = glm::vec3(0.0f, 1.0f, 0.0f);
        } else {
            perpendicular = glm::vec3(1.0f, 0.0f, 0.0f);
        }

        glm::vec3 tangent = glm::normalize(glm::cross(direction, perpendicular));
        glm::vec3 bitangent = glm::cross(direction, tangent);

        float x = std::sin(angle) * std::cos(azimuth);
        float y = std::sin(angle) * std::sin(azimuth);
        float z = std::cos(angle);

        glm::vec3 result = z * direction + x * tangent + y * bitangent;
        return glm::normalize(result);
    };

    // Position initiale
    p.position = worldPosition;

    float u = dist01(s_rng);
    float baseSpeed = branch.initialSpeed;

    // Variance simple
    float speedVar = branch.speedVariance;
    if (branch.visualMode == BranchDescriptor::VisualMode::Sparkle) {
        speedVar *= (1.0f + std::max(0.0f, branch.sparkleSpeedJitter));
    }
    float speedMultiplierVar = 1.0f + (u - 0.5f) * 2.0f * speedVar;

    // En émission étalée, l'ordre (progress) est la source principale de "tête"/"queue".
    // progress=0 : tête (plus rapide), progress=1 : queue (plus lente)
    const float progress = std::max(0.0f, std::min(1.0f, orderedProgress01));
    const bool isFront = (progress <= std::max(0.0f, std::min(1.0f, branch.frontPortion)));

    float uSpeed = dist01(s_rng);
    if (isFront) {
        uSpeed = Biased01(uSpeed, std::max(1.0f, branch.frontSpeedBias));
    } else {
        uSpeed *= std::max(0.0f, std::min(1.0f, branch.backSpeedScale));
    }

    // Map vers un multiplier autour de 1.0
    float speedMultiplier = 0.6f + 0.8f * uSpeed; // [0.6..1.4]
    baseSpeed *= speedMultiplierVar * speedMultiplier;

    glm::vec3 velocity = branch.direction * baseSpeed;

    // Appliquer spread angulaire (+ boost sparkle)
    float spread = branch.angularSpread;
    if (branch.visualMode == BranchDescriptor::VisualMode::Sparkle) {
        spread *= std::max(0.0f, branch.sparkleSpreadMult);
    }
    if (spread > 0.0f) {
        glm::vec3 spreadDir = RandomInConeLocal(branch.direction, spread * 0.5f);
        float speed = glm::length(velocity);
        velocity = spreadDir * speed;
    }

    // Sparkle: jitter additionnel de vitesse
    if (branch.visualMode == BranchDescriptor::VisualMode::Sparkle && branch.sparkleSpeedJitter > 0.0f) {
        float j = (dist01(s_rng) - 0.5f) * 2.0f * branch.sparkleSpeedJitter;
        velocity *= (1.0f + j);
    }

    p.velocity = velocity;

    // Drag
    float damp = branch.damping;
    if (branch.dampingVariance > 0.0f) {
        float mult = 1.0f + (dist01(s_rng) - 0.5f) * 2.0f * branch.dampingVariance;
        damp *= mult;
    }
    if (damp < 0.0f) damp = 0.0f;
    p.damping = damp;

    // Gravité (contrôle artistique)
    p.gravityScale = branch.gravityScale;
    p.updraft = branch.updraft;

    // Couleur
    p.color = branch.color;
    p.baseColor = branch.color;

    // Taille avec variance
    float sizeVar = 1.0f + (dist01(s_rng) - 0.5f) * 2.0f * branch.sizeVariance;
    p.size = branch.particleSize * sizeVar;

    // Durée de vie
    p.lifeTime = branch.lifetime;
    p.originalLifeTime = branch.lifetime;

    // Shape
    p.shapeId = branch.shapeId;

    // Fade
    p.shouldFade = branch.shouldFade;
    p.fadeStartRatio = branch.fadeStartRatio;

    // Trail
    p.trailEnabled = branch.trailEnabled;
    p.trailWidth = branch.trailWidth;
    p.trailDuration = branch.trailDuration;
    p.trailOpacity = branch.trailOpacity;
    p.trailFalloffPow = branch.trailFalloffPow;
    if (p.trailEnabled && p.trailDuration > 0.0f) {
        const float denom = static_cast<float>(ParticlePool::kTrailSamples - 1);
        p.trailSamplePeriod = (denom > 0.0f) ? (p.trailDuration / denom) : (1.0f / 60.0f);
        if (p.trailSamplePeriod < 1.0f / 240.0f) p.trailSamplePeriod = 1.0f / 240.0f;
        if (p.trailSamplePeriod > 1.0f / 10.0f)  p.trailSamplePeriod = 1.0f / 10.0f;
    } else {
        p.trailSamplePeriod = 1.0f / 60.0f;
    }
    p.trailSampleAccum = 0.0f;
    p.trailHead = 0;
    p.trailCount = 0;

    // Smoke / recursion
    p.smokeAmount = std::max(0.0f, std::min(1.0f, branch.smokeAmount));
    p.recursionDepthRemaining = std::max(0, branch.recursionDepth);
    p.recursionProb = std::max(0.0f, std::min(1.0f, branch.recursionProb));

    p.active = true;
}

int BranchGenerator::EmitParticle(
    const GeneratedBranch& branch,
    const PhysicsProfile& physics,
    const glm::vec3& worldPosition,
    ParticlePool& pool,
    int spawnIndex,
    int totalSpawns
)
{
    (void)physics;
    const int idx = pool.Allocate();
    if (idx < 0) return -1;

    const float denom = (totalSpawns > 1) ? static_cast<float>(totalSpawns - 1) : 1.0f;
    const float progress = std::max(0.0f, std::min(1.0f, static_cast<float>(spawnIndex) / denom));

    Particle& p = pool.Get(idx);
    InitParticleFromBranch(p, branch, worldPosition, progress);
    return idx;
}

std::vector<int> BranchGenerator::EmitBranch(
    const GeneratedBranch& branch,
    const PhysicsProfile& physics,
    const glm::vec3& worldPosition,
    ParticlePool& pool
)
{
    std::vector<int> indices;
    indices.reserve(branch.particlesPerBranch);

    for (int i = 0; i < branch.particlesPerBranch; ++i) {
        const int idx = EmitParticle(branch, physics, worldPosition, pool, i, branch.particlesPerBranch);
        if (idx < 0) break;
        indices.push_back(idx);
    }

    return indices;
}

glm::vec3 BranchGenerator::RandomInCone(const glm::vec3& direction, float halfAngleDeg)
{
    static const float TWO_PI = 2.0f * 3.14159265358979323846f;

    std::uniform_real_distribution<float> distAngle(0.0f, glm::radians(halfAngleDeg));
    std::uniform_real_distribution<float> distAzimuth(0.0f, TWO_PI);

    float angle = distAngle(s_rng);
    float azimuth = distAzimuth(s_rng);

    // Créer un vecteur perpendiculaire à direction
    glm::vec3 perpendicular;
    if (std::abs(direction.y) < 0.9f) {
        perpendicular = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    else {
        perpendicular = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 tangent = glm::normalize(glm::cross(direction, perpendicular));
    glm::vec3 bitangent = glm::cross(direction, tangent);

    // Rotation dans le cone
    float x = std::sin(angle) * std::cos(azimuth);
    float y = std::sin(angle) * std::sin(azimuth);
    float z = std::cos(angle);

    glm::vec3 result = z * direction + x * tangent + y * bitangent;
    return glm::normalize(result);
}