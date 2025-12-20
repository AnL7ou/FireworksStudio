#include "ParticleRenderer.h"

ParticleRenderer::ParticleRenderer()
    : VAO(0), VBO(0), shader(nullptr), shapeRegistry(nullptr), aspectRatio(16.0f / 9.0f)
{
}

ParticleRenderer::ParticleRenderer(Shader* _shader)
    : VAO(0), VBO(0), shader(_shader), shapeRegistry(nullptr), aspectRatio(16.0f / 9.0f)
{
}

ParticleRenderer::~ParticleRenderer()
{
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
}

bool ParticleRenderer::initialize()
{
    // Configure VAO/VBO pour attributes (vec3 pos, vec4 color, float size) => 8 floats stride
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // reserve small initial size
    glBufferData(GL_ARRAY_BUFFER, 1024, nullptr, GL_DYNAMIC_DRAW);

    constexpr GLsizei stride = 8 * sizeof(float);
    // pos (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    // color (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    // size (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void ParticleRenderer::SetShapeRegistry(ShapeRegistry* registry)
{
    shapeRegistry = registry;
}

void ParticleRenderer::Render(const std::vector<Particle>& particles, const Camera& camera)
{
    if (!shader) return;

    // Grouper les particules par shapeId pour minimiser les changements de texture
    std::unordered_map<uint16_t, std::vector<const Particle*>> groups;
    for (const auto& p : particles) {
        if (!p.active) continue;
        groups[p.shapeId].push_back(&p);
    }
    if (groups.empty()) return;

    shader->use();

    // Utiliser les vraies matrices de la caméra
    shader->setMat4("view", camera.getViewMatrix());
    shader->setMat4("projection", camera.getProjectionMatrix(aspectRatio));
    shader->setVec3("uCameraPos", camera.getPosition());

    // Définir l'unité de texture utilisée (texture unit 0)
    shader->setInt("uTexture", 0);

    // Configurer l'état OpenGL pour le rendu des particules
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Désactiver le depth test pour que les particules se superposent naturellement
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Rendre chaque groupe de particules (un draw call par texture)
    for (auto& kv : groups) {
        uint16_t shapeId = kv.first;
        const auto& vec = kv.second;
        if (vec.empty()) continue;

        // Construire le buffer de vertices pour ce groupe
        std::vector<float> data;
        data.reserve(vec.size() * 8);
        for (const Particle* p : vec) {
            data.push_back(p->position.x);
            data.push_back(p->position.y);
            data.push_back(p->position.z);
            data.push_back(p->color.r);
            data.push_back(p->color.g);
            data.push_back(p->color.b);
            data.push_back(p->color.a);
            data.push_back(p->size);
        }

        // Upload vers GPU
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_DYNAMIC_DRAW);

        // Obtenir la texture pour cette forme
        GLuint texId = 0;
        if (shapeRegistry) {
            texId = shapeRegistry->GetTextureId(shapeId);
        }
        
        // Configurer texture et uniforms
        if (texId == 0) {
            // Pas de texture: rendu en couleur unie
            shader->setInt("uHasTexture", 0);
            shader->setInt("uUseMask", 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            // Avec texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texId);
            shader->setInt("uHasTexture", 1);

            // uUseMask: 
            // false = utilise le canal alpha de la texture (recommandé pour RGBA)
            // true = utilise canal rouge comme alpha (pour textures grayscale)
            shader->setInt("uUseMask", 0);
        }

        // Draw call
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(data.size() / 8));

        // Débinder texture après utilisation
        if (texId != 0) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Restaurer l'état OpenGL
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}