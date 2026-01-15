#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../template/FireworkTemplate.h"
#include "../particle/ParticlePool.h"
#include "../template/PhysicsProfile.h"

class FireworkInstance {
private:
    const FireworkTemplate* fireworkTemplate;
    glm::vec3 position;
    float triggerTime;
    bool triggered;

    struct BranchEmitter {
        const GeneratedBranch* branch = nullptr;
        int emitted = 0;
        float accum = 0.0f;
        float interval = 0.0f; // seconds between spawns; 0 => burst
        bool done = false;
    };

    std::vector<BranchEmitter> emitters;
    bool finished;

public:
    FireworkInstance();
    FireworkInstance(const FireworkTemplate* tmpl, const glm::vec3& pos, float trigTime);
    ~FireworkInstance();

    void Update(float currentTime, float deltaTime, ParticlePool& pool);

    inline bool IsTriggered() const { return triggered; }
    inline bool IsFinished() const { return finished; }
    inline const glm::vec3& GetPosition() const { return position; }
    inline float GetTriggerTime() const { return triggerTime; }
};