#pragma once

#include <memory>
#include <string>

class Scene;
class FireworkTemplate;

namespace fireworks {
class FireworkAsset;
}

namespace serialization {

// -----------------------------
// Firework assets (template + editor metadata)
// -----------------------------

// Writes a .fwasset text file.
bool SaveFireworkAsset(const fireworks::FireworkAsset& asset, const std::string& path);

// Reads a .fwasset text file.
// Returns nullptr on failure.
std::shared_ptr<fireworks::FireworkAsset> LoadFireworkAsset(const std::string& path);

// -----------------------------
// Scenes (timeline + events)
// -----------------------------

// Writes a .fwscene text file.
bool SaveScene(const Scene& scene, const std::string& path);

// Reads a .fwscene text file.
// Returns nullptr on failure.
std::shared_ptr<Scene> LoadScene(const std::string& path);

} // namespace serialization
