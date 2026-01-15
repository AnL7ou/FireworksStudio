#include "FileDialogModal.h"

#include <algorithm>
#include <cstdio>
#include <filesystem>

#include <imgui.h>

namespace ui::widgets {

FileDialogModal::FileDialogModal() = default;

void FileDialogModal::Open(Kind kind,
                           const char* title,
                           const char* defaultPath,
                           const char* wantedExt,
                           OnAcceptFn onAccept)
{
    namespace fs = std::filesystem;

    m_kind = kind;
    m_title = title ? title : "File";
    m_wantedExt = wantedExt ? wantedExt : "";
    m_onAccept = std::move(onAccept);

    fs::path cwd = fs::current_path();
    fs::path filename;

    if (defaultPath && defaultPath[0] != '\0') {
        fs::path p(defaultPath);
        if (p.has_parent_path()) {
            cwd = p.parent_path();
            filename = p.filename();
        } else {
            filename = p.filename();
        }
    }

    m_cwd = cwd.lexically_normal().string();
    m_filename = filename.string();

    std::snprintf(m_cwdBuffer, sizeof(m_cwdBuffer), "%s", m_cwd.c_str());
    std::snprintf(m_filenameBuffer, sizeof(m_filenameBuffer), "%s", m_filename.c_str());

    m_selectedIndex = -1;
    m_needsRescan = true;
    m_open = true;

    ImGui::OpenPopup("File Dialog");
}

void FileDialogModal::RescanEntries()
{
    namespace fs = std::filesystem;
    m_entries.clear();

    fs::path cwd;
    try {
        cwd = m_cwdBuffer[0] ? fs::path(m_cwdBuffer) : fs::current_path();
    } catch (...) {
        cwd = fs::current_path();
    }

    std::error_code ec;
    if (!fs::exists(cwd, ec) || !fs::is_directory(cwd, ec)) {
        cwd = fs::current_path();
    }

    // Normalize and persist back
    m_cwd = cwd.lexically_normal().string();
    std::snprintf(m_cwdBuffer, sizeof(m_cwdBuffer), "%s", m_cwd.c_str());

    for (auto it = fs::directory_iterator(cwd, ec);
         !ec && it != fs::directory_iterator();
         it.increment(ec))
    {
        const fs::directory_entry& de = *it;
        Entry e;
        e.name = de.path().filename().string();
        e.isDirectory = de.is_directory(ec);
        m_entries.push_back(std::move(e));
    }

    std::sort(m_entries.begin(), m_entries.end(),
              [](const Entry& a, const Entry& b) {
                  if (a.isDirectory != b.isDirectory) return a.isDirectory > b.isDirectory; // dirs first
                  return a.name < b.name;
              });
}

void FileDialogModal::Render()
{
    if (!m_open) return;

    if (m_needsRescan) {
        RescanEntries();
        m_needsRescan = false;
    }

    bool open = true;
    if (ImGui::BeginPopupModal("File Dialog", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
        namespace fs = std::filesystem;

        const bool isSave = (m_kind == Kind::Save);
        const char* wantedExt = m_wantedExt.c_str();

        ImGui::TextUnformatted(m_title.c_str());
        ImGui::Separator();

        // Directory controls
        ImGui::TextUnformatted("Folder:");
        ImGui::SameLine();
        if (ImGui::Button("Up")) {
            fs::path p(m_cwdBuffer);
            if (p.has_parent_path()) {
                std::snprintf(m_cwdBuffer, sizeof(m_cwdBuffer), "%s", p.parent_path().string().c_str());
                m_needsRescan = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh")) {
            m_needsRescan = true;
        }

        if (ImGui::InputText("##cwd", m_cwdBuffer, sizeof(m_cwdBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
            m_needsRescan = true;
        }

        ImGui::Separator();

        // Listing
        ImGui::BeginChild("##filelist", ImVec2(700, 320), true);
        for (int i = 0; i < (int)m_entries.size(); ++i) {
            const auto& e = m_entries[i];

            // Filter files by extension (directories always visible)
            if (!e.isDirectory && wantedExt && wantedExt[0]) {
                fs::path p(e.name);
                if (p.extension() != wantedExt) continue;
            }

            std::string label = e.isDirectory ? ("[D] " + e.name) : e.name;
            bool selected = (m_selectedIndex == i);
            if (ImGui::Selectable(label.c_str(), selected)) {
                m_selectedIndex = i;
                if (!e.isDirectory) {
                    std::snprintf(m_filenameBuffer, sizeof(m_filenameBuffer), "%s", e.name.c_str());
                }
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                if (e.isDirectory) {
                    fs::path next = fs::path(m_cwdBuffer) / e.name;
                    std::snprintf(m_cwdBuffer, sizeof(m_cwdBuffer), "%s", next.string().c_str());
                    m_selectedIndex = -1;
                    m_filenameBuffer[0] = '\0';
                    m_needsRescan = true;
                }
            }
        }
        ImGui::EndChild();

        ImGui::Separator();
        if (wantedExt && wantedExt[0]) {
            ImGui::Text("File (%s):", wantedExt);
        } else {
            ImGui::TextUnformatted("File:");
        }
        ImGui::InputText("##filename", m_filenameBuffer, sizeof(m_filenameBuffer));

        ImGui::Separator();

        fs::path fullPath = fs::path(m_cwdBuffer) / fs::path(m_filenameBuffer);

        bool canOk = (m_filenameBuffer[0] != '\0');
        if (!isSave) {
            std::error_code ec;
            canOk = canOk && fs::exists(fullPath, ec) && fs::is_regular_file(fullPath, ec);
        }

        if (!canOk) ImGui::BeginDisabled();
        if (ImGui::Button("OK")) {
            if (isSave && wantedExt && wantedExt[0]) {
                if (fullPath.extension() != wantedExt) {
                    fullPath.replace_extension(wantedExt);
                }
            }
            if (m_onAccept) m_onAccept(fullPath.string());

            m_open = false;
            ImGui::CloseCurrentPopup();
        }
        if (!canOk) ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            m_open = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    } else {
        // Closed via ESC / click outside
        m_open = false;
    }
}

} // namespace ui::widgets
