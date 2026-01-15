#pragma once

#include <functional>

class TemplateLibrary;

namespace ui {
namespace panels {

class TemplateLibraryPanel {
public:
    using SelectionChangedCallback = std::function<void(int activeTemplateId)>;

    explicit TemplateLibraryPanel(TemplateLibrary* library);
    ~TemplateLibraryPanel() = default;

    void SetLibrary(TemplateLibrary* library);
    void SetOnSelectionChanged(SelectionChangedCallback cb);

    void Render();

private:
    TemplateLibrary* library;
    SelectionChangedCallback onSelectionChanged;
    int uiSelectedIndex;
};

} // namespace panels
} // namespace ui
