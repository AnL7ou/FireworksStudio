#include "TimelinePanel.h"

#include <imgui.h>

#include "src/scene/Scene.h"
#include "src/scene/Timeline.h"

namespace ui {
namespace panels {

TimelinePanel::TimelinePanel(Scene* s, Timeline* t, TemplateLibrary* l)
    : scene(s)
    , timeline(t)
    , library(l)
    , selectedEventIndex(nullptr)
{
}

void TimelinePanel::Render()
{
    if (!scene || !timeline) {
        ImGui::TextDisabled("Timeline indisponible");
        return;
    }

    float duration = scene->GetDuration();
    ImGui::Text("Durée scène: %.2fs", duration);
    ImGui::SliderFloat("##time", &duration, 1.0f, 120.0f, "%.1f s");
    scene->SetDuration(duration);

    ImGui::Separator();

    bool playing = timeline->IsPlaying();
    if (ImGui::Button(playing ? "Pause" : "Play")) {
        timeline->SetPlaying(!playing);
        if (!playing) {
            // Start playback from current time. We avoid retro-triggering by aligning dispatch cursor.
            timeline->SetLastDispatchedTime(timeline->GetTime());
        }
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        timeline->Reset();
    }
    ImGui::SameLine();
    if (ImGui::Button("Step +1/60")) {
        float t = timeline->GetTime() + (1.0f / 60.0f);
        if (t > duration) t = duration;
        timeline->SetTime(t);
        timeline->SetLastDispatchedTime(t);
    }

    ImGui::Separator();

    // Interactive editor (video-editor-like) for playhead and events.
    if (scrubber.Render(scene, timeline, library, selectedEventIndex)) {
        // Keep UI slider in sync + prevent accidental retrigger on changes.
        timeline->SetLastDispatchedTime(timeline->GetTime());
    }
}

} // namespace panels
} // namespace ui
