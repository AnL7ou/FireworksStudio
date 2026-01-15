#pragma once

#include <functional>
#include <string>
#include <vector>

namespace ui::widgets {

// Portable in-app file dialog implemented as an ImGui modal.
// Responsibilities are intentionally narrow:
// - browse folders and pick a file name (with extension filtering)
// - validate selection for load vs save
// - return the final selected path to the caller
class FileDialogModal {
public:
    enum class Kind { Save, Load };
    using OnAcceptFn = std::function<void(const std::string& path)>;

    FileDialogModal();

    // Opens the modal.
    // - kind: Save or Load
    // - title: UI title (shown inside the modal)
    // - defaultPath: optional (cwd + prefilled filename)
    // - wantedExt: extension filter (e.g. ".fwscene")
    // - onAccept: called when user presses OK and selection is valid
    void Open(Kind kind,
              const char* title,
              const char* defaultPath,
              const char* wantedExt,
              OnAcceptFn onAccept);

    // Render the modal if opened.
    void Render();

    bool IsOpen() const { return m_open; }

private:
    struct Entry {
        std::string name;
        bool isDirectory = false;
    };

    void RescanEntries();

    bool m_open = false;
    bool m_needsRescan = false;
    int m_selectedIndex = -1;

    Kind m_kind = Kind::Load;
    std::string m_title;
    std::string m_wantedExt;
    OnAcceptFn m_onAccept;

    std::string m_cwd;
    std::string m_filename;
    char m_cwdBuffer[512] = {0};
    char m_filenameBuffer[256] = {0};

    std::vector<Entry> m_entries;
};

} // namespace ui::widgets
