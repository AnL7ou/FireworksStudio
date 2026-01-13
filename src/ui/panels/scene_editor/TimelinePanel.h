#pragma once

class Scene;
class Timeline;
class TemplateLibrary;

#include "src/ui/widgets/TimelineScrubber.h"

namespace ui {
namespace panels {

class TimelinePanel {
public:
    TimelinePanel(Scene* scene, Timeline* timeline, TemplateLibrary* library);
    ~TimelinePanel() = default;

    void SetScene(Scene* s) { scene = s; }
    void SetTimeline(Timeline* t) { timeline = t; }
    void SetTemplateLibrary(TemplateLibrary* l) { library = l; }
    void SetSelectedEventIndexPtr(int* p) { selectedEventIndex = p; }

    void Render();

private:
    Scene* scene;
    Timeline* timeline;
    TemplateLibrary* library;
    int* selectedEventIndex;
    TimelineScrubber scrubber;
};

} // namespace panels
} // namespace ui
