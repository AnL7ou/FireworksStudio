#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>

#include "Window.h"
#include "../rendering/Camera.h"
#include "../rendering/ParticleRenderer.h"
#include "../fireworks/Particle.h"
#include "../fireworks/ParticleSystem.h"
#include "../fireworks/FireworkTemplate.h"
#include "../fireworks/FireworkInstance.h"
#include "../rendering/Shader.h"
#include "../ui/UIManager.h"

// Forward declare UI manager to éviter d'exposer ImGui ici
namespace ui { class UIManager; }

class Application {
private:
    Window window;
    Shader* shader;
    ParticleRenderer* renderer;
    ParticleSystem* particleSystem;
    FireworkTemplate fwTemplate;
    FireworkInstance* fwInstance;
    Camera camera;

    // UI manager responsable de tout ImGui (séparation claire)
    UIManager* uiManager;

    // Fonctions d'initialisation factorisées
    bool InitializeWindow();
    bool InitializeShaderAndRenderer();
    bool InitializeParticleSystem();
    void InitializeFireworkTemplate();
    bool InitializeUI();
    void SetupGLStates();
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

public:
    Application();
    ~Application();

    bool Initialize();
    int Run();
    void Shutdown();
};