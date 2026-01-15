#pragma once

#include "src/ui/panels/IPanel.h"

class FireworkTemplate;

namespace ui::panels {

// Placeholder for a future per-branch editor (fine-grained branch parameters).
class BranchEditorPanel final : public IPanel {
public:
    explicit BranchEditorPanel(FireworkTemplate* tmpl) : tmpl(tmpl) {}
    void SetTemplate(FireworkTemplate* t) { tmpl = t; }
    void Render() override;

private:
    FireworkTemplate* tmpl = nullptr;
};

} // namespace ui::panels
