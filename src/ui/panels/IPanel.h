#pragma once

// Lightweight common interface for panels.
// Existing panels do not need to inherit from it, but it provides a consistent
// abstraction for future refactors.
namespace ui::panels {

class IPanel {
public:
    virtual ~IPanel() = default;
    virtual void Render() = 0;
};

} // namespace ui::panels
