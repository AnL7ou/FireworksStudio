#pragma once

#include <functional>

class Scene;
class TemplateLibrary;

namespace ui {
namespace panels {

class FireworkListPanel {
public:
    using EventSelectedCallback = std::function<void(int eventIndex)>;

    FireworkListPanel(Scene* scene, TemplateLibrary* library);
    ~FireworkListPanel() = default;

    void SetScene(Scene* s) { scene = s; }
    void SetLibrary(TemplateLibrary* l) { library = l; }
    void SetOnEventSelected(EventSelectedCallback cb) { onSelected = std::move(cb); }
    void SetSelectedEventIndexPtr(int* p) { selectedIndexPtr = p; }

    int GetSelectedEvent() const { return selectedIndex; }

    void Render();

private:
    Scene* scene;
    TemplateLibrary* library;
    EventSelectedCallback onSelected;
    int selectedIndex;
    int* selectedIndexPtr;
};

} // namespace panels
} // namespace ui
