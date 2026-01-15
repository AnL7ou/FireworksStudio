#pragma once

#include <functional>
#include "src/fireworks/template/BranchLayout.h"

namespace ui {
    namespace panels {

        class LayoutEditorPanel {
        public:
            using LayoutChangedCallback = std::function<void(const BranchLayout&)>;

            LayoutEditorPanel(BranchLayout* layout) noexcept;
            ~LayoutEditorPanel() = default;

            void SetLayout(BranchLayout* layout) noexcept;
            void SetOnLayoutChangedCallback(LayoutChangedCallback cb) noexcept;

            void Render() noexcept;

        private:
            void TakeSnapshot() noexcept;
            bool DetectAndNotifyChanges() noexcept;

        private:
            BranchLayout* layout;
            BranchLayout snapshot;
            LayoutChangedCallback onChanged;
        };

    } // namespace panels
} // namespace ui