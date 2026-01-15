#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// FireworkTemplate currently lives in the global namespace.
class FireworkTemplate;

namespace fireworks {

// Editor-facing, serializable unit.
//
// - FireworkTemplate stays "physics-only".
// - FireworkAsset wraps a template with authoring metadata (name, tags, UX hints).
//
// Rationale:
// The editor should manipulate stable, persistent "assets" rather than directly
// poking runtime simulation structures. This decouples UI/authoring from physics.
class FireworkAsset {
public:
    struct Metadata {
        // UX-only hints (not used by simulation).
        float typicalDurationSec = 2.0f;          // Default preview timeline
        float estimatedMaxRadius = 1.0f;          // Preview framing / warnings
        uint32_t dominantColorRGBA = 0xFFFFFFFFu; // Optional quick swatch
        std::vector<std::string> tags;            // e.g. "sparkle", "smoke"
    };

    FireworkAsset() = default;
    FireworkAsset(uint64_t id,
                  std::string name,
                  std::shared_ptr<::FireworkTemplate> templ,
                  Metadata metadata = {});

    uint64_t id() const noexcept { return m_id; }
    const std::string& name() const noexcept { return m_name; }
    const Metadata& metadata() const noexcept { return m_metadata; }

    // The wrapped template should be treated as immutable during simulation.
    std::shared_ptr<const ::FireworkTemplate> templ() const noexcept { return m_template; }

    // Editor-side mutation (authoring time only).
    void setName(std::string name) { m_name = std::move(name); }
    void setMetadata(Metadata md) { m_metadata = std::move(md); }
    void setTemplate(std::shared_ptr<::FireworkTemplate> t) { m_template = std::move(t); }

private:
    uint64_t m_id = 0;
    std::string m_name;
    std::shared_ptr<::FireworkTemplate> m_template;
    Metadata m_metadata;
};

} // namespace fireworks
