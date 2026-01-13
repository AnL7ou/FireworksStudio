#pragma once

#include <cstddef>

class Scene;
class Timeline;
class TemplateLibrary;

// Interactive timeline widget (ImGui) inspired by NLE editors:
// - draggable playhead
// - zoom & horizontal scroll
// - keyframe-like events that can be dragged
// - right-click to add/remove
class TimelineScrubber {
public:
    TimelineScrubber();

    // Returns true if it changed the timeline time or edited events.
    bool Render(Scene* scene, Timeline* timeline, TemplateLibrary* library, int* selectedEventIndex);

private:
    float pixelsPerSecond;
    float scrollSeconds;
    float rowHeight;
    bool snapEnabled;
    float snapSeconds;
};
