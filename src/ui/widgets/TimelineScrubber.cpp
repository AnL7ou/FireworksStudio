#include "TimelineScrubber.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

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

} // namespace

TimelineScrubber::TimelineScrubber()
    : pixelsPerSecond(90.0f)
    , scrollSeconds(0.0f)
    , rowHeight(31.5f)
    , snapEnabled(true)
    , snapSeconds(0.1f)
    , pendingFocusTimeSeconds(-1.0f)
{
}

static float EstimateTemplateDurationSeconds(const FireworkTemplate* t)
{
    if (!t) return 0.05f;

    float d = 0.0f;
    d += std::max(0.0f, t->branchTemplate.emissionDuration);
    d += std::max(0.0f, t->branchTemplate.lifetime);

    if (t->branchTemplate.trailEnabled) {
        d += std::max(0.0f, t->branchTemplate.trailDuration);
    }

    return std::max(d, 0.05f);
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

    ImGui::PushItemWidth(160.0f);
    ImGui::DragFloat("Zoom (px/s)", &pixelsPerSecond, 1.0f, 20.0f, 400.0f, "%.0f");
    ImGui::SameLine();
    ImGui::Checkbox("Snap", &snapEnabled);
    ImGui::SameLine();
    ImGui::DragFloat("Step (s)", &snapSeconds, 0.01f, 0.01f, 2.0f, "%.2f");
    ImGui::PopItemWidth();

    float duration = scene->GetDuration();
    if (duration <= 0.0f) duration = 10.0f;

    const auto& events = scene->GetEvents();

    const float headerHeight = 24.0f;
    float canvasHeight = headerHeight + events.size() * rowHeight + 6.0f;

    const ImVec2 canvasSize(
        ImGui::GetContentRegionAvail().x,
        std::max(180.0f, canvasHeight)
    );

    if (pendingFocusTimeSeconds >= 0.0f) {
        float visibleSeconds = canvasSize.x / pixelsPerSecond;
        scrollSeconds = std::max(0.0f, pendingFocusTimeSeconds - visibleSeconds * 0.25f);
        pendingFocusTimeSeconds = -1.0f;
        changed = true;
    }

    ImGui::BeginChild(
        "##timeline_canvas",
        canvasSize,
        true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    );

    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImVec2 p1 = ImVec2(p0.x + canvasSize.x, p0.y + canvasSize.y);

    dl->AddRectFilled(p0, p1, IM_COL32(20, 22, 28, 255));

    auto TimeToX = [&](float t) {
        return p0.x + (t - scrollSeconds) * pixelsPerSecond;
        };
    auto XToTime = [&](float x) {
        return scrollSeconds + (x - p0.x) / pixelsPerSecond;
        };

    // Grid
    float major = 1.0f;
    if (pixelsPerSecond > 220.0f) major = 0.25f;
    else if (pixelsPerSecond > 140.0f) major = 0.5f;
    else if (pixelsPerSecond < 60.0f) major = 2.0f;

    float tStart = std::max(0.0f, scrollSeconds);
    float tEnd = scrollSeconds + canvasSize.x / pixelsPerSecond;

    for (float t = std::floor(tStart / major) * major; t <= tEnd; t += major) {
        float x = TimeToX(t);
        dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), IM_COL32(40, 44, 55, 255));
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%.1fs", t);
        dl->AddText(ImVec2(x + 4, p0.y + 2), IM_COL32(150, 160, 180, 255), buf);
    }

    const ImVec2 mouse = ImGui::GetMousePos();
    const bool hovered = ImGui::IsWindowHovered();

    // --- Playhead dragging (keeps previous behavior) ---
    static bool playheadDragging = false;
    static float playheadDragOffsetPx = 0.0f;
    const float playGrabRadius = 6.0f;

    float playX = TimeToX(timeline->GetTime());

    // Start playhead drag if clicking near it AND not already dragging an event
    if (!playheadDragging && hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (std::abs(mouse.x - playX) <= playGrabRadius) {
            playheadDragging = true;
            playheadDragOffsetPx = mouse.x - playX;
        }
    }

    // Update playhead drag
    if (playheadDragging) {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            playheadDragging = false;
        }
        else if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            float x = mouse.x - playheadDragOffsetPx;
            float t = XToTime(x);
            t = std::clamp(t, 0.0f, duration);
            if (snapEnabled) t = SnapTo(t, snapSeconds);
            timeline->SetTime(t);
            timeline->SetLastDispatchedTime(t);
            changed = true;
        }
    }

    static int draggingEvent = -1;
    int hoveredEvent = -1;

    // Rows + events
    for (int i = 0; i < (int)events.size(); ++i) {
        const FireworkEvent& e = events[i];

        float rowY0 = p0.y + headerHeight + i * rowHeight;
        float rowY1 = rowY0 + rowHeight;
        float y = (rowY0 + rowY1) * 0.5f;

        ImU32 rowCol = (i % 2 == 0)
            ? IM_COL32(26, 28, 36, 255)
            : IM_COL32(30, 32, 40, 255);

        // Draw row background first
        dl->AddRectFilled(ImVec2(p0.x, rowY0), ImVec2(p1.x, rowY1), rowCol);

        // Segment duration
        float dur = 0.2f;
        if (library) {
            dur = EstimateTemplateDurationSeconds(library->Get(e.templateId));
        }

        float x0 = TimeToX(e.triggerTime);
        float x1 = TimeToX(e.triggerTime + dur);
        if (x1 < x0 + 10.0f) x1 = x0 + 10.0f;

        ImVec2 a(x0, y - 10.0f);
        ImVec2 b(x1, y + 10.0f);

        // Capture input on the row (so window doesn't drag).
        // Important: the InvisibleButton must NOT "cover" the whole child in a way that hides the playhead.
        // We do it per-row, after we computed rowY0/rowY1.
        ImGui::SetCursorScreenPos(ImVec2(p0.x, rowY0));
        ImGui::InvisibleButton(
            ("##timeline_row_" + std::to_string(i)).c_str(),
            ImVec2(canvasSize.x, rowHeight)
        );

        bool rowHovered = ImGui::IsItemHovered();
        bool rowActive = ImGui::IsItemActive();

        // Hover detection for event rect (not whole row)
        if (rowHovered &&
            mouse.x >= a.x && mouse.x <= b.x &&
            mouse.y >= a.y && mouse.y <= b.y) {
            hoveredEvent = i;
        }

        bool selected = (selectedEventIndex && *selectedEventIndex == i);
        ImU32 col = selected
            ? IM_COL32(110, 190, 255, 255)
            : IM_COL32(200, 220, 255, 220);

        if (!e.enabled)
            col = IM_COL32(130, 130, 130, 180);

        dl->AddRectFilled(a, b, col, 3.0f);
        dl->AddRect(a, b, IM_COL32(0, 0, 0, 180), 3.0f);

        const char* name = library ? library->GetName(e.templateId) : nullptr;
        if (!name || !name[0]) name = e.label.c_str();

        dl->PushClipRect(a, b, true);
        dl->AddText(ImVec2(a.x + 6, a.y + 2), IM_COL32(10, 12, 16, 220), name);
        dl->PopClipRect();

        // Start handle
        dl->AddRectFilled(
            ImVec2(x0 - 3, y - 12),
            ImVec2(x0 + 3, y + 12),
            IM_COL32(0, 0, 0, 120),
            2.0f
        );

        // Start event drag only if we are over the event rect, and not dragging playhead
        if (!playheadDragging && rowActive && hoveredEvent == i && draggingEvent < 0) {
            draggingEvent = i;
            if (selectedEventIndex) *selectedEventIndex = i;
            changed = true;
        }
    }

    // Click on empty space: move playhead (only if not clicking an event and not dragging playhead)
    if (hovered &&
        hoveredEvent < 0 &&
        !playheadDragging &&
        draggingEvent < 0 &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        float t = XToTime(mouse.x);
        t = std::clamp(t, 0.0f, duration);
        if (snapEnabled) t = SnapTo(t, snapSeconds);

        timeline->SetTime(t);
        timeline->SetLastDispatchedTime(t);
        changed = true;
    }

    // Drag update
    if (draggingEvent >= 0 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        float t = XToTime(mouse.x);
        t = std::clamp(t, 0.0f, duration);
        if (snapEnabled) t = SnapTo(t, snapSeconds);
        scene->GetEvents()[draggingEvent].triggerTime = t;
        changed = true;
    }

    // Drag end
    if (draggingEvent >= 0 && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        draggingEvent = -1;
        changed = true;
    }

    // Draw playhead LAST so it stays on top of row fills (fixes "under black stripe")
    playX = TimeToX(timeline->GetTime());
    dl->AddLine(ImVec2(playX, p0.y), ImVec2(playX, p1.y),
        IM_COL32(255, 200, 80, 255), 2.0f);

    ImGui::EndChild();
    return changed;
}
