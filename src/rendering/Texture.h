#pragma once

#include <string>
#include <glad/glad.h>
#include <iostream>

class Texture
{
public:
    // Retourne 0 en cas d'erreur. flipVertically = true par défaut.
    static GLuint LoadFromFile(const std::string& path, bool flipVertically = true);
};