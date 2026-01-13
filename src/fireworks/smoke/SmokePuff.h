#pragma once

#include <glm/glm.hpp>

struct SmokePuff {
    glm::vec3 position{0.0f};
    glm::vec3 velocity{0.0f};
    float     age = 0.0f;
    float     life = 1.5f;
    float     size = 0.25f;
    float     opacity = 0.08f;
    float     damping = 2.0f; // 1/s
    bool      active = false;
};
