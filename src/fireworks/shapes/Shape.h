#pragma once

#include <string>

enum class BuiltinShape {
    Disk,
    Rice,
    Star,
    Spark,
    Flower,
    Count
};

// Représente soit une forme builtin, soit une forme chargée par l'utilisateur
struct Shape {
    // si true -> utiliser customPath, sinon utiliser builtin
    bool isCustom = false;
    BuiltinShape builtin = BuiltinShape::Disk;
    std::string customPath; // chemin vers le fichier shape (ex: assets/shapes/myshape.png)

    // Retourne le chemin d'asset à charger (builtin -> path dans assets)
    std::string getAssetPath() const {
        if (isCustom && !customPath.empty()) {
            return customPath;
        }

        switch (builtin) {
        case BuiltinShape::Disk:   return "../assets/shapes/disk.png";
        case BuiltinShape::Rice:   return "../assets/shapes/rice.png";
        case BuiltinShape::Star:   return "../assets/shapes/star.png";
        case BuiltinShape::Spark:  return "../assets/shapes/spark.png";
        case BuiltinShape::Flower: return "../assets/shapes/flower.png";
        default:                   return "../assets/shapes/disk.png";
        }
    }

    // Nom lisible pour UI
    std::string displayName() const {
        if (isCustom) return "Custom: " + customPath;
        switch (builtin) {
        case BuiltinShape::Disk:   return "Disk";
        case BuiltinShape::Rice:   return "Rice";
        case BuiltinShape::Star:   return "Star";
        case BuiltinShape::Spark:  return "Spark";
        case BuiltinShape::Flower: return "Flower";
        default:                   return "Unknown";
        }
    }
};