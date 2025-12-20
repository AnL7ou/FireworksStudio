#pragma once

#include <glm/glm.hpp>
#include "FireworkTemplate.h"
#include "ParticleSystem.h"

class FireworkInstance {
	private:
		glm::vec3 position;
		float triggerTime;
		bool triggered;
		FireworkTemplate* fireworkTemplate;

	public:
		FireworkInstance();
		FireworkInstance(glm::vec3 _position, float _triggeredTime, FireworkTemplate* _fireworkTemplate);
		~FireworkInstance();
		void update(float currentTime, ParticleSystem& particleSystem);

		inline bool IsTriggered() const { return triggered; }
};