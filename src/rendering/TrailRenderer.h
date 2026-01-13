#pragma once

#include <vector>
#include <cstdint>

#include <glm/glm.hpp>

class Shader;
class Camera;
class ParticlePool;

// Renders camera-facing ribbon trails using per-particle position history
// stored in ParticlePool.
class TrailRenderer {
public:
    explicit TrailRenderer(Shader* shader);
    ~TrailRenderer();

    bool initialize();

    void SetAspectRatio(float aspect) { aspectRatio = aspect; }
    void Render(const ParticlePool& pool, const Camera& camera);

    // Global tuning (kept minimal; per-particle settings come from Particle fields)
    void SetAlphaPower(float p) { alphaPower = p; }

    // Multiplicative opacity applied to all trails (0..1). This is intentionally
    // renderer-level so you can quickly tune visibility without changing data models.
    void SetBaseOpacity(float o) { baseOpacity = o; }

private:
    struct TrailVertex {
        glm::vec3 position;
        glm::vec4 color;
    };

    Shader* shader;
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    float aspectRatio;
    float alphaPower;
    float baseOpacity;

    std::vector<TrailVertex> cpuVertices;
    std::vector<std::uint32_t> cpuIndices;
};
