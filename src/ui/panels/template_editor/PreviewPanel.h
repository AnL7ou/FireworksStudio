#pragma once

#include "src/ui/panels/IPanel.h"

namespace ui::panels {

// Placeholder for a future template preview (off-screen simulation, thumbnail, etc.).
// Not currently wired into UIManager.
class PreviewPanel final : public IPanel {
public:
    PreviewPanel() = default;
    void Render() override;
};

} // namespace ui::panels
