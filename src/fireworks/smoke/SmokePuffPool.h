#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "SmokePuff.h"

class SmokePuffPool {
public:
    explicit SmokePuffPool(size_t capacity = 5000);

    void Update(float dt);

    // amount: 0..1
    void Spawn(const glm::vec3& pos, float amount);

    const std::vector<SmokePuff>& GetAll() const { return puffs; }
    std::vector<SmokePuff>& GetAll() { return puffs; }

private:
    std::vector<SmokePuff> puffs;
    size_t lastIndex = 0;
};
