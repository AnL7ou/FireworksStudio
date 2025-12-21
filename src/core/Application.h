#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <memory>

#include "Window.h"
#include "OrbitalCameraController.h"
#include "InputRouter.h"
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
    Camera camera;

    // Managers / Services
    ParticleRenderer* renderer;
    OrbitalCameraController* cameraController;
	InputRouter* inputRouter;
    UIManager* uiManager;

    // State / Controllers
    Shader* shader;
    ParticleSystem* particleSystem;
    FireworkTemplate fwTemplate;
    FireworkInstance* fwInstance;

    // Fonctions d'initialisation factorisées
    bool InitializeWindow();
    bool InitializeShaderAndRenderer();
    bool InitializeParticleSystem();
    void InitializeFireworkTemplate();
    bool InitializeUI();
    void SetupGLStates();

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* w, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* w, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* w, double xoffset, double yoffset);

public:
    Application();
    ~Application();

    bool Initialize();
    int Run();
    void Shutdown();
};