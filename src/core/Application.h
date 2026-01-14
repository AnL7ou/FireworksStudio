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
#include "../rendering/TrailRenderer.h"
#include "../fireworks/particle/Particle.h"
#include "../fireworks/particle/ParticlePool.h"
#include "../fireworks/template/FireworkTemplate.h"
#include "../fireworks/instance/FireworkInstance.h"
#include "../fireworks/instance/InstanceManager.h"
#include "../fireworks/template/TemplateLibrary.h"
#include "../scene/Scene.h"
#include "../scene/Timeline.h"
#include "../scene/ScenePlacementController.h"
#include "../rendering/Shader.h"
#include "../ui/UIManager.h"

// Forward declare UI manager
class UIManager;

class Application {
private:
    Window window;
    Camera camera;

    // Managers / Services
    ParticleRenderer* renderer;
    TrailRenderer* trailRenderer;
    OrbitalCameraController* cameraController;
    ScenePlacementController* scenePlacementController;
    InputRouter* inputRouter;
    UIManager* uiManager;

    // State / Controllers
    Shader* shader;
    Shader* trailShader;
    ParticlePool* particlePool;
    TemplateLibrary* templateLibrary;
    InstanceManager* instanceManager;

    Scene* scene;
    Timeline* timeline;

    // Scene-mode paroxysm preview cache (avoid re-simulating every frame)
    bool lastTimelinePlaying;
    uint64_t scenePreviewKey;
    bool scenePreviewValid;

    uint64_t ComputeScenePreviewKey() const;
    void RebuildSceneParoxysmPreview(float nowSeconds);

    // Fonctions d'initialisation factorisées
    bool InitializeWindow();
    bool InitializeShaderAndRenderer();
    bool InitializeParticlePool();
    void InitializeFireworkTemplates();
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