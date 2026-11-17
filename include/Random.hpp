#pragma once
#include <cstdint>
#include <random>

// Random numbers that are the same on every platform. std::mt19937 is specified bit for bit, but
// the standard distributions (uniform_real_distribution, bernoulli_distribution, ...) are not:
// MSVC's standard library and libstdc++ turn the same engine output into values that differ in
// the last bits, so a seed gave a slightly different scene on Windows than on Linux. These
// helpers use only the engine output and exact floating-point operations.
namespace game::portable
{
// Uniform in [0, 1) with 53 random bits (the full double mantissa).
inline double canonical(std::mt19937& rng)
{
    const std::uint64_t high = rng() >> 5;  // 27 bits; drawn first
    const std::uint64_t low = rng() >> 6;   // 26 bits
    return static_cast<double>((high << 26) | low) * 0x1.0p-53;
}

// Uniform in [a, b].
inline double uniform(std::mt19937& rng, double a, double b)
{
    return a + (b - a) * canonical(rng);
}

// True with probability p.
inline bool chance(std::mt19937& rng, double p)
{
    return canonical(rng) < p;
}
}  // namespace game::portable
