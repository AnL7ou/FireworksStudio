#include "FireworkSerialization.h"

#include <fstream>
#include <iomanip>
#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "fireworks/asset/FireworkAsset.h"
#include "fireworks/template/FireworkTemplate.h"
#include "scene/Scene.h"

namespace {

constexpr int kAssetVersion = 1;
constexpr int kSceneVersion = 1;

void WriteVec3(std::ostream& out, const glm::vec3& v) {
    out << v.x << ' ' << v.y << ' ' << v.z;
}

void WriteVec4(std::ostream& out, const glm::vec4& v) {
    out << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
}

bool ReadVec3(std::istream& in, glm::vec3& v) {
    return static_cast<bool>(in >> v.x >> v.y >> v.z);
}

bool ReadVec4(std::istream& in, glm::vec4& v) {
    return static_cast<bool>(in >> v.x >> v.y >> v.z >> v.w);
}

bool Expect(std::istream& in, const char* token) {
    std::string t;
    if (!(in >> t)) return false;
    return t == token;
}

} // namespace

namespace serialization {

bool SaveFireworkAsset(const fireworks::FireworkAsset& asset, const std::string& path) {
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if (!out.is_open()) return false;

    out << "FWASSET " << kAssetVersion << '\n';

    out << "id " << asset.id() << '\n';
    out << "name " << std::quoted(asset.name()) << '\n';

    const auto& md = asset.metadata();
    out << "meta_typicalDurationSec " << md.typicalDurationSec << '\n';
    out << "meta_estimatedMaxRadius " << md.estimatedMaxRadius << '\n';
    out << "meta_dominantColorRGBA " << md.dominantColorRGBA << '\n';
    out << "meta_tags " << md.tags.size();
    for (const auto& tag : md.tags) {
        out << ' ' << std::quoted(tag);
    }
    out << '\n';

    auto t = asset.templ();
    if (!t) {
        // Still a valid asset file, but without a template.
        out << "template_present 0\n";
        return true;
    }

    out << "template_present 1\n";
    out << "t_name " << std::quoted(t->name) << '\n';

    out << "t_zoneAzimuth " << t->zoneAzimuthMin << ' ' << t->zoneAzimuthMax << '\n';
    out << "t_zoneElevation " << t->zoneElevationMin << ' ' << t->zoneElevationMax << '\n';
    out << "t_worldRotation ";
    WriteVec3(out, t->worldRotation);
    out << '\n';

    // PhysicsProfile
    out << "phys "
        << t->physics.gravity << ' '
        << t->physics.gravityCurve << ' '
        << t->physics.dragCoefficient << ' '
        << t->physics.turbulenceStrength << ' '
        << t->physics.turbulenceFrequency << ' '
        << t->physics.maxSpeed << ' '
        << t->physics.maxLifetime << '\n';

    // BranchLayout
    out << "layout "
        << t->layout.gridX << ' '
        << t->layout.gridY << ' '
        << t->layout.constraintAzimuthMin << ' '
        << t->layout.constraintAzimuthMax << ' '
        << t->layout.constraintElevationMin << ' '
        << t->layout.constraintElevationMax << ' '
        << (t->layout.staggered ? 1 : 0) << ' '
        << t->layout.randomness << '\n';

    // ColorScheme
    out << "colorscheme " << static_cast<int>(t->colorScheme.type) << '\n';
    out << "cs_uniform "; WriteVec4(out, t->colorScheme.uniformColor); out << '\n';
    out << "cs_gradientStart "; WriteVec4(out, t->colorScheme.gradientStart); out << '\n';
    out << "cs_gradientEnd "; WriteVec4(out, t->colorScheme.gradientEnd); out << '\n';
    out << "cs_palette " << t->colorScheme.palette.size();
    for (const auto& c : t->colorScheme.palette) {
        out << ' ' << c.x << ' ' << c.y << ' ' << c.z << ' ' << c.w;
    }
    out << '\n';
    out << "cs_variance " << t->colorScheme.saturationVariance << ' ' << t->colorScheme.brightnessVariance << '\n';
    out << "cs_fade " << (t->colorScheme.fadeOverTime ? 1 : 0) << ' ' << t->colorScheme.fadeStartRatio << '\n';

    // BranchDescriptor
    const auto& b = t->branchTemplate;
    out << "branch "
        << b.initialSpeed << ' ' << b.speedVariance << ' '
        << b.damping << ' ' << b.dampingVariance << ' '
        << b.gravityScale << ' ' << b.updraft << ' '
        << b.angularSpread << ' '
        << b.particlesPerBranch << ' '
        << b.emissionDuration << ' '
        << b.particleSize << ' ' << b.sizeVariance << ' '
        << b.lifetime << ' '
        << b.shapeId << ' '
        << static_cast<int>(b.visualMode) << ' '
        << (b.trailEnabled ? 1 : 0) << ' '
        << b.trailWidth << ' ' << b.trailDuration << ' ' << b.trailOpacity << ' ' << b.trailFalloffPow << ' '
        << b.frontPortion << ' ' << b.frontSpeedBias << ' ' << b.backSpeedScale << ' '
        << b.sparkleSpeedJitter << ' ' << b.sparkleSpreadMult << ' '
        << b.smokeAmount << ' '
        << b.recursionDepth << ' ' << b.recursionProb << ' '
        << (b.shouldFade ? 1 : 0) << ' ' << b.fadeStartRatio
        << '\n';

    return true;
}

std::shared_ptr<fireworks::FireworkAsset> LoadFireworkAsset(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return nullptr;

    if (!Expect(in, "FWASSET")) return nullptr;
    int version = 0;
    if (!(in >> version)) return nullptr;
    if (version <= 0 || version > kAssetVersion) return nullptr;

    uint64_t id = 0;
    std::string name;
    fireworks::FireworkAsset::Metadata md;
    bool templatePresent = false;

    if (!Expect(in, "id") || !(in >> id)) return nullptr;
    if (!Expect(in, "name") || !(in >> std::quoted(name))) return nullptr;

    if (!Expect(in, "meta_typicalDurationSec") || !(in >> md.typicalDurationSec)) return nullptr;
    if (!Expect(in, "meta_estimatedMaxRadius") || !(in >> md.estimatedMaxRadius)) return nullptr;
    if (!Expect(in, "meta_dominantColorRGBA") || !(in >> md.dominantColorRGBA)) return nullptr;

    if (!Expect(in, "meta_tags")) return nullptr;
    size_t tagCount = 0;
    if (!(in >> tagCount)) return nullptr;
    md.tags.clear();
    md.tags.reserve(tagCount);
    for (size_t i = 0; i < tagCount; ++i) {
        std::string tag;
        if (!(in >> std::quoted(tag))) return nullptr;
        md.tags.push_back(std::move(tag));
    }

    if (!Expect(in, "template_present")) return nullptr;
    int presentInt = 0;
    if (!(in >> presentInt)) return nullptr;
    templatePresent = (presentInt != 0);

    std::shared_ptr<::FireworkTemplate> t;
    if (templatePresent) {
        t = std::make_shared<::FireworkTemplate>();

        if (!Expect(in, "t_name") || !(in >> std::quoted(t->name))) return nullptr;

        if (!Expect(in, "t_zoneAzimuth") || !(in >> t->zoneAzimuthMin >> t->zoneAzimuthMax)) return nullptr;
        if (!Expect(in, "t_zoneElevation") || !(in >> t->zoneElevationMin >> t->zoneElevationMax)) return nullptr;

        if (!Expect(in, "t_worldRotation") || !ReadVec3(in, t->worldRotation)) return nullptr;

        if (!Expect(in, "phys")) return nullptr;
        if (!(in
              >> t->physics.gravity
              >> t->physics.gravityCurve
              >> t->physics.dragCoefficient
              >> t->physics.turbulenceStrength
              >> t->physics.turbulenceFrequency
              >> t->physics.maxSpeed
              >> t->physics.maxLifetime)) return nullptr;

        if (!Expect(in, "layout")) return nullptr;
        int staggered = 0;
        if (!(in
              >> t->layout.gridX
              >> t->layout.gridY
              >> t->layout.constraintAzimuthMin
              >> t->layout.constraintAzimuthMax
              >> t->layout.constraintElevationMin
              >> t->layout.constraintElevationMax
              >> staggered
              >> t->layout.randomness)) return nullptr;
        t->layout.staggered = (staggered != 0);

        if (!Expect(in, "colorscheme")) return nullptr;
        int csType = 0;
        if (!(in >> csType)) return nullptr;
        t->colorScheme.type = static_cast<ColorDistributionType>(csType);

        if (!Expect(in, "cs_uniform") || !ReadVec4(in, t->colorScheme.uniformColor)) return nullptr;
        if (!Expect(in, "cs_gradientStart") || !ReadVec4(in, t->colorScheme.gradientStart)) return nullptr;
        if (!Expect(in, "cs_gradientEnd") || !ReadVec4(in, t->colorScheme.gradientEnd)) return nullptr;

        if (!Expect(in, "cs_palette")) return nullptr;
        size_t palCount = 0;
        if (!(in >> palCount)) return nullptr;
        t->colorScheme.palette.clear();
        t->colorScheme.palette.reserve(palCount);
        for (size_t i = 0; i < palCount; ++i) {
            glm::vec4 c;
            if (!ReadVec4(in, c)) return nullptr;
            t->colorScheme.palette.push_back(c);
        }

        if (!Expect(in, "cs_variance") || !(in >> t->colorScheme.saturationVariance >> t->colorScheme.brightnessVariance)) return nullptr;
        int fadeOverTime = 0;
        if (!Expect(in, "cs_fade") || !(in >> fadeOverTime >> t->colorScheme.fadeStartRatio)) return nullptr;
        t->colorScheme.fadeOverTime = (fadeOverTime != 0);

        if (!Expect(in, "branch")) return nullptr;
        auto& b = t->branchTemplate;
        int visualMode = 0;
        int trailEnabled = 0;
        int shouldFade = 0;
        if (!(in
              >> b.initialSpeed >> b.speedVariance
              >> b.damping >> b.dampingVariance
              >> b.gravityScale >> b.updraft
              >> b.angularSpread
              >> b.particlesPerBranch
              >> b.emissionDuration
              >> b.particleSize >> b.sizeVariance
              >> b.lifetime
              >> b.shapeId
              >> visualMode
              >> trailEnabled
              >> b.trailWidth >> b.trailDuration >> b.trailOpacity >> b.trailFalloffPow
              >> b.frontPortion >> b.frontSpeedBias >> b.backSpeedScale
              >> b.sparkleSpeedJitter >> b.sparkleSpreadMult
              >> b.smokeAmount
              >> b.recursionDepth >> b.recursionProb
              >> shouldFade >> b.fadeStartRatio)) return nullptr;

        b.visualMode = static_cast<BranchDescriptor::VisualMode>(visualMode);
        b.trailEnabled = (trailEnabled != 0);
        b.shouldFade = (shouldFade != 0);

        // Derived data.
        t->RegenerateBranches();
    }

    return std::make_shared<fireworks::FireworkAsset>(id, std::move(name), std::move(t), std::move(md));
}

bool SaveScene(const Scene& scene, const std::string& path) {
    std::ofstream out(path, std::ios::out | std::ios::trunc);
    if (!out.is_open()) return false;

    out << "FWSCENE " << kSceneVersion << '\n';
    out << "name " << std::quoted(scene.GetName()) << '\n';
    out << "duration " << scene.GetDuration() << '\n';

    const auto& events = scene.GetEvents();
    out << "events " << events.size() << '\n';
    for (const auto& e : events) {
        out << "event "
            << e.templateId << ' ';
        WriteVec3(out, e.position);
        out << ' '
            << e.triggerTime << ' '
            << (e.enabled ? 1 : 0) << ' '
            << std::quoted(e.label)
            << '\n';
    }
    return true;
}

std::shared_ptr<Scene> LoadScene(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return nullptr;

    if (!Expect(in, "FWSCENE")) return nullptr;
    int version = 0;
    if (!(in >> version)) return nullptr;
    if (version <= 0 || version > kSceneVersion) return nullptr;

    std::string name;
    float duration = 0.0f;
    if (!Expect(in, "name") || !(in >> std::quoted(name))) return nullptr;
    if (!Expect(in, "duration") || !(in >> duration)) return nullptr;

    if (!Expect(in, "events")) return nullptr;
    size_t count = 0;
    if (!(in >> count)) return nullptr;

    auto scene = std::make_shared<Scene>(name);
    scene->SetDuration(duration);

    auto& ev = scene->GetEvents();
    ev.clear();
    ev.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        if (!Expect(in, "event")) return nullptr;
        FireworkEvent e;
        int enabled = 0;
        if (!(in >> e.templateId)) return nullptr;
        if (!ReadVec3(in, e.position)) return nullptr;
        if (!(in >> e.triggerTime >> enabled >> std::quoted(e.label))) return nullptr;
        e.enabled = (enabled != 0);
        ev.push_back(std::move(e));
    }

    scene->SortByTime();
    return scene;
}

} // namespace serialization
