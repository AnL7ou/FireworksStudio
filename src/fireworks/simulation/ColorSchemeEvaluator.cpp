#include "ColorSchemeEvaluator.h"
#include <random>
#include <cmath>
#include <algorithm>

static std::mt19937 s_rng(std::random_device{}());

void ColorSchemeEvaluator::ApplyColors(
    const ColorScheme& scheme,
    const BranchLayout& layout,
    std::vector<GeneratedBranch>& branches
) {
    const int totalBranches = static_cast<int>(branches.size());
    if (totalBranches == 0) return;

    // Calculer position de chaque branche dans la grid
    int gridX = layout.gridX;
    int gridY = layout.gridY;

    for (int i = 0; i < totalBranches; ++i) {
        int ix = i % gridX;
        int iy = i / gridX;

        // Calculer couleur
        glm::vec4 color = ComputeColor(
            scheme,
            i,
            totalBranches,
            ix,
            iy,
            branches[i].direction
        );

        // Appliquer variances
        color = ApplyVariances(color, scheme.saturationVariance, scheme.brightnessVariance);

        branches[i].color = color;
        branches[i].shouldFade = scheme.fadeOverTime;
        branches[i].fadeStartRatio = scheme.fadeStartRatio;
    }
}

glm::vec4 ColorSchemeEvaluator::ComputeColor(
    const ColorScheme& scheme,
    int branchIndex,
    int totalBranches,
    int gridX,
    int gridY,
    const glm::vec3& direction
) {
    glm::vec4 baseColor(1.0f);

    switch (scheme.type) {
    case ColorDistributionType::Uniform:
        baseColor = scheme.uniformColor;
        break;

    case ColorDistributionType::Gradient: {
        float t = (totalBranches > 1)
            ? static_cast<float>(branchIndex) / (totalBranches - 1)
            : 0.0f;
        baseColor = glm::mix(scheme.gradientStart, scheme.gradientEnd, t);
        break;
    }

    case ColorDistributionType::Radial: {
        // Distance 3D depuis direction "référence" (0, 1, 0) = haut
        glm::vec3 refDir(0.0f, 1.0f, 0.0f);

        // Angle entre direction et référence
        float cosAngle = glm::dot(glm::normalize(direction), refDir);
        float angle = std::acos(std::max(-1.0f, std::min(1.0f, cosAngle)));

        // Normaliser [0, PI] → [0, 1]
        float t = angle / 3.14159265f;

        baseColor = glm::mix(scheme.gradientStart, scheme.gradientEnd, t);
        break;
    }

    case ColorDistributionType::Random: {
        if (!scheme.palette.empty()) {
            std::uniform_int_distribution<int> dist(0, static_cast<int>(scheme.palette.size()) - 1);
            baseColor = scheme.palette[dist(s_rng)];
        }
        break;
    }

    case ColorDistributionType::Alternating: {
        if (!scheme.palette.empty()) {
            int paletteIndex = branchIndex % static_cast<int>(scheme.palette.size());
            baseColor = scheme.palette[paletteIndex];
        }
        break;
    }

    case ColorDistributionType::PerBranch:
        // Custom mode - couleurs définies manuellement
        break;
    }

    return baseColor;
}

glm::vec4 ColorSchemeEvaluator::ApplyVariances(
    glm::vec4 baseColor,
    float saturationVariance,
    float brightnessVariance
) {
    if (saturationVariance <= 0.0f && brightnessVariance <= 0.0f) {
        return baseColor;
    }

    // Convertir RGB → HSV
    glm::vec3 hsv = RGBtoHSV(glm::vec3(baseColor));

    // Appliquer variance de saturation
    if (saturationVariance > 0.0f) {
        std::uniform_real_distribution<float> dist(-saturationVariance, saturationVariance);
        hsv.y = std::max(0.0f, std::min(1.0f, hsv.y + dist(s_rng)));
    }

    // Appliquer variance de luminosité
    if (brightnessVariance > 0.0f) {
        std::uniform_real_distribution<float> dist(-brightnessVariance, brightnessVariance);
        hsv.z = std::max(0.0f, std::min(1.0f, hsv.z + dist(s_rng)));
    }

    // Convertir HSV → RGB
    glm::vec3 rgb = HSVtoRGB(hsv);

    return glm::vec4(rgb, baseColor.a);
}

// ═══════════════════════════════════════════════════════════════
// CONVERSIONS HSV
// ═══════════════════════════════════════════════════════════════

glm::vec3 ColorSchemeEvaluator::RGBtoHSV(const glm::vec3& rgb)
{
    float r = rgb.r, g = rgb.g, b = rgb.b;

    float max = std::max({ r, g, b });
    float min = std::min({ r, g, b });
    float delta = max - min;

    glm::vec3 hsv;

    // Hue
    if (delta < 0.00001f) {
        hsv.x = 0.0f;
    }
    else if (max == r) {
        hsv.x = 60.0f * std::fmod(((g - b) / delta), 6.0f);
    }
    else if (max == g) {
        hsv.x = 60.0f * (((b - r) / delta) + 2.0f);
    }
    else {
        hsv.x = 60.0f * (((r - g) / delta) + 4.0f);
    }

    if (hsv.x < 0.0f) hsv.x += 360.0f;

    // Saturation
    hsv.y = (max < 0.00001f) ? 0.0f : (delta / max);

    // Value
    hsv.z = max;

    return hsv;
}

glm::vec3 ColorSchemeEvaluator::HSVtoRGB(const glm::vec3& hsv)
{
    float h = hsv.x, s = hsv.y, v = hsv.z;

    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r = 0, g = 0, b = 0;

    if (h >= 0.0f && h < 60.0f) {
        r = c; g = x; b = 0;
    }
    else if (h >= 60.0f && h < 120.0f) {
        r = x; g = c; b = 0;
    }
    else if (h >= 120.0f && h < 180.0f) {
        r = 0; g = c; b = x;
    }
    else if (h >= 180.0f && h < 240.0f) {
        r = 0; g = x; b = c;
    }
    else if (h >= 240.0f && h < 300.0f) {
        r = x; g = 0; b = c;
    }
    else {
        r = c; g = 0; b = x;
    }

    return glm::vec3(r + m, g + m, b + m);
}