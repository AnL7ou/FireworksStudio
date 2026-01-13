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
