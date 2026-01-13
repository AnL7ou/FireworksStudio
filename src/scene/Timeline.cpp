#include "Timeline.h"

Timeline::Timeline()
    : playing(false)
    , timeSeconds(0.0f)
    , lastDispatchedTime(0.0f)
{
}

void Timeline::Reset()
{
    timeSeconds = 0.0f;
    lastDispatchedTime = 0.0f;
    playing = false;
}

void Timeline::SetTime(float t)
{
    timeSeconds = (t < 0.0f) ? 0.0f : t;
}

void Timeline::Update(float deltaSeconds, float durationSeconds)
{
    if (!playing) {
        return;
    }
    float d = (deltaSeconds < 0.0f) ? 0.0f : deltaSeconds;
    timeSeconds += d;
    if (durationSeconds >= 0.0f && timeSeconds > durationSeconds) {
        timeSeconds = durationSeconds;
        playing = false;
    }
}
