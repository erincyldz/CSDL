#pragma once
#include <SDL2/SDL.h>

namespace game
{
// Fixed-timestep clock ("Fix Your Timestep"). Frame time is clamped so a long stall (window
// drag, breakpoint) can't trigger a spiral of death.
class FrameClock
{
  public:
    explicit FrameClock(double fixedStep, double maxFrameTime = 0.25);

    // Call once per frame; returns the (clamped) frame time in seconds.
    double tick();
    // Returns true and consumes one fixed step while enough time has accumulated.
    bool consumeStep();
    // Sleeps so that the frame rate doesn't exceed maxFps (no-op when maxFps <= 0).
    void limitFrameRate(int maxFps) const;

    double fixedStep() const
    {
        return m_fixedStep;
    }
    void setFixedStep(double step)
    {
        m_fixedStep = step;
    }
    // Simulated time advances `scale` times faster than real time; the step size is unchanged,
    // so slow motion / fast forward don't affect the integration accuracy.
    void setTimeScale(double scale)
    {
        m_timeScale = scale;
    }
    double frameTime() const
    {
        return m_frameTime;
    }
    // Leftover fraction of a step in [0, 1); usable for render interpolation.
    double alpha() const
    {
        return m_accumulator / m_fixedStep;
    }

  private:
    double m_fixedStep;
    double m_maxFrameTime;
    double m_frequency;
    Uint64 m_lastCounter;
    double m_accumulator = 0.0;
    double m_timeScale = 1.0;
    double m_frameTime = 0.0;
};
}  // namespace game
