#include "FireworkInstance.h"

#include <iostream>

FireworkInstance::FireworkInstance()
	: position(0.0f, 0.0f, 0.0f)
	, triggerTime(0.0f)
	, triggered(false)
	, fireworkTemplate(nullptr)
{
}

FireworkInstance::FireworkInstance(glm::vec3 _position, float _triggeredTime, FireworkTemplate* _fireworkTemplate)
	: position(_position)
	, triggerTime(_triggeredTime)
	, triggered(false)
	, fireworkTemplate(_fireworkTemplate)
{
}

FireworkInstance::~FireworkInstance() {}

void FireworkInstance::update(float currentTime, ParticleSystem& particleSystem)
{
	// Ne rien faire si déjà déclenché
	if (triggered)
		return;

	// Vérifier que le template est valide
	if (!fireworkTemplate)
	{
		std::cerr << "FireworkInstance::update - no FireworkTemplate assigned\n";
		return;
	}

	// Déclencher l'explosion lorsque le temps courant atteint le temps de déclenchement
	if (currentTime >= triggerTime)
	{
		std::cout << "[FireworkInstance] trigger at t=" << currentTime
			<< " pos=(" << position.x << "," << position.y << "," << position.z << ")\n";
		particleSystem.emit(*fireworkTemplate, position);
		triggered = true;
	}
}