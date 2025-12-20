#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <iostream>
#include "Particle.h"
#include "FireworkTemplate.h"



class ParticleSystem {
private:
	std::vector<Particle> particles;
	unsigned int maxParticles;
	unsigned int lastUsedParticle;

public:
	ParticleSystem();
	ParticleSystem(unsigned int _maxParticles);
	~ParticleSystem();
	void update(float deltaTime);
	// emit utilise désormais le drapeau deterministic contenu dans FireworkTemplate
	void emit(const FireworkTemplate& fireworkTemplate, const glm::vec3& spawnPosition);
	inline std::vector<Particle>& getPaticles() { return particles; }

private:
	unsigned int firstUnusedParticle();
	void respawnParticle(Particle& particle, const FireworkTemplate& fireworkTemplate, const glm::vec3& spawnPosition);
};