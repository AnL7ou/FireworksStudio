#include "TimelineScrubber.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include <imgui.h>

#include "src/scene/Scene.h"
#include "src/scene/Timeline.h"
#include "src/fireworks/template/TemplateLibrary.h"

namespace {

float SnapTo(float t, float step)
{
    if (step <= 0.0f) return t;
    return std::round(t / step) * step;
}

float Clamp01(float x)
{
    if (x < 0.0f) return 0.0f;
    if (x > 1.0f) return 1.0f;
    return x;
}

} // namespace

TimelineScrubber::TimelineScrubber()
    : pixelsPerSecond(90.0f)
    , scrollSeconds(0.0f)
    , rowHeight(42.0f)
    , snapEnabled(true)
    , snapSeconds(0.1f)
    , pendingFocusTimeSeconds(-1.0f)
{
}

static float EstimateTemplateDurationSeconds(const FireworkTemplate* t)
{
    if (!t) return 0.0f;
    float d = 0.0f;

    // Minimal, pragmatic estimate: emission window + particle lifetime.
    d += std::max(0.0f, t->branchTemplate.emissionDuration);
    d += std::max(0.0f, t->branchTemplate.lifetime);

    // Trails extend visibility slightly.
    if (t->branchTemplate.trailEnabled) {
        d += std::max(0.0f, t->branchTemplate.trailDuration);
    }

    // Keep a visible minimum.
    if (d < 0.05f) d = 0.05f;
    return d;
}

void TimelineScrubber::RequestFocusTime(float tSeconds)
{
    pendingFocusTimeSeconds = tSeconds;
}

bool TimelineScrubber::Render(Scene* scene, Timeline* timeline, TemplateLibrary* library, int* selectedEventIndex)
{
    if (!scene || !timeline) {
        ImGui::TextDisabled("Timeline indisponible");
        return false;
    }

    bool changed = false;

    // Controls
    ImGui::PushItemWidth(180.0f);
    ImGui::DragFloat("Zoom (px/s)", &pixelsPerSecond, 1.0f, 20.0f, 400.0f, "%.0f");
    ImGui::SameLine();
    ImGui::DragFloat("Scroll (s)", &scrollSeconds, 0.1f, 0.0f, 99999.0f, "%.1f");
    ImGui::SameLine();
    ImGui::Checkbox("Snap", &snapEnabled);
    ImGui::SameLine();
    ImGui::DragFloat("Step (s)", &snapSeconds, 0.01f, 0.01f, 2.0f, "%.2f");
    ImGui::PopItemWidth();

    float duration = scene->GetDuration();
    if (duration <= 0.0f) duration = 10.0f;

    // Timeline canvas
    const ImVec2 canvasSize = ImVec2(ImGui::GetContentRegionAvail().x, 180.0f);

    // Consume pending focus request once we know the viewport size.
    if (pendingFocusTimeSeconds >= 0.0f) {
        const float visibleSeconds = (pixelsPerSecond > 0.0f) ? (canvasSize.x / pixelsPerSecond) : 0.0f;
        const float maxScroll = std::max(0.0f, duration - visibleSeconds);
        // Put the focused time a bit after the left edge (like most NLEs).
        float targetScroll = pendingFocusTimeSeconds - (visibleSeconds * 0.25f);
        if (targetScroll < 0.0f) targetScroll = 0.0f;
        if (targetScroll > maxScroll) targetScroll = maxScroll;
        scrollSeconds = targetScroll;
        pendingFocusTimeSeconds = -1.0f;
        changed = true;
    }

    ImGui::BeginChild("##timeline_canvas", canvasSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const ImVec2 p0 = ImGui::GetCursorScreenPos();
    const ImVec2 p1 = ImVec2(p0.x + canvasSize.x, p0.y + canvasSize.y);

    // Background
    dl->AddRectFilled(p0, p1, IM_COL32(20, 22, 28, 255));

    auto TimeToX = [&](float t) {
        return p0.x + (t - scrollSeconds) * pixelsPerSecond;
    };
    auto XToTime = [&](float x) {
        return scrollSeconds + (x - p0.x) / pixelsPerSecond;
    };

    // Consume external focus request (e.g., from inspector selection).
    if (pendingFocusTimeSeconds >= 0.0f) {
        const float visibleSpan = (pixelsPerSecond > 0.0f) ? (canvasSize.x / pixelsPerSecond) : 1.0f;
        float target = pendingFocusTimeSeconds;
        // Put the target time near the first quarter of the viewport (gives room to the right).
        float newScroll = target - visibleSpan * 0.25f;
        float maxScroll = std::max(0.0f, duration - visibleSpan);
        if (newScroll < 0.0f) newScroll = 0.0f;
        if (newScroll > maxScroll) newScroll = maxScroll;
        scrollSeconds = newScroll;
        pendingFocusTimeSeconds = -1.0f;
        changed = true;
    }

    // Grid
    float major = 1.0f;
    if (pixelsPerSecond > 220.0f) major = 0.25f;
    else if (pixelsPerSecond > 140.0f) major = 0.5f;
    else if (pixelsPerSecond < 60.0f) major = 2.0f;

    float tStart = std::max(0.0f, scrollSeconds);
    float tEnd = std::min(duration, scrollSeconds + canvasSize.x / pixelsPerSecond);
    float t0 = std::floor(tStart / major) * major;
    for (float t = t0; t <= tEnd + major; t += major) {
        float x = TimeToX(t);
        if (x < p0.x || x > p1.x) continue;
        dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), IM_COL32(40, 44, 55, 255));
        char label[32];
        std::snprintf(label, sizeof(label), "%.1fs", t);
        dl->AddText(ImVec2(x + 4.0f, p0.y + 2.0f), IM_COL32(160, 170, 190, 255), label);
    }

    // Single track row
    float rowY0 = p0.y + 24.0f;
    float rowY1 = rowY0 + rowHeight;
    dl->AddRectFilled(ImVec2(p0.x, rowY0), ImVec2(p1.x, rowY1), IM_COL32(26, 28, 36, 255));
    dl->AddText(ImVec2(p0.x + 6.0f, rowY0 + 6.0f), IM_COL32(200, 205, 215, 255), "Fireworks");

    // Playhead
    float playX = TimeToX(timeline->GetTime());
    dl->AddLine(ImVec2(playX, p0.y), ImVec2(playX, p1.y), IM_COL32(255, 200, 80, 255), 2.0f);

    // Interaction helpers
    const bool hovered = ImGui::IsWindowHovered();
    const ImVec2 mouse = ImGui::GetMousePos();

    auto& events = scene->GetEvents();

    // Drag & drop target: allow dropping an event from the scene list to move it in time.
    // We target only the track row, since dropping on the ruler can be ambiguous.
    ImGui::SetCursorScreenPos(ImVec2(p0.x, rowY0));
    ImGui::InvisibleButton("##timeline_drop_row", ImVec2(canvasSize.x, rowHeight));
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_EVENT_INDEX")) {
            if (payload->Data && payload->DataSize == sizeof(int)) {
                int idx = *static_cast<const int*>(payload->Data);
                if (idx >= 0 && idx < static_cast<int>(events.size())) {
                    float t = XToTime(mouse.x);
                    t = std::max(0.0f, std::min(duration, t));
                    if (snapEnabled) t = SnapTo(t, snapSeconds);
                    events[idx].triggerTime = t;
                    scene->SortByTime();
                    // Sorting changes indices: re-find by pointer-stable key (best effort).
                    // Here we keep selection on the closest event in time.
                    if (selectedEventIndex) {
                        // Find event with smallest time delta.
                        int best = 0;
                        float bestDt = std::abs(scene->GetEvents()[0].triggerTime - t);
                        for (int i = 1; i < static_cast<int>(scene->GetEvents().size()); ++i) {
                            float dt = std::abs(scene->GetEvents()[i].triggerTime - t);
                            if (dt < bestDt) { best = i; bestDt = dt; }
                        }
                        *selectedEventIndex = best;
                    }
                    changed = true;
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    // --- Playhead dragging (public ImGui API only) ---
    static bool playheadDragging = false;     // if you have multiple timelines, move this into your timeline UI state
    static float dragOffsetPx = 0.0f;         // optional: keeps the grab stable

    const float playGrabRadius = 6.0f;

    // Start drag when clicking near playhead
    if (!playheadDragging && hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        if (std::abs(mouse.x - playX) <= playGrabRadius)
        {
            playheadDragging = true;
            dragOffsetPx = mouse.x - playX;
        }
    }

    // Update drag
    if (playheadDragging)
    {
        // Stop drag on release (even if mouse left the widget)
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            playheadDragging = false;
        }
        else if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
        {
            float x = mouse.x - dragOffsetPx;
            float t = XToTime(x);
            t = std::max(0.0f, std::min(duration, t));
            if (snapEnabled) t = SnapTo(t, snapSeconds);

            timeline->SetTime(t);
            // Avoid accidental re-triggering after scrub.
            timeline->SetLastDispatchedTime(t);

            changed = true;
        }
    }

    // Events
    int hoveredEvent = -1;
    for (int i = 0; i < static_cast<int>(events.size()); ++i) {
        const FireworkEvent& e = events[i];
        float x0 = TimeToX(e.triggerTime);
        float y = (rowY0 + rowY1) * 0.5f;

        // Show event as a segment whose length matches the template duration.
        float dur = 0.2f;
        if (library) {
            const FireworkTemplate* t = library->Get(e.templateId);
            dur = EstimateTemplateDurationSeconds(t);
        }
        float x1 = TimeToX(e.triggerTime + dur);
        if (x1 < x0 + 10.0f) x1 = x0 + 10.0f; // always draggable/visible

        ImVec2 a(x0, y - 10.0f);
        ImVec2 b(x1, y + 10.0f);

        if (mouse.x >= a.x && mouse.x <= b.x && mouse.y >= a.y && mouse.y <= b.y && hovered) {
            hoveredEvent = i;
        }

        bool selected = (selectedEventIndex && *selectedEventIndex == i);
        ImU32 col = selected ? IM_COL32(110, 190, 255, 255) : IM_COL32(200, 220, 255, 220);
        if (!e.enabled) col = IM_COL32(130, 130, 130, 180);

        dl->AddRectFilled(a, b, col, 3.0f);
        dl->AddRect(a, b, IM_COL32(0, 0, 0, 180), 3.0f);

        // Label inside the segment: prefer template name, fallback to event label.
        const char* name = nullptr;
        if (library) {
            name = library->GetName(e.templateId);
        }
        if (!name || name[0] == '\0') {
            name = e.label.c_str();
        }
        if (name && name[0] != '\0') {
            // Clip to the segment bounds so long names don't bleed out.
            dl->PushClipRect(a, b, true);
            const float padX = 6.0f;
            const float padY = 2.0f;
            ImVec2 tp(a.x + padX, a.y + padY);
            dl->AddText(tp, IM_COL32(10, 12, 16, 220), name);
            dl->PopClipRect();
        }

        // Small "start handle" to keep the "point" semantics visible.
        dl->AddRectFilled(ImVec2(x0 - 3.0f, y - 12.0f), ImVec2(x0 + 3.0f, y + 12.0f), IM_COL32(0, 0, 0, 120), 2.0f);
    }

    // Click on empty space: move playhead (and therefore resume playback from there).
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hoveredEvent < 0 && !playheadDragging) {
        if (std::abs(mouse.x - playX) > playGrabRadius) {
            float t = XToTime(mouse.x);
            t = std::max(0.0f, std::min(duration, t));
            if (snapEnabled) t = SnapTo(t, snapSeconds);
            timeline->SetTime(t);
            timeline->SetLastDispatchedTime(t);
            changed = true;
        }
    }

    // Select / drag event
    static int draggingEvent = -1;
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && hoveredEvent >= 0) {
        if (selectedEventIndex) *selectedEventIndex = hoveredEvent;
        draggingEvent = hoveredEvent;
        changed = true;
    }
    if (draggingEvent >= 0 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        float t = XToTime(mouse.x);
        t = std::max(0.0f, std::min(duration, t));
        if (snapEnabled) t = SnapTo(t, snapSeconds);
        auto& e = events[draggingEvent];
        e.triggerTime = t;
        changed = true;
    }
    if (draggingEvent >= 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        draggingEvent = -1;
        scene->SortByTime();
        changed = true;
    }

    // Context menu: add/remove
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup("##timeline_ctx");
    }
    if (ImGui::BeginPopup("##timeline_ctx")) {
        float t = XToTime(mouse.x);
        t = std::max(0.0f, std::min(duration, t));
        if (snapEnabled) t = SnapTo(t, snapSeconds);

        if (ImGui::MenuItem("Add event here")) {
            FireworkEvent e;
            e.triggerTime = t;
            if (library) {
                int active = library->GetActiveId();
                e.templateId = (active >= 0) ? library->Clone(active) : -1;
            } else {
                e.templateId = -1;
            }
            e.position = {0.0f, 0.0f, 0.0f};
            e.label = "Firework";
            scene->AddEvent(e);
            if (selectedEventIndex) *selectedEventIndex = static_cast<int>(scene->GetEvents().size()) - 1;
            changed = true;
        }

        if (selectedEventIndex && *selectedEventIndex >= 0 && *selectedEventIndex < static_cast<int>(events.size())) {
            if (ImGui::MenuItem("Delete selected")) {
                scene->RemoveEvent(static_cast<size_t>(*selectedEventIndex));
                if (*selectedEventIndex >= static_cast<int>(scene->GetEvents().size())) {
                    *selectedEventIndex = static_cast<int>(scene->GetEvents().size()) - 1;
                }
                changed = true;
            }
            if (ImGui::MenuItem(events[*selectedEventIndex].enabled ? "Disable selected" : "Enable selected")) {
                events[*selectedEventIndex].enabled = !events[*selectedEventIndex].enabled;
                changed = true;
            }
        }

        ImGui::EndPopup();
    }

    // Scroll with mouse wheel when hovering
    if (hovered && ImGui::GetIO().MouseWheel != 0.0f) {
        scrollSeconds -= ImGui::GetIO().MouseWheel * (canvasSize.x / pixelsPerSecond) * 0.08f;
        if (scrollSeconds < 0.0f) scrollSeconds = 0.0f;
        changed = true;
    }

    ImGui::EndChild();
    return changed;
}
