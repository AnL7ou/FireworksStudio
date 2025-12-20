#pragma once

#include <memory>
#include "panels/TemplatePropertyPanel.h" // Ajouté pour inclure la définition complète

struct GLFWwindow;
class FireworkTemplate;

class UIManager {
public:
    UIManager() noexcept;
    ~UIManager() noexcept;

    // Initialise ImGui avec la fenêtre GLFW. Appeler après la création de la fenêtre et l'initialisation de glad.
    bool Initialize(GLFWwindow* window);

    // Commence une frame ImGui (doit être appelé après window.PollEvents())
    void NewFrame() noexcept;

    // Dessine tous les panneaux ImGui et émet le draw call (doit être appelé après le rendu 3D ou juste avant SwapBuffers)
    void Render() noexcept;

    // Ferme/clean ImGui
    void Shutdown() noexcept;

    // Panneau TemplateProperties : création / accès
    void CreateTemplatePanel(FireworkTemplate* fireworkTemplate) noexcept;
    TemplatePropertiesPanel* GetTemplatePanel() noexcept;

private:
    struct Impl;
    Impl* panelImpl;
};