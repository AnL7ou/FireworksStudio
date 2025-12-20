#include "Application.h"
#include "../fireworks/ShapeRegistry.h" // ajout

// file-scope pointer pour éviter de toucher Application.h
static ShapeRegistry * g_shapeRegistry = nullptr;

Application::Application()
    : shader(nullptr)
    , renderer(nullptr)
    , particleSystem(nullptr)
    , fwInstance(nullptr)
    , uiManager(nullptr)
{
}

Application::~Application()
{
    Shutdown();
}

bool Application::Initialize()
{
    if (!InitializeWindow())
        return false;

    if (!InitializeShaderAndRenderer())
        return false;

    if (!InitializeParticleSystem())
        return false;

    InitializeFireworkTemplate();

    if (!InitializeUI())
        return false;

    SetupGLStates();

    return true;
}

bool Application::InitializeWindow()
{
    if (!window.Create(1200, 720, "Fireworks Demo"))
    {
        std::cerr << "Failed to create window\n";
        return false;
    }

    // ========== AJOUTER CES LIGNES ==========
    // Stocker le pointeur vers Application pour la callback
    glfwSetWindowUserPointer(window.GetWindow(), this);

    // Enregistrer la callback de redimensionnement
    glfwSetFramebufferSizeCallback(window.GetWindow(), framebufferSizeCallback);
    // =========================================

    return true;
}

bool Application::InitializeShaderAndRenderer()
{
    // Shader
    shader = new Shader("../shaders/particle.vert", "../shaders/particle.frag");

    // Renderer
    renderer = new ParticleRenderer(shader);
    if (!renderer->initialize())
    {
        std::cerr << "Failed to initialize particle renderer\n";
        return false;
    }

    // ========== AJOUTER CES LIGNES ==========
    // Configurer l'aspect ratio initial
    int width, height;
    glfwGetFramebufferSize(window.GetWindow(), &width, &height);
    float aspectRatio = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 16.0f / 9.0f;
    renderer->SetAspectRatio(aspectRatio);
    std::cerr << "ParticleRenderer aspect ratio set to: " << aspectRatio << " (" << width << "x" << height << ")\n";
    // =========================================

    // Initialise et branche le ShapeRegistry APRÈS l'init du contexte GL / renderer
    if (!g_shapeRegistry) {
        g_shapeRegistry = new ShapeRegistry();
        if (!g_shapeRegistry->Initialize()) {
            std::cerr << "Warning: ShapeRegistry::Initialize() failed\n";
        }
    }
    renderer->SetShapeRegistry(g_shapeRegistry);

    return true;
}

bool Application::InitializeParticleSystem()
{
    particleSystem = new ParticleSystem(4000);
    if (!particleSystem)
    {
        std::cerr << "Failed to create particle system\n";
        return false;
    }
    return true;
}

void Application::InitializeFireworkTemplate()
{
    fwTemplate.particlesCount = 800;
    fwTemplate.particleLifeTime = 3.0f;
    fwTemplate.explosionRadius = 0.1f;
    fwTemplate.baseColor = glm::vec4(1.0f, 0.1f, 0.25f, 1.0f);

    // DEBUG : taille visible en pixels (à réduire ensuite)
    fwTemplate.baseSize = 100.0f; // 16 px = visible, replace par 8/12 en production

    fwTemplate.speedMin = 1.0f;
    fwTemplate.speedMax = 4.0f;

    // Instance déclenchée plus tard (optionnel)
    if (fwInstance)
    {
        delete fwInstance;
        fwInstance = nullptr;
    }
    fwInstance = new FireworkInstance(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, &fwTemplate);
}

bool Application::InitializeUI()
{
    uiManager = new UIManager();
    if (!uiManager->Initialize(window.GetWindow()))
    {
        std::cerr << "Failed to initialize UI manager\n";
        return false;
    }

    // Créer et lier le panneau de propriétés au template courant
    uiManager->CreateTemplatePanel(&fwTemplate);
    auto panel = uiManager->GetTemplatePanel();
    if (panel)
    {
        // Test explosion via callback
        panel->SetOnExplosionTestCallback([this](const FireworkTemplate& t) {
            // explosion au centre du monde
            if (this->particleSystem)
                this->particleSystem->emit(t, glm::vec3(0.0f, 0.0f, 0.0f));
            });
    }
    return true;
}

void Application::SetupGLStates()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // DEBUG : on peut désactiver le depth test temporairement pour vérifier
    // glEnable(GL_DEPTH_TEST);
    // Pour debug garder désactivé si nécessaire
    // glDisable(GL_DEPTH_TEST);
}

int Application::Run()
{
    float lastTime = static_cast<float>(glfwGetTime());
    while (!window.ShouldClose())
    {
        float now = static_cast<float>(glfwGetTime());
        float delta = now - lastTime;
        lastTime = now;

        window.PollEvents();

        // Start ImGui frame
        uiManager->NewFrame();

        // Update
        if (fwInstance)
        {
            fwInstance->update(now, *particleSystem);

            if (fwInstance->IsTriggered())
            {
                delete fwInstance;
                fwInstance = nullptr;
            }
        }

        particleSystem->update(delta);

        // Render 3D
        glClearColor(1.0f, 0.0f, 1.0f, 1.0f); // magenta pour debug (remettre plus tard)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer->Render(particleSystem->getPaticles(), camera);

        // Render ImGui (panels)
        uiManager->Render();

        window.SwapBuffers();
    }

    return 0;
}

void Application::Shutdown()
{
    // Free owned resources
    delete fwInstance;
    fwInstance = nullptr;

    delete particleSystem;
    particleSystem = nullptr;

    delete renderer;
    renderer = nullptr;

    delete shader;
    shader = nullptr;

    // delete global registry
    if (g_shapeRegistry) {
        delete g_shapeRegistry;
        g_shapeRegistry = nullptr;
    }

    if (uiManager)
    {
        uiManager->Shutdown();
        delete uiManager;
        uiManager = nullptr;
    }

    window.Destroy();

    // Terminate GLFW globally
    glfwTerminate();
}


void Application::framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
    glViewport(0, 0, width, height);

    // Récupérer l'instance Application depuis le user pointer
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(glfwWindow));
    if (app && app->renderer) {
        float aspectRatio = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 16.0f / 9.0f;
        app->renderer->SetAspectRatio(aspectRatio);
    }
}