#pragma once

#include "src/ui/panels/IPanel.h"

class FireworkTemplate;

namespace ui::panels {

// Placeholder for a future physics parameters panel.
class PhysicsEditorPanel final : public IPanel {
public:
    explicit PhysicsEditorPanel(FireworkTemplate* tmpl) : tmpl(tmpl) {}
    void SetTemplate(FireworkTemplate* t) { tmpl = t; }
    void Render() override;

private:
    FireworkTemplate* tmpl = nullptr;
};

} // namespace ui::panels
