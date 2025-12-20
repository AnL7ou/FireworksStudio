#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include "Shape.h"

// Template d'un feu d'artifice
class FireworkTemplate {
public:
	std::string name;
	unsigned int particlesCount;
	float particleLifeTime;
	// Index dans ShapeRegistry (0 = builtin Disk par défaut)
	uint16_t particleShapeId;
	int particleTextureLayerIndex;
	float explosionRadius;
	glm::vec4 baseColor;
	float baseSize;
	float speedMin;
	float speedMax;

	FireworkTemplate()
		: name("Default Firework")
		, particlesCount(100)
		, particleLifeTime(2.0f)
		, particleShapeId(static_cast<uint16_t>(BuiltinShape::Disk))
		, particleTextureLayerIndex(-1)
		, explosionRadius(5.0f)
		, baseColor(1.0f, 0.5f, 0.0f, 1.0f)
		, baseSize(1.0f)
		, speedMin(1.0f)
		, speedMax(3.0f)
	{
	}

	FireworkTemplate(const std::string& _name)
		: name(_name)
		, particlesCount(100)
		, particleLifeTime(2.0f)
		, particleShapeId(static_cast<uint16_t>(BuiltinShape::Disk))
		, particleTextureLayerIndex(-1)
		, explosionRadius(5.0f)
		, baseColor(1.0f, 0.5f, 0.0f, 1.0f)
		, baseSize(1.0f)
		, speedMin(1.0f)
		, speedMax(3.0f)
	{
	}

	~FireworkTemplate() = default;

	// Accesseurs utilitaires
	inline uint16_t GetShapeId() const noexcept { return particleShapeId; };
	inline void SetShapeId(uint16_t id) noexcept { particleShapeId = id; };
};