#pragma once

#include <glm/glm.hpp>
#include <vector>

// Stratégie de distribution des couleurs sur les branches
enum class ColorDistributionType {
    Uniform,        // Toutes les branches = même couleur
    Gradient,       // Dégradé linéaire entre start/end
    Radial,         // Dégradé radial depuis le centre
    Random,         // Couleurs aléatoires dans une palette
    PerBranch,      // Chaque branche a sa couleur définie (mode Custom)
    Alternating     // Alternance de couleurs (ex: rouge/bleu/rouge/bleu)
};

// Schéma de couleurs pour un FireworkTemplate
struct ColorScheme {
    ColorDistributionType type;

    // ═══ UNIFORM ═══
    glm::vec4 uniformColor;

    // ═══ GRADIENT & RADIAL ═══
    glm::vec4 gradientStart;      // Couleur de départ
    glm::vec4 gradientEnd;        // Couleur de fin

    // ═══ RANDOM & ALTERNATING ═══
    std::vector<glm::vec4> palette;  // Liste de couleurs à utiliser

    // ═══ VARIATIONS ═══
    float saturationVariance;     // 0-1 : variation de saturation par branche
    float brightnessVariance;     // 0-1 : variation de luminosité par branche

    // ═══ FADE TEMPOREL ═══
    bool fadeOverTime;            // Les particules fadent vers transparent
    float fadeStartRatio;         // À quel % de lifetime le fade commence (0-1)

    // Constructeur par défaut : blanc uniforme
    ColorScheme()
        : type(ColorDistributionType::Uniform)
        , uniformColor(1.0f, 1.0f, 1.0f, 1.0f)
        , gradientStart(1.0f, 0.0f, 0.0f, 1.0f)  // Rouge
        , gradientEnd(0.0f, 0.0f, 1.0f, 1.0f)    // Bleu
        , saturationVariance(0.0f)
        , brightnessVariance(0.0f)
        , fadeOverTime(true)
        , fadeStartRatio(0.7f)
    {
        // Palette par défaut : rouge, blanc, bleu
        palette.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
        palette.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        palette.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
    }

    // Presets utiles
    static ColorScheme RedWhiteBlue() {
        ColorScheme cs;
        cs.type = ColorDistributionType::Alternating;
        cs.palette.clear();
        cs.palette.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));  // Rouge
        cs.palette.push_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));  // Blanc
        cs.palette.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));  // Bleu
        return cs;
    }

    static ColorScheme Rainbow() {
        ColorScheme cs;
        cs.type = ColorDistributionType::Gradient;
        cs.gradientStart = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // Rouge
        cs.gradientEnd = glm::vec4(0.5f, 0.0f, 1.0f, 1.0f);    // Violet
        return cs;
    }

    static ColorScheme GoldenCore() {
        ColorScheme cs;
        cs.type = ColorDistributionType::Radial;
        cs.gradientStart = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);    // Rouge centre
        cs.gradientEnd = glm::vec4(1.0f, 0.843f, 0.0f, 1.0f);    // Or extérieur
        return cs;
    }
};