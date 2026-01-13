#pragma once

#include <memory>
#include <string>
#include <vector>

#include "FireworkTemplate.h"

// Simple in-memory library of templates.
// This is the link between the Template editor and the Scene editor.

class TemplateLibrary {
public:
    TemplateLibrary();

    // Returns an integer ID stable for the current session.
    int Add(std::unique_ptr<FireworkTemplate> t);

    // Utility to seed the library with presets.
    void SeedPresets();

    size_t Count() const { return templates.size(); }

    const FireworkTemplate* Get(int id) const;
    FireworkTemplate* Get(int id);

    // Active template for editing.
    int GetActiveId() const { return activeId; }
    void SetActiveId(int id);

    FireworkTemplate* GetActive();
    const FireworkTemplate* GetActive() const;

    // Names for UI lists.
    const std::vector<std::string>& GetNames() const { return names; }

private:
    int NextId();

private:
    int nextId;
    int activeId;
    std::vector<int> ids;
    std::vector<std::string> names;
    std::vector<std::unique_ptr<FireworkTemplate>> templates;
};
