#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fireworks {
class FireworkAsset;
}

namespace editor {

// Lightweight container for editor-side state.
// This is intentionally independent from runtime simulation state.
class EditorContext {
public:
    using AssetPtr = std::shared_ptr<fireworks::FireworkAsset>;

    // Asset registry (palette, search, drag source).
    void addAsset(const AssetPtr& asset);
    AssetPtr getAsset(uint64_t id) const;

    const std::vector<uint64_t>& assetOrder() const noexcept { return m_assetOrder; }

private:
    std::unordered_map<uint64_t, AssetPtr> m_assets;
    std::vector<uint64_t> m_assetOrder;
};

} // namespace editor
