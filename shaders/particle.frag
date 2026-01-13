#version 330 core

// Entrées depuis le vertex shader
in vec4 vColor;

// Sortie finale
out vec4 FragColor;

// Uniforms
uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform bool uUseMask;  // Si true, utilise le rouge comme alpha

void main()
{
    vec4 texColor = vec4(1.0, 1., 1., 0.);  // Blanc opaque par défaut
    
    if (uHasTexture) {
        // Échantillonner la texture
        texColor = texColor + texture(uTexture, gl_PointCoord) * vec4(0., 0., 0., 1);
        
        if (uUseMask) {
            // Utiliser le canal rouge comme masque d'alpha
            // Utile pour textures en niveaux de gris
            texColor.a = texColor.r;
        }
    }
    
    // Couleur finale = couleur de la particule * texture
    FragColor = vColor * texColor;
    
    // Optimisation: rejeter les pixels complètement transparents
    // (évite d'écrire dans le framebuffer inutilement)
    if (FragColor.a < 0.01) {
        discard;
    }
}
