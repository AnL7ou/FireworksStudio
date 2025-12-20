#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "Shader.h"
#include "Camera.h"
#include "../fireworks/Particle.h"
#include "../fireworks/ShapeRegistry.h"

class ParticleRenderer {
private:
    unsigned int VAO;
    unsigned int VBO;
    Shader* shader;
    ShapeRegistry* shapeRegistry;

    // Aspect ratio pour la projection (mis à jour depuis l'extérieur)
    float aspectRatio;

public:
    ParticleRenderer();
    ParticleRenderer(Shader* _shader);
    ~ParticleRenderer();

    bool initialize();
    void SetShapeRegistry(ShapeRegistry* registry);

    // Définir l'aspect ratio (appelé depuis Application quand la fenêtre change)
    void SetAspectRatio(float aspect) { aspectRatio = aspect; }

    // Méthode principale de rendu
    void Render(const std::vector<Particle>& particles, const Camera& camera);
};