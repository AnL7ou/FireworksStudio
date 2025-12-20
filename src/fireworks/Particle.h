#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;       // Couleur RGBA (R, G, B, Alpha)
    float lifeTime;        // Durée de vie restante de la particule
    float size;            // Taille de la particule (en unités que shader convertira en pixels)
    bool active;
    uint16_t shapeId = 0;  // index dans le ShapeRegistry (0 = Disk builtin par ex.)

    Particle()
        : position(0.0f, 0.0f, 0.0f)
        , velocity(0.0f, 0.0f, 0.0f)
        , color(1.0f, 1.0f, 1.0f, 1.0f)
        , lifeTime(1.0f)
        , size(8.0f)
        , active(false)
        , shapeId(0)
    {
    }
};