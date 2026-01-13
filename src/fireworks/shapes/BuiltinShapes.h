#pragma once

#include <glm/glm.hpp>

enum class CurveType {
    Straight,
    Arc,
    Wave,
    Spiral
};

class CurvePrototype {
public:
    CurveType type = CurveType::Straight;

    float length = 1.0f;
    float curvature = 0.0f;
    float torsion = 0.0f;
    float frequency = 1.0f;

    glm::vec3 evaluate(float t) const;
    glm::vec3 tangent(float t) const;
};
