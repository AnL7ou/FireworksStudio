#pragma once

// Minimal timeline controller for the Scene editor.
// Keeps current time, playback, and one-shot event triggering.

class Timeline {
public:
    Timeline();

    void Reset();

    bool IsPlaying() const { return playing; }
    void SetPlaying(bool p) { playing = p; }

    float GetTime() const { return timeSeconds; }
    void SetTime(float t);

    // Advances time if playing; clamps to [0, duration].
    void Update(float deltaSeconds, float durationSeconds);

    // For one-shot triggering: remember which point in time we already processed.
    float GetLastDispatchedTime() const { return lastDispatchedTime; }
    void SetLastDispatchedTime(float t) { lastDispatchedTime = t; }

private:
    bool playing;
    float timeSeconds;
    float lastDispatchedTime;
};
