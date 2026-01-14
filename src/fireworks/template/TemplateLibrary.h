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

    // Creates a deep copy of an existing template and returns the new template id.
    // This is used by the Scene editor so each placed firework can be edited independently.
    int Clone(int sourceId);

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
    //
    // Templates are editable (including their name). To keep UI elements (scene list,
    // timeline labels, etc.) in sync, we refresh the cached name list on demand.
    const std::vector<std::string>& GetNames() const;

    // Convenience: fetch name for a given template id (or nullptr if not found).
    const char* GetName(int id) const;

private:
    int NextId();

    // Keeps `names` consistent with the current template names.
    void SyncNames() const;

private:
    int nextId;
    int activeId;
    std::vector<int> ids;
    // Cached names for UI (refreshed on demand because templates are editable).
    mutable std::vector<std::string> names;
    std::vector<std::unique_ptr<FireworkTemplate>> templates;
};
