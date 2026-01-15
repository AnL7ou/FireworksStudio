#pragma once

#include <functional>
#include "src/fireworks/template/FireworkTemplate.h"

namespace ui {
    namespace panels {

        class TemplatePropertiesPanel {
        public:
            using ExplosionTestCallback = std::function<void(const FireworkTemplate&)>;
            using TemplateChangedCallback = std::function<void(const FireworkTemplate&)>;

            TemplatePropertiesPanel(FireworkTemplate* tmpl) noexcept;
            ~TemplatePropertiesPanel() = default;

            void SetTemplate(FireworkTemplate* tmpl) noexcept;
            void SetOnExplosionTestCallback(ExplosionTestCallback cb) noexcept;
            void SetOnTemplateChangedCallback(TemplateChangedCallback cb) noexcept;

            void Render() noexcept;

        private:
            void takeSnapshot() noexcept;
            bool detectAndNotifyChanges() noexcept;

        private:
            FireworkTemplate* fireworkTemplate;
            FireworkTemplate fireworkTemplateSnapshot;
            ExplosionTestCallback onTest;
            TemplateChangedCallback onChanged;
        };

    } // namespace panels
} // namespace ui