#pragma once

class Scene;
class TemplateLibrary;

namespace ui {
namespace panels {

class SceneViewPanel {
public:
    SceneViewPanel(Scene* scene, TemplateLibrary* library);
    ~SceneViewPanel() = default;

    void SetScene(Scene* s) { scene = s; }
    void SetLibrary(TemplateLibrary* l) { library = l; }

    void SetSelectedEvent(int idx) { selectedEvent = idx; }

    void Render();

private:
    Scene* scene;
    TemplateLibrary* library;
    int selectedEvent;
};

} // namespace panels
} // namespace ui
