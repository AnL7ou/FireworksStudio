#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <glm/glm.hpp>

// A Scene is an orchestration of firework triggers over time.
// It is intentionally decoupled from template authoring.

struct FireworkEvent {
    int templateId = -1;
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    float triggerTime = 0.0f; // seconds on the timeline
    bool enabled = true;
    std::string label;
};

class Scene {
public:
    explicit Scene(std::string name = "Untitled Scene");

    const std::string& GetName() const { return name; }
    void SetName(const std::string& n) { name = n; }

    // Duration is not strictly enforced; it is primarily UI guidance.
    float GetDuration() const { return durationSeconds; }
    void SetDuration(float seconds);

    const std::vector<FireworkEvent>& GetEvents() const { return events; }
    std::vector<FireworkEvent>& GetEvents() { return events; }

    // Returns index in the internal array.
    size_t AddEvent(const FireworkEvent& e);
    void RemoveEvent(size_t index);
    void SortByTime();

private:
    std::string name;
    float durationSeconds;
    std::vector<FireworkEvent> events;
};
