FireworksStudio/
├── src/
│   ├── main.cpp
│   │
│   ├── core/
│   │   ├── Application.h/cpp          // Point d'entrée
│   │   └── Window.h/cpp               // GLFW wrapper
│   │
│   ├── rendering/
│   │   ├── Shader.h/cpp               // Gestion shaders
│   │   ├── Camera.h/cpp               // Caméra simple
│   │   └── ParticleRenderer.h/cpp     // Rendu particules
│   │
│   ├── fireworks/
│   │   ├── Particle.h                 // Structure particule
│   │   ├── FireworkTemplate.h/cpp     // Modèle de feu
│   │   ├── FireworkInstance.h/cpp     // Instance en scène
│   │   └── ParticleSystem.h/cpp       // Gère toutes les particules
│   │
│   ├── scene/
│   │   ├── Scene.h/cpp                // Scène avec instances
│   │   └── Timeline.h/cpp             // Timeline simple
│   │
│   └── ui/
│       ├── UIManager.h/cpp            // Gestion globale ImGui
│       ├── EditorMode.h               // Enum pour les 2 modes
│       │
│       ├── panels/                    // Nouveau dossier !
│       │   ├── IPanel.h               // Interface commune (optionnel)
│       │   │
│       │   ├── TemplateLibraryPanel.h/cpp
│       │   ├── TemplatePropertiesPanel.h/cpp
│       │   ├── PreviewPanel.h/cpp
│       │   │
│       │   ├── SceneViewPanel.h/cpp
│       │   ├── FireworksListPanel.h/cpp
│       │   └── TimelinePanel.h/cpp
│       │
│       └── widgets/                   // Widgets réutilisables (optionnel)
│           ├── ColorPicker.h/cpp
│           └── TimelineScrubber.h/cpp
│
├── shaders/
│   ├── particle.vert
│   └── particle.frag
│
├── vendor/
│   ├── glad/
│   ├── glfw-3.3.8/
│   ├── glm-1.0.1/
│   ├── imgui-1.91.1/
│   └── stbimage/
│
└── CMakeLists.txt