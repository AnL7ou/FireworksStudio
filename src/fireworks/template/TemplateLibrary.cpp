#include "TemplateLibrary.h"

TemplateLibrary::TemplateLibrary()
    : nextId(1)
    , activeId(-1)
{
}

int TemplateLibrary::NextId()
{
    return nextId++;
}

int TemplateLibrary::Add(std::unique_ptr<FireworkTemplate> t)
{
    if (!t) {
        return -1;
    }
    int id = NextId();
    ids.push_back(id);
    names.push_back(t->name);
    templates.push_back(std::move(t));
    if (activeId < 0) {
        activeId = id;
    }
    return id;
}

int TemplateLibrary::Clone(int sourceId)
{
    const FireworkTemplate* src = Get(sourceId);
    if (!src) return -1;

    auto copy = std::make_unique<FireworkTemplate>(*src);

    // Make the copy name explicit and (mostly) unique for UI.
    // We keep it simple and append the new numeric id after Add().
    std::string baseName = copy->name;
    int newId = Add(std::move(copy));
    if (newId >= 0) {
        FireworkTemplate* t = Get(newId);
        if (t) {
            t->name = baseName + " (" + std::to_string(newId) + ")";
        }
    }
    return newId;
}

void TemplateLibrary::SyncNames() const
{
    // Keep a stable vector instance for UI code that holds references.
    if (names.size() != templates.size()) {
        names.resize(templates.size());
    }
    for (size_t i = 0; i < templates.size(); ++i) {
        if (templates[i]) {
            names[i] = templates[i]->name;
        } else {
            names[i].clear();
        }
    }
}

const std::vector<std::string>& TemplateLibrary::GetNames() const
{
    SyncNames();
    return names;
}

const FireworkTemplate* TemplateLibrary::Get(int id) const
{
    for (size_t i = 0; i < ids.size(); ++i) {
        if (ids[i] == id) {
            return templates[i].get();
        }
    }
    return nullptr;
}

FireworkTemplate* TemplateLibrary::Get(int id)
{
    return const_cast<FireworkTemplate*>(static_cast<const TemplateLibrary*>(this)->Get(id));
}

void TemplateLibrary::SetActiveId(int id)
{
    if (Get(id)) {
        activeId = id;
    }
}

FireworkTemplate* TemplateLibrary::GetActive()
{
    return Get(activeId);
}

const FireworkTemplate* TemplateLibrary::GetActive() const
{
    return Get(activeId);
}

void TemplateLibrary::SeedPresets()
{
    Add(std::make_unique<FireworkTemplate>(FireworkTemplate::Chrysanthemum()));
    Add(std::make_unique<FireworkTemplate>(FireworkTemplate::Palm()));
    Add(std::make_unique<FireworkTemplate>(FireworkTemplate::Willow()));
    Add(std::make_unique<FireworkTemplate>(FireworkTemplate::Ring()));
    Add(std::make_unique<FireworkTemplate>(FireworkTemplate::Sphere()));
}

const char* TemplateLibrary::GetName(int id) const
{
    for (size_t i = 0; i < ids.size(); ++i) {
        if (ids[i] == id) {
            // Return the live template name so timeline labels update immediately.
            return templates[i] ? templates[i]->name.c_str() : nullptr;
        }
    }
    return nullptr;
}
