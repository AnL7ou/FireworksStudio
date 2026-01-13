#include "ParticleEmitter.h"
#include <random>
#include <cmath>

static std::mt19937 s_rng(std::random_device{}());

// Constante pour 2*PI
static const float TWO_PI = 2.0f * 3.14159265358979323846f;

std::vector<int> ParticleEmitter::Emit(const EmissionParams& params, ParticlePool& pool)
{
    std::vector<int> indices;
    indices.reserve(params.count);

    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
    std::uniform_real_distribution<float> distSpeed(params.speedMin, params.speedMax);

    for (int i = 0; i < params.count; ++i) {
        int idx = pool.Allocate();
        if (idx < 0) break;  // Pool plein

        Particle& p = pool.Get(idx);

        // Position
        p.position = params.position;

        // Vélocité avec spread
        glm::vec3 dir = params.direction;
        if (params.spread > 0.0f) {
            dir = RandomInCone(dir, params.spread * 0.5f);
        }
        float speed = distSpeed(s_rng);
        p.velocity = dir * speed;

        // Couleur
        p.color = params.color;
        p.baseColor = params.color;

        // Taille
        float sizeVar = 1.0f + (dist01(s_rng) - 0.5f) * 2.0f * params.sizeVariance;
        p.size = params.size * sizeVar;

        // Lifetime
        p.lifeTime = params.lifetime;
        p.originalLifeTime = params.lifetime;

        // Shape
        p.shapeId = params.shapeId;

        // Fade
        p.shouldFade = true;
        p.fadeStartRatio = 0.7f;

        // Activer
        p.active = true;

        indices.push_back(idx);
    }

    return indices;
}

glm::vec3 ParticleEmitter::RandomInCone(const glm::vec3& direction, float halfAngleDeg)
{
    std::uniform_real_distribution<float> distAngle(0.0f, glm::radians(halfAngleDeg));
    std::uniform_real_distribution<float> distAzimuth(0.0f, TWO_PI);

    float angle = distAngle(s_rng);
    float azimuth = distAzimuth(s_rng);

    // Créer un vecteur perpendiculaire
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

    return glm::normalize(z * direction + x * tangent + y * bitangent);
}