#include "Scene.h"

Scene::Scene(std::string n)
    : name(std::move(n))
    , durationSeconds(10.0f)
{
}

void Scene::SetDuration(float seconds)
{
    durationSeconds = (seconds < 0.0f) ? 0.0f : seconds;
}

size_t Scene::AddEvent(const FireworkEvent& e)
{
    events.push_back(e);
    SortByTime();
    return events.size() - 1;
}

void Scene::RemoveEvent(size_t index)
{
    if (index >= events.size()) {
        return;
    }
    events.erase(events.begin() + static_cast<long>(index));
}

void Scene::SortByTime()
{
    std::stable_sort(events.begin(), events.end(), [](const FireworkEvent& a, const FireworkEvent& b) {
        return a.triggerTime < b.triggerTime;
    });
}
