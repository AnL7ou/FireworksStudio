#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aSize;

out vec4 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 uCameraPos;

void main()
{
    vColor = aColor;
    
    // Transformation standard MVP
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    // Calcul de la taille du point
    // Option 1: Taille fixe (simple, recommandé pour commencer)
    // gl_PointSize = aSize;
    
    // Option 2: Taille adaptée à la distance (décommenter si souhaité)
    float dist = max(length(aPos - uCameraPos), 0.1); // éviter division par 0
    float scaleFactor = 1.0; // ajuster selon vos besoins
    gl_PointSize = aSize * scaleFactor / dist;
}
