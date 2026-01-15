#include "TrailRenderer.h"

#include <glad/glad.h>

#include <algorithm>

#include "Shader.h"
#include "Camera.h"

#include "../fireworks/particle/ParticlePool.h"

TrailRenderer::TrailRenderer(Shader* s)
    : shader(s)
    , vao(0)
    , vbo(0)
    , ebo(0)
    , aspectRatio(16.0f / 9.0f)
    , alphaPower(1.5f)
    , baseOpacity(0.15f)
{
}

TrailRenderer::~TrailRenderer()
{
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
}

bool TrailRenderer::initialize()
{
    if (!shader) return false;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // layout(location=0) vec3 aPos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), (void*)0);
    // layout(location=1) vec4 aColor
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TrailVertex), (void*)offsetof(TrailVertex, color));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

static inline glm::vec3 CameraRightFromView(const glm::mat4& view)
{
    // View matrix rotates world into camera space. The camera basis in world space is the inverse rotation.
    glm::mat4 invView = glm::inverse(view);
    glm::vec3 right = glm::vec3(invView[0]);
    float len = glm::length(right);
    if (len > 0.0f) right /= len;
    return right;
}

void TrailRenderer::Render(const ParticlePool& pool, const Camera& camera, const glm::mat4& model)
{
    if (!shader || vao == 0 || vbo == 0 || ebo == 0) return;

    cpuVertices.clear();
    cpuIndices.clear();

    const glm::mat4 view = camera.getViewMatrix();
    const glm::vec3 camRight = CameraRightFromView(view);

    const auto& particles = pool.GetAll();
    cpuVertices.reserve(pool.GetActiveCount() * 8); // heuristic; avoids frequent realloc
    cpuIndices.reserve(pool.GetActiveCount() * 12);

    constexpr std::uint32_t kRestart = 0xFFFFFFFFu;
    std::uint32_t baseVertex = 0;

    for (size_t i = 0; i < particles.size(); ++i) {
        const auto& p = particles[i];
        if (!p.active) continue;
        if (!p.trailEnabled) continue;
        if (p.trailCount < 2) continue;
        if (p.trailWidth <= 0.0f) continue;

        const glm::vec3* buf = pool.GetTrailBuffer(static_cast<int>(i));

        // We want oldest->newest order.
        const int count = static_cast<int>(p.trailCount);
        const int head = static_cast<int>(p.trailHead);

        // Build one triangle strip per particle.
        // For simplicity, we approximate the ribbon normal with camera right.
        // This is visually stable and cheap; if you want true "segment perpendicular" ribbons,
        // compute per-segment perpendicular using segment direction and camera forward.

        const float falloffPow = std::max(1.0f, p.trailFalloffPow);
        const float opacity = std::max(0.0f, p.trailOpacity);

        for (int j = 0; j < count; ++j) {
            // Oldest sample index in ring buffer
            int ringIdx = head - (count - 1 - j);
            while (ringIdx < 0) ringIdx += ParticlePool::kTrailSamples;
            ringIdx %= ParticlePool::kTrailSamples;

            const glm::vec3 pos = buf[ringIdx];

            float u = (count <= 1) ? 1.0f : (static_cast<float>(j) / static_cast<float>(count - 1));
            // u=0 oldest (tail), u=1 newest (head)
            float a = std::pow(u, falloffPow);

            // Width taper (thinner at the tail)
            // Authoring convenience: if trailWidth <= 1, treat it as a fraction of particle size.
            // Otherwise keep it as world-space width.
            float baseW = p.trailWidth;
            if (baseW > 0.0f && baseW <= 1.0f) {
                baseW = (p.size * baseW) * 0.0025f; // heuristic mapping pixels -> world
            }
            float w = baseW * (0.25f + 0.75f * u);
            glm::vec3 off = camRight * w;

            glm::vec4 c = p.color;
            // Low-opacity trails: baseOpacity is an explicit visibility knob.
            c.a *= (baseOpacity * opacity * a);

            cpuVertices.push_back({ pos - off, c });
            cpuVertices.push_back({ pos + off, c });
        }

        // Indexed triangle strip + primitive restart => no accidental bridging between particles.
        const std::uint32_t vertCount = static_cast<std::uint32_t>(count) * 2u;
        for (std::uint32_t k = 0; k < vertCount; ++k) {
            cpuIndices.push_back(baseVertex + k);
        }
        cpuIndices.push_back(kRestart);
        baseVertex += vertCount;
    }

    if (cpuVertices.empty() || cpuIndices.empty()) return;

    shader->use();

    int width, height;
    // We only need aspect; caller already sets aspectRatio. Keep this renderer simple.
    (void)width; (void)height;
    glm::mat4 proj = camera.getProjectionMatrix(aspectRatio);
    shader->setMat4("view", view);
    shader->setMat4("projection", proj);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, cpuVertices.size() * sizeof(TrailVertex), cpuVertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cpuIndices.size() * sizeof(std::uint32_t), cpuIndices.data(), GL_DYNAMIC_DRAW);

    // Trails should be low opacity and stable: use standard alpha blending.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(kRestart);

    glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(cpuIndices.size()), GL_UNSIGNED_INT, (void*)0);

    glDisable(GL_PRIMITIVE_RESTART);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
