#pragma once

#include <functional>
#include "src/fireworks/template/ColorScheme.h"

namespace ui {
    namespace panels {
        class ColorSchemePanel {
        public:
            using ColorSchemeChangedCallback = std::function<void(const ColorScheme&)>;

            ColorSchemePanel(ColorScheme* scheme) noexcept;
            ~ColorSchemePanel() = default;

            void SetColorScheme(ColorScheme* scheme) noexcept;
            void SetOnColorSchemeChangedCallback(ColorSchemeChangedCallback cb) noexcept;

            void Render() noexcept;

        private:
            void RenderUniformSettings() noexcept;
            void RenderGradientSettings() noexcept;
            void RenderRadialSettings() noexcept;
            void RenderPaletteSettings() noexcept;

            bool DetectAndNotifyChanges() noexcept;
            void TakeSnapshot() noexcept;

        private:
            ColorScheme* scheme;
            ColorScheme snapshot;
            ColorSchemeChangedCallback onChanged;
        };
    }
}