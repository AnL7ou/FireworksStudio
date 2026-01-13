#include "Application.h"

#include <imgui.h>

#include "../fireworks/shapes/ShapeRegistry.h"
#include "../ui/EditorMode.h"
#include "../ui/panels/template_editor/TemplatePropertiesPanel.h"

// file-scope pointer
static ShapeRegistry* g_shapeRegistry = nullptr;

Application::Application()
    : camera(
        /*position*/glm::vec3(0.0f, 5.0f, 15.0f),
        /*focus*/   glm::vec3(0.0f, 0.0f, 0.0f),
        /*up*/      glm::vec3(0.0f, 1.0f, 0.0f)
    )
    , renderer(nullptr)
    , trailRenderer(nullptr)
    , cameraController(nullptr)
    , inputRouter(nullptr)
    , uiManager(nullptr)
    , shader(nullptr)
    , trailShader(nullptr)
    , particlePool(nullptr)
    , templateLibrary(nullptr)
    , instanceManager(nullptr)
    , scene(nullptr)
    , timeline(nullptr)
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

    if (!InitializeParticlePool())
        return false;

    InitializeFireworkTemplates();

    if (!InitializeUI())
        return false;

    SetupGLStates();

    return true;
}

bool Application::InitializeWindow()
{
    if (!window.Create(1900, 1080, "Fireworks Studio"))
    {
        std::cerr << "Failed to create window\n";
        return false;
    }

    glfwSetWindowUserPointer(window.GetWindow(), this);
    glfwSetFramebufferSizeCallback(window.GetWindow(), framebufferSizeCallback);

    return true;
}

bool Application::InitializeShaderAndRenderer()
{
    shader = new Shader("../shaders/particle.vert", "../shaders/particle.frag");
    renderer = new ParticleRenderer(shader);

    trailShader = new Shader("../shaders/trail.vert", "../shaders/trail.frag");
    trailRenderer = new TrailRenderer(trailShader);

    if (!renderer->initialize())
    {
        std::cerr << "Failed to initialize particle renderer\n";
        return false;
    }

    int width, height;
    glfwGetFramebufferSize(window.GetWindow(), &width, &height);
    float aspectRatio = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 16.0f / 9.0f;
    renderer->SetAspectRatio(aspectRatio);

    if (!trailRenderer->initialize())
    {
        std::cerr << "Failed to initialize trail renderer\n";
        return false;
    }
    trailRenderer->SetAspectRatio(aspectRatio);

    if (!g_shapeRegistry) {
        g_shapeRegistry = new ShapeRegistry();
        if (!g_shapeRegistry->Initialize()) {
            std::cerr << "Warning: ShapeRegistry::Initialize() failed\n";
        }
    }
    renderer->SetShapeRegistry(g_shapeRegistry);

    return true;
}

bool Application::InitializeParticlePool()
{
    particlePool = new ParticlePool(500000);
    if (!particlePool)
    {
        std::cerr << "Failed to create particle pool\n";
        return false;
    }
    std::cerr << "Particle pool created with capacity: " << particlePool->GetCapacity() << "\n";
    return true;
}

void Application::InitializeFireworkTemplates()
{
    templateLibrary = new TemplateLibrary();
    templateLibrary->SeedPresets();

    instanceManager = new InstanceManager();

    scene = new Scene("Untitled Scene");
    timeline = new Timeline();
}

bool Application::InitializeUI()
{
    GLFWwindow* w = window.GetWindow();

    uiManager = new UIManager();
    if (!uiManager->Initialize(w))
    {
        std::cerr << "Failed to initialize UI manager\n";
        return false;
    }

    // Panels
    uiManager->CreateTemplatePanels(templateLibrary);
    uiManager->CreateScenePanels(scene, timeline, templateLibrary);

    // Configurer le callback de test d'explosion
    auto* panel = uiManager->GetTemplatePanel();
    if (panel) {
        panel->SetOnExplosionTestCallback([this](const FireworkTemplate& t) {
            float now = static_cast<float>(glfwGetTime());
            FireworkTemplate* active = templateLibrary ? templateLibrary->GetActive() : nullptr;
            if (!active || !instanceManager) {
                return;
            }
            auto* instance = new FireworkInstance(active, glm::vec3(0.0f, 0.0f, 0.0f), now + 0.5f);
            instanceManager->AddInstance(instance);
            std::cerr << "[UI] Test explosion triggered (instance added)\n";
            });
    }

    // Input system
    inputRouter = new InputRouter();
    cameraController = new OrbitalCameraController(camera);
    inputRouter->AddListener(cameraController);

    glfwSetMouseButtonCallback(w, mouseButtonCallback);
    glfwSetCursorPosCallback(w, cursorPosCallback);
    glfwSetScrollCallback(w, scrollCallback);

    return true;
}

void Application::SetupGLStates()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
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

        // Scene playback (timeline time is independent from glfw time)
        if (timeline && scene && uiManager && uiManager->GetMode() == EditorMode::Scene) {
            float prev = timeline->GetLastDispatchedTime();
            float duration = scene->GetDuration();
            timeline->Update(delta, duration);
            float cur = timeline->GetTime();
            if (timeline->IsPlaying()) {
                const auto& events = scene->GetEvents();
                for (const auto& e : events) {
                    if (!e.enabled) continue;
                    if (e.triggerTime > prev && e.triggerTime <= cur) {
                        if (instanceManager && templateLibrary) {
                            FireworkTemplate* t = templateLibrary->Get(e.templateId);
                            if (t) {
                                auto* inst = new FireworkInstance(t, e.position, now);
                                instanceManager->AddInstance(inst);
                            }
                        }
                    }
                }
                timeline->SetLastDispatchedTime(cur);
            }
        }

        // Update all active firework instances
        if (instanceManager) {
            instanceManager->Update(now, delta, *particlePool);
        }

        // Update particle pool
        particlePool->Update(delta);

        // Render 3D
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render particles
        if (trailRenderer) {
            trailRenderer->Render(*particlePool, camera);
        }
        renderer->Render(particlePool->GetAll(), camera);

        // Render ImGui
        uiManager->Render();

        window.SwapBuffers();
    }

    return 0;
}

void Application::Shutdown()
{
    delete timeline;
    timeline = nullptr;

    delete scene;
    scene = nullptr;

    if (instanceManager) {
        instanceManager->Clear();
        delete instanceManager;
        instanceManager = nullptr;
    }

    delete templateLibrary;
    templateLibrary = nullptr;

    delete particlePool;
    particlePool = nullptr;

    delete renderer;
    renderer = nullptr;

    delete trailRenderer;
    trailRenderer = nullptr;

    delete shader;
    shader = nullptr;

    delete trailShader;
    trailShader = nullptr;

    delete cameraController;
    cameraController = nullptr;

    delete inputRouter;
    inputRouter = nullptr;

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
    glfwTerminate();
}

void Application::framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height)
{
    glViewport(0, 0, width, height);

    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(glfwWindow));
    if (app && app->renderer) {
        float aspectRatio = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 16.0f / 9.0f;
        app->renderer->SetAspectRatio(aspectRatio);
    }
}

void Application::mouseButtonCallback(GLFWwindow* w, int button, int action, int mods)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
    if (!app || !app->inputRouter) return;
    app->inputRouter->DispatchMouseButton(w, button, action, mods);
}

void Application::cursorPosCallback(GLFWwindow* w, double xpos, double ypos)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
    if (!app || !app->inputRouter) return;
    app->inputRouter->DispatchCursorPos(w, xpos, ypos);
}

void Application::scrollCallback(GLFWwindow* w, double xoffset, double yoffset)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(w));
    if (!app || !app->inputRouter) return;
    app->inputRouter->DispatchScroll(w, xoffset, yoffset);
}