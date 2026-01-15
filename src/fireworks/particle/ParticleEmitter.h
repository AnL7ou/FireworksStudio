#pragma once

#include <glm/glm.hpp>
#include "ParticlePool.h"

// Émetteur générique de particules
// Simplifie l'émission de particules avec des paramètres de base
class ParticleEmitter {
public:
    struct EmissionParams {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec4 color;

        float speedMin;
        float speedMax;
        float spread;           // Angle de spread (degrés)

        float size;
        float sizeVariance;

        float lifetime;
        uint16_t shapeId;

        int count;              // Nombre de particules à émettre

        EmissionParams()
            : position(0.0f)
            , direction(0.0f, 1.0f, 0.0f)
            , color(1.0f)
            , speedMin(1.0f)
            , speedMax(5.0f)
            , spread(30.0f)
            , size(8.0f)
            , sizeVariance(0.2f)
            , lifetime(2.0f)
            , shapeId(0)
            , count(10)
        {
        }
    };

public:
    ParticleEmitter() = default;
    ~ParticleEmitter() = default;

    // Émet des particules selon les paramètres
    // Retourne les indices des particules émises
    static std::vector<int> Emit(const EmissionParams& params, ParticlePool& pool);

private:
    static glm::vec3 RandomInCone(const glm::vec3& direction, float halfAngleDeg);
};