#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <filesystem>

#include "Shape.h"

// Minimal shape registry : charge et met en cache les textures (stb_image)
// Indices 0..BuiltinCount-1 -> builtin shapes
class ShapeRegistry {
public:
    ShapeRegistry() noexcept;
    ~ShapeRegistry() noexcept;

    // Initialise (charge les builtin). Appeler après contexte GL prêt.
    bool Initialize();

    // Retourne l'index pour un builtin (toujours le même)
    uint16_t RegisterBuiltin(BuiltinShape builtin);

    // Enregistre un shape custom (charge la texture). Retourne index.
    uint16_t RegisterCustom(const std::string& path);

    // Renvoie la texture OpenGL (0 si introuvable)
    GLuint GetTextureId(uint16_t shapeId) const noexcept;

    // Renvoie le descriptor Shape (valide si index < size)
    const Shape& GetShape(uint16_t shapeId) const noexcept;

    // Nombre total d'entrées (builtin + customs)
    size_t Count() const noexcept { return m_shapes.size(); }

private:
    bool loadTexture(const std::string& path, GLuint& outTex);

private:
    std::vector<Shape> m_shapes;
    std::vector<GLuint> m_textures;
    std::unordered_map<std::string, uint16_t> m_pathToIndex;
};