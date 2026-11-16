#include "FrameClock.hpp"

#include <algorithm>

namespace game
{
FrameClock::FrameClock(double fixedStep, double maxFrameTime)
    : m_fixedStep(fixedStep), m_maxFrameTime(maxFrameTime),
      m_frequency(static_cast<double>(SDL_GetPerformanceFrequency())),
      m_lastCounter(SDL_GetPerformanceCounter())
{
}

double FrameClock::tick()
{
    const Uint64 now = SDL_GetPerformanceCounter();
    const double elapsed = static_cast<double>(now - m_lastCounter) / m_frequency;
    m_lastCounter = now;
    m_frameTime = std::min(elapsed, m_maxFrameTime);
    m_accumulator += m_frameTime * m_timeScale;
    return m_frameTime;
}

bool FrameClock::consumeStep()
{
    if (m_accumulator < m_fixedStep)
        return false;
    m_accumulator -= m_fixedStep;
    return true;
}

void FrameClock::limitFrameRate(int maxFps) const
{
    if (maxFps <= 0)
        return;
    const double target = 1.0 / maxFps;
    const double spent =
        static_cast<double>(SDL_GetPerformanceCounter() - m_lastCounter) / m_frequency;
    if (spent < target)
        SDL_Delay(static_cast<Uint32>((target - spent) * 1000.0));
}
}  // namespace game
