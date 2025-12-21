#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
	: maxParticles(1000)
	, lastUsedParticle(0)
{
	particles.resize(maxParticles);
}

ParticleSystem::ParticleSystem(unsigned int _maxParticles)
	: maxParticles(_maxParticles > 0 ? _maxParticles : 1u)
	, lastUsedParticle(0)
{
	particles.resize(maxParticles);
}

ParticleSystem::~ParticleSystem()
{
	// Rien de particulier à libérer : std::vector s'en charge
}

void ParticleSystem::update(float deltaTime)
{
    for (auto& particle : particles)
    {
        if (!particle.active)
            continue;

        // Décrémente la durée de vie
        particle.lifeTime -= deltaTime;

        // Si la particule est morte, on la désactive
        if (particle.lifeTime <= 0.0f)
        {
            particle.active = false;
            continue;
        }

        // Mise à jour de la position
        // Ici simple mouvement : position += velocity * deltaTime
        particle.position += particle.velocity * deltaTime;

        // Optionnel : appliquer une gravité
        // Exemple : accélération vers le bas
        glm::vec3 gravity(0.0f, -9.81f, 0.0f);
        particle.velocity += gravity * deltaTime;

        // Optionnel : damping ou friction
        float damping = 0.3f;
        particle.velocity *= damping;

        // Optionnel : fade out de la couleur selon la vie
        // particle.color.a = particle.life / fireworkTemplate.particleLifetime;
    }
}

void ParticleSystem::emit(const FireworkTemplate& fireworkTemplate, const glm::vec3& spawnPosition)
{
    for (unsigned int i = 0; i < fireworkTemplate.particlesCount; ++i)
    {
        // Trouve une particule inactive
        unsigned int particleIndex = firstUnusedParticle();
        Particle& particle = particles[particleIndex];

        // Réinitialise la particule avec le template
        respawnParticle(particle, fireworkTemplate, spawnPosition);
    }
}



unsigned int ParticleSystem::firstUnusedParticle()
{
    // Commence la recherche depuis la dernière particule utilisée
    for (unsigned int i = lastUsedParticle; i < particles.size(); ++i)
    {
        if (!particles[i].active)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    // Si aucune inactive trouvée, recherche depuis le début
    for (unsigned int i = 0; i < lastUsedParticle; ++i)
    {
        if (!particles[i].active)
        {
            lastUsedParticle = i;
            return i;
        }
    }

    // Toutes les particules sont actives : réutilise la première
    lastUsedParticle = 0;
    return 0;
}


void ParticleSystem::respawnParticle(Particle& particle, const FireworkTemplate& fireworkTemplate, const glm::vec3& spawnPosition)
{
    // Position initiale
    particle.position = spawnPosition;

    // Direction aléatoire
    float randomX = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
    float randomY = ((rand() % 100) / 100.0f) * 1.0f;
    float randomZ = ((rand() % 100) / 100.0f - 0.5f) * 2.0f;
    glm::vec3 randomDirection = glm::normalize(glm::vec3(randomX, randomY, randomZ));

    // Vitesse
    float speed = fireworkTemplate.speedMin + static_cast<float>(rand()) / RAND_MAX *
        (fireworkTemplate.speedMax - fireworkTemplate.speedMin);
    particle.velocity = randomDirection * speed;

    // Couleur et taille
    particle.color = fireworkTemplate.baseColor;
    particle.size = fireworkTemplate.baseSize;

    // Apparence : assigner l'index de forme (shapeId) depuis le template
    // NOTE: FireworkTemplate doit exposer `uint16_t particleShapeId`.
    particle.shapeId = fireworkTemplate.particleShapeId;

    // Durée de vie
    particle.lifeTime = fireworkTemplate.particleLifeTime;

    // Active la particule
    particle.active = true;
}

