#include "EditorContext.h"

#include "../fireworks/asset/FireworkAsset.h"

namespace editor {

void EditorContext::addAsset(const AssetPtr& asset) {
    if (!asset) return;
    const auto id = asset->id();
    if (m_assets.find(id) == m_assets.end()) {
        m_assetOrder.push_back(id);
    }
    m_assets[id] = asset;
}

EditorContext::AssetPtr EditorContext::getAsset(uint64_t id) const {
    auto it = m_assets.find(id);
    if (it == m_assets.end()) return nullptr;
    return it->second;
}

} // namespace editor
