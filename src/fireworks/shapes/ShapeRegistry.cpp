#include "ShapeRegistry.h"
#include "../rendering/Texture.h"

ShapeRegistry::ShapeRegistry() noexcept
{
    // reserve builtins
    m_shapes.resize(static_cast<size_t>(BuiltinShape::Count));
    m_textures.resize(static_cast<size_t>(BuiltinShape::Count), 0);

    // Initialise Shape entries for builtins
    for (int i = 0; i < static_cast<int>(BuiltinShape::Count); ++i) {
        Shape s;
        s.isCustom = false;
        s.builtin = static_cast<BuiltinShape>(i);
        m_shapes[static_cast<size_t>(i)] = s;
    }
}

ShapeRegistry::~ShapeRegistry() noexcept
{
    for (GLuint t : m_textures) {
        if (t != 0) glDeleteTextures(1, &t);
    }
}

bool ShapeRegistry::Initialize()
{
    // Charge les textures builtin (après création du contexte GL)
    for (int i = 0; i < static_cast<int>(BuiltinShape::Count); ++i) {
        uint16_t idx = static_cast<uint16_t>(i);
        const Shape& s = m_shapes[idx];
        std::string path = s.getAssetPath();
        GLuint tex = 0;
        if (!loadTexture(path, tex)) {
            std::cerr << "ShapeRegistry: Échec chargement builtin texture: " << path << '\n';
            tex = 0;
        }
        m_textures[idx] = tex;
        m_pathToIndex[path] = idx;
    }
    return true;
}

uint16_t ShapeRegistry::RegisterBuiltin(BuiltinShape builtin)
{
    uint16_t idx = static_cast<uint16_t>(builtin);
    // ensure vector size (should already be)
    if (idx >= m_shapes.size()) {
        m_shapes.resize(idx + 1);
        m_textures.resize(idx + 1, 0);
    }
    return idx;
}

uint16_t ShapeRegistry::RegisterCustom(const std::string& path)
{
    auto it = m_pathToIndex.find(path);
    if (it != m_pathToIndex.end()) return it->second;

    GLuint tex = 0;
    if (!loadTexture(path, tex)) {
        std::cerr << "ShapeRegistry: échec chargement custom texture: " << path << '\n';
        // fallback : crée une texture blanche 1x1
        unsigned char white[4] = { 255,255,255,255 };
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Shape s;
    s.isCustom = true;
    s.customPath = path;
    uint16_t newIndex = static_cast<uint16_t>(m_shapes.size());
    m_shapes.push_back(std::move(s));
    m_textures.push_back(tex);
    m_pathToIndex[path] = newIndex;
    return newIndex;
}

GLuint ShapeRegistry::GetTextureId(uint16_t shapeId) const noexcept
{
    if (shapeId < m_textures.size()) return m_textures[shapeId];
    return 0u;
}

const Shape& ShapeRegistry::GetShape(uint16_t shapeId) const noexcept
{
    static Shape s_default;
    if (shapeId < m_shapes.size()) return m_shapes[shapeId];
    return s_default;
}

bool ShapeRegistry::loadTexture(const std::string& path, GLuint& outTex)
{
    // Debug: afficher cwd et vérifier l'existence du fichier avant de déléguer au loader
    try {
        std::filesystem::path p(path);
        std::filesystem::path abs = std::filesystem::absolute(p);
        std::cerr << "ShapeRegistry: trying to load: " << path << " -> absolute: " << abs << "\n";
        std::cerr << "ShapeRegistry: current_path = " << std::filesystem::current_path() << "\n";
        if (!std::filesystem::exists(p)) {
            std::cerr << "ShapeRegistry: file not found at: " << abs << "\n";
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ShapeRegistry: filesystem error: " << e.what() << "\n";
        // continue to let stbi attempt if path is weird
    }

    outTex = Texture::LoadFromFile(path, true);
    if (outTex == 0) {
        std::cerr << "ShapeRegistry: Texture::LoadFromFile returned 0 for: " << path << '\n';
        return false;
    }

    std::cerr << "ShapeRegistry: loaded texId=" << outTex << " for " << path << '\n';
    return true;
}