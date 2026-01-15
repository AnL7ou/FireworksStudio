#include "Application.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

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
    , scenePlacementController(nullptr)
    , templateRotationController(nullptr)
    , inputRouter(nullptr)
    , uiManager(nullptr)
    , shader(nullptr)
    , trailShader(nullptr)
    , particlePool(nullptr)
    , templateLibrary(nullptr)
    , instanceManager(nullptr)
    , scene(nullptr)
    , timeline(nullptr)
    , lastTimelinePlaying(false)
    , scenePreviewKey(0)
    , scenePreviewValid(false)
    , templatePreviewKey(0)
    , templatePreviewValid(false)
    , templatePreviewVersion(0)
    , templatePreviewBakedRotation(0.0f, 0.0f, 0.0f)
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
            // Ensure we don't mix idle preview state with a live test.
            templatePreviewValid = false;
            if (instanceManager) instanceManager->Clear();
            if (particlePool) particlePool->ClearAll();
            // Immediate start: the template editor's "Test" should be responsive.
            auto* instance = new FireworkInstance(active, glm::vec3(0.0f, 0.0f, 0.0f), now);
            instanceManager->AddInstance(instance);
            std::cerr << "[UI] Test explosion triggered (instance added)\n";
            });

        // Any template edit invalidates the idle preview cache.
        panel->SetOnTemplateChangedCallback([this](const FireworkTemplate&) {
            templatePreviewValid = false;
            ++templatePreviewVersion;
        });
    }

    // Input system
    inputRouter = new InputRouter();
    cameraController = new OrbitalCameraController(camera);
    inputRouter->AddListener(cameraController);

    // Scene manipulation (select + drag firework events in the 3D view)
    if (uiManager && scene) {
        scenePlacementController = new ScenePlacementController(camera, *uiManager, *scene);
        inputRouter->AddListener(scenePlacementController);
    }

    // Template manipulation (rotate active template directly in the 3D view)
    if (uiManager && templateLibrary) {
        templateRotationController = new TemplateRotationController(*uiManager, *templateLibrary);
        inputRouter->AddListener(templateRotationController);
    }

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
            // If we just switched to Play, discard any preview state so playback is deterministic.
            const bool playingNow = timeline->IsPlaying();
            if (!lastTimelinePlaying && playingNow) {
                if (instanceManager) instanceManager->Clear();
                if (particlePool) particlePool->ClearAll();
                scenePreviewValid = false;
            }
            lastTimelinePlaying = playingNow;

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

        // --- Paroxysm preview in Scene mode (no need to press Play) ---
        // Rebuild only when selection/template/position changes.
        if (uiManager && uiManager->GetMode() == EditorMode::Scene && timeline && !timeline->IsPlaying()) {
            const uint64_t key = ComputeScenePreviewKey();
            if (!scenePreviewValid || key != scenePreviewKey) {
                scenePreviewKey = key;
                RebuildSceneParoxysmPreview(now);
            }
        }

        // --- Idle preview in Template mode (no need to press "Test") ---
        // Only when nothing is currently happening (no live particles/instances).
        if (uiManager && uiManager->GetMode() == EditorMode::Template && particlePool && instanceManager) {
            const bool idle = (particlePool->GetActiveCount() == 0 && instanceManager->GetActiveCount() == 0);
            if (idle) {
                const uint64_t key = ComputeTemplatePreviewKey();
                if (!templatePreviewValid || key != templatePreviewKey) {
                    templatePreviewKey = key;
                    RebuildTemplateParoxysmPreview(now);
                }
            } else {
				// If particles are present but there are no active instances, this is our frozen
				// preview snapshot: keep it alive (and it will be excluded from simulation update).
				// Only invalidate when a live test is running (instances > 0).
				if (instanceManager->GetActiveCount() > 0) {
					templatePreviewValid = false;
				}
            }
        }

        // Update all active firework instances
        // Note: in Scene mode preview (paroxysm), we keep a frozen cache and do not advance simulation.
        if (instanceManager && particlePool) {
            const bool inScenePreview = (uiManager && uiManager->GetMode() == EditorMode::Scene && timeline && !timeline->IsPlaying());
            const bool inTemplatePreview = (uiManager && uiManager->GetMode() == EditorMode::Template && templatePreviewValid && particlePool->GetActiveCount() > 0 && instanceManager->GetActiveCount() == 0);
            if (!inScenePreview && !inTemplatePreview) {
                instanceManager->Update(now, delta, *particlePool);
                particlePool->Update(delta);
            }
        }

        // Render 3D
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
// Global model transform used for frozen previews (so rotation can update in real time without resimulation)
glm::mat4 modelMat(1.0f);
if (uiManager && templateLibrary) {
    const bool inTemplatePreview = (uiManager->GetMode() == EditorMode::Template && particlePool && particlePool->GetActiveCount() > 0 && instanceManager && instanceManager->GetActiveCount() == 0);
    if (inTemplatePreview) {
        const FireworkTemplate* t = templateLibrary->GetActive();
        const glm::vec3 cur = t ? t->worldRotation : glm::vec3(0.0f);
        const glm::vec3 baked = templatePreviewBakedRotation;

        auto rotMat = [](const glm::vec3& eulerDeg) {
            glm::mat4 m(1.0f);
            m = glm::rotate(m, glm::radians(eulerDeg.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Yaw
            m = glm::rotate(m, glm::radians(eulerDeg.x), glm::vec3(1.0f, 0.0f, 0.0f)); // Pitch
            m = glm::rotate(m, glm::radians(eulerDeg.z), glm::vec3(0.0f, 0.0f, 1.0f)); // Roll
            return m;
        };

        // Apply only the delta between current rotation and the rotation baked into the snapshot.
        modelMat = rotMat(cur) * glm::inverse(rotMat(baked));
    }
}

// Render particles
        if (trailRenderer) {
            trailRenderer->Render(*particlePool, camera, modelMat);
        }
        renderer->Render(particlePool->GetAll(), camera, modelMat);

        // Render ImGui
        uiManager->Render();

        window.SwapBuffers();
    }

    return 0;
}

static inline uint64_t HashCombine64(uint64_t h, uint64_t v)
{
    // 64-bit variant of boost::hash_combine
    return h ^ (v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2));
}

uint64_t Application::ComputeScenePreviewKey() const
{
    if (!uiManager || !scene) return 0;
    if (uiManager->GetMode() != EditorMode::Scene) return 0;

    const int sel = uiManager->GetSelectedSceneEventIndex();
    const auto& events = scene->GetEvents();
    if (sel < 0 || sel >= static_cast<int>(events.size())) return 1; // valid but empty selection

    const auto& e = events[static_cast<size_t>(sel)];

    uint64_t h = 1469598103934665603ULL; // FNV offset basis
    h = HashCombine64(h, static_cast<uint64_t>(sel));
    h = HashCombine64(h, static_cast<uint64_t>(e.templateId));
    h = HashCombine64(h, static_cast<uint64_t>(e.enabled ? 1 : 0));

    // Quantize position to avoid rebuilds from tiny float jitter.
    auto q = [](float x) -> int32_t { return static_cast<int32_t>(std::round(x * 1000.0f)); };
    h = HashCombine64(h, static_cast<uint64_t>(static_cast<uint32_t>(q(e.position.x))));
    h = HashCombine64(h, static_cast<uint64_t>(static_cast<uint32_t>(q(e.position.y))));
    h = HashCombine64(h, static_cast<uint64_t>(static_cast<uint32_t>(q(e.position.z))));

    return h;
}

void Application::RebuildSceneParoxysmPreview(float nowSeconds)
{
    scenePreviewValid = true;

    if (!instanceManager || !particlePool || !scene || !templateLibrary || !uiManager || !timeline) {
        if (instanceManager) instanceManager->Clear();
        if (particlePool) particlePool->ClearAll();
        return;
    }
    if (uiManager->GetMode() != EditorMode::Scene) return;
    if (timeline->IsPlaying()) return;

    const int sel = uiManager->GetSelectedSceneEventIndex();
    const auto& events = scene->GetEvents();

    instanceManager->Clear();
    particlePool->ClearAll();

    if (sel < 0 || sel >= static_cast<int>(events.size())) {
        return;
    }

    const auto& e = events[static_cast<size_t>(sel)];
    if (!e.enabled) return;

    FireworkTemplate* t = templateLibrary->Get(e.templateId);
    if (!t) return;

    // Estimate a visually meaningful peak: after emission, around mid-life.
    const float emission = std::max(0.0f, t->branchTemplate.emissionDuration);
    const float life = std::max(0.0f, t->branchTemplate.lifetime);
    float peakOffset = emission + 0.5f * life;

    // Keep within a sane budget.
    if (peakOffset < 0.1f) peakOffset = 0.1f;
    if (peakOffset > 4.0f) peakOffset = 4.0f; // tighter cap to avoid stalls

    const float startTime = nowSeconds - peakOffset;
    auto* inst = new FireworkInstance(t, e.position, startTime);
    instanceManager->AddInstance(inst);

    // Simulate forward to "nowSeconds" with a bounded step count.
    const float step = 1.0f / 30.0f;
    const int maxSteps = 120;
    int steps = static_cast<int>(std::ceil(peakOffset / step));
    if (steps > maxSteps) steps = maxSteps;

    float tcur = startTime;
    const float target = nowSeconds;
    for (int s = 0; s < steps; ++s) {
        float dt = step;
        if (tcur + dt > target) dt = std::max(0.0f, target - tcur);
        instanceManager->Update(tcur + dt, dt, *particlePool);
        particlePool->Update(dt);
        tcur += dt;
        if (dt <= 0.0f) break;
    }
}

uint64_t Application::ComputeTemplatePreviewKey() const
{
    if (!uiManager || !templateLibrary) return 0;
    if (uiManager->GetMode() != EditorMode::Template) return 0;

    // The active template id is stable. `templatePreviewVersion` increments whenever the template is edited.
    uint64_t h = 1469598103934665603ULL;
    h = HashCombine64(h, static_cast<uint64_t>(templateLibrary->GetActiveId()));
    h = HashCombine64(h, static_cast<uint64_t>(templatePreviewVersion));
    return h;
}

void Application::RebuildTemplateParoxysmPreview(float nowSeconds)
{
    templatePreviewValid = true;

    if (!instanceManager || !particlePool || !templateLibrary || !uiManager) {
        if (instanceManager) instanceManager->Clear();
        if (particlePool) particlePool->ClearAll();
        return;
    }
    if (uiManager->GetMode() != EditorMode::Template) return;

    FireworkTemplate* t = templateLibrary->GetActive();
    templatePreviewBakedRotation = t ? t->worldRotation : glm::vec3(0.0f);
    if (!t) {
        instanceManager->Clear();
        particlePool->ClearAll();
        return;
    }

    instanceManager->Clear();
    particlePool->ClearAll();

    // Estimate a visually meaningful peak: after emission, around mid-life.
    const float emission = std::max(0.0f, t->branchTemplate.emissionDuration);
    const float life = std::max(0.0f, t->branchTemplate.lifetime);
    float peakOffset = emission + 0.5f * life;

    if (peakOffset < 0.1f) peakOffset = 0.1f;
    if (peakOffset > 4.0f) peakOffset = 4.0f;

    const float startTime = nowSeconds - peakOffset;
    auto* inst = new FireworkInstance(t, glm::vec3(0.0f, 0.0f, 0.0f), startTime);
    instanceManager->AddInstance(inst);

    // Simulate forward to "nowSeconds" with a bounded step count.
    const float step = 1.0f / 30.0f;
    const int maxSteps = 120;
    int steps = static_cast<int>(std::ceil(peakOffset / step));
    if (steps > maxSteps) steps = maxSteps;

    float tcur = startTime;
    const float target = nowSeconds;
    for (int s = 0; s < steps; ++s) {
        float dt = step;
        if (tcur + dt > target) dt = std::max(0.0f, target - tcur);
        instanceManager->Update(tcur + dt, dt, *particlePool);
        particlePool->Update(dt);
        tcur += dt;
        if (dt <= 0.0f) break;
    }

    // We don't want to keep an instance around for the idle preview.
    instanceManager->Clear();
}

void Application::Shutdown()
{
    delete scenePlacementController;
    scenePlacementController = nullptr;

    delete templateRotationController;
    templateRotationController = nullptr;
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