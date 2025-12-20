#define STB_IMAGE_IMPLEMENTATION
#include <stbimage/stb_image.h>
#include "Texture.h"
#include <iostream>

GLuint Texture::LoadFromFile(const std::string& path, bool flipVertically)
{
    stbi_set_flip_vertically_on_load(flipVertically);

    int width = 0, height = 0, originalChannels = 0;

    // Forcer 4 canaux (RGBA) pour uniformiser le traitement
    // Cela garantit que toutes les textures ont un canal alpha
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &originalChannels, STBI_rgb_alpha);

    if (!data)
    {
        std::cerr << "Texture::LoadFromFile - stbi_load failed for: " << path << "\n";
        std::cerr << "  Reason: " << stbi_failure_reason() << "\n";
        return 0;
    }

    // Avec STBI_rgb_alpha, on a toujours 4 canaux
    // originalChannels contient le nombre de canaux du fichier source
    std::cerr << "Texture::LoadFromFile - loaded " << path << "\n";
    std::cerr << "  Dimensions: " << width << "x" << height << "\n";
    std::cerr << "  Original channels: " << originalChannels << " (converted to RGBA)\n";

    // Avertir si le fichier source n'avait pas d'alpha
    if (originalChannels == 3) {
        std::cerr << "  ⚠️  WARNING: Source image was RGB (no alpha)\n";
        std::cerr << "      stb_image added opaque alpha. Particles may appear as squares!\n";
        std::cerr << "      For best results, use RGBA PNG with proper alpha channel.\n";
    }

    // Créer la texture OpenGL
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Configuration importante pour textures non-power-of-2
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Paramètres de texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // Upload texture data (toujours RGBA)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Générer mipmaps pour un meilleur rendu à distance
    glGenerateMipmap(GL_TEXTURE_2D);

    // Libérer les données CPU
    stbi_image_free(data);

    // Débinder
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cerr << "  ✅ Texture created successfully, OpenGL ID: " << tex << "\n";

    return tex;
}