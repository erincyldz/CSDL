#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <unordered_set>
#include <vector>

using Clock = std::chrono::high_resolution_clock;

// ============================================================
// 1. Position History: vector+erase(begin()) vs Ring Buffer
// ============================================================

struct Vec2
{
    double x, y;
};
constexpr int HISTORY_SIZE = 100;

void bench_position_history()
{
    constexpr int OBJECTS = 10;
    constexpr int FRAMES = 175 * 60;  // 60 seconds at 175 FPS

    // --- OLD: vector + erase(begin()) ---
    std::vector<std::vector<Vec2>> old_histories(OBJECTS);
    auto t0 = Clock::now();
    for (int frame = 0; frame < FRAMES; frame++)
    {
        for (int obj = 0; obj < OBJECTS; obj++)
        {
            if (old_histories[obj].size() == HISTORY_SIZE)
                old_histories[obj].erase(old_histories[obj].begin());
            old_histories[obj].push_back({static_cast<double>(frame), static_cast<double>(obj)});
        }
    }
    auto t1 = Clock::now();
    double old_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // --- NEW: ring buffer ---
    struct RingBuffer
    {
        std::array<Vec2, HISTORY_SIZE> data{};
        size_t head = 0, count = 0;
        void push(Vec2 v)
        {
            size_t idx = (head + count) % HISTORY_SIZE;
            data[idx] = v;
            if (count < HISTORY_SIZE)
                count++;
            else
                head = (head + 1) % HISTORY_SIZE;
        }
    };
    std::vector<RingBuffer> new_histories(OBJECTS);
    auto t2 = Clock::now();
    for (int frame = 0; frame < FRAMES; frame++)
    {
        for (int obj = 0; obj < OBJECTS; obj++)
        {
            new_histories[obj].push({static_cast<double>(frame), static_cast<double>(obj)});
        }
    }
    auto t3 = Clock::now();
    double new_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    printf("=== Position History (10 objects, 60s @ 175 FPS = %d frames) ===\n", FRAMES);
    printf("  OLD (vector + erase):  %8.2f ms\n", old_ms);
    printf("  NEW (ring buffer):     %8.2f ms\n", new_ms);
    printf("  Speedup:               %8.1fx\n\n", old_ms / new_ms);
}

// ============================================================
// 2. Gravity Collision Lookup: linear scan vs unordered_set
// ============================================================

void bench_gravity_lookup()
{
    constexpr int NUM_OBJECTS = 10;
    constexpr int NUM_COLLISIONS = 5;
    constexpr int FRAMES = 175 * 60;

    // Simulate some pointer values
    std::vector<void*> objects(NUM_OBJECTS);
    for (int i = 0; i < NUM_OBJECTS; i++)
        objects[i] = reinterpret_cast<void*>(0x1000 + i * 0x100);

    // Active collisions (pairs of pointers)
    std::vector<std::pair<void*, void*>> active_collisions;
    for (int i = 0; i < NUM_COLLISIONS && i + 1 < NUM_OBJECTS; i++)
        active_collisions.push_back({objects[i], objects[i + 1]});

    volatile bool sink = false;  // prevent optimization

    // --- OLD: linear scan per pair ---
    auto t0 = Clock::now();
    for (int frame = 0; frame < FRAMES; frame++)
    {
        for (int i = 0; i < NUM_OBJECTS; i++)
        {
            for (int j = i + 1; j < NUM_OBJECTS; j++)
            {
                bool in_collision = false;
                for (const auto& c : active_collisions)
                {
                    if ((c.first == objects[i] && c.second == objects[j]) ||
                        (c.first == objects[j] && c.second == objects[i]))
                    {
                        in_collision = true;
                        break;
                    }
                }
                sink = in_collision;
            }
        }
    }
    auto t1 = Clock::now();
    double old_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

    // --- NEW: unordered_set with canonical ordering ---
    struct ptr_pair_hash
    {
        size_t operator()(const std::pair<const void*, const void*>& p) const
        {
            auto h1 = std::hash<const void*>{}(p.first);
            auto h2 = std::hash<const void*>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    auto t2 = Clock::now();
    for (int frame = 0; frame < FRAMES; frame++)
    {
        std::unordered_set<std::pair<const void*, const void*>, ptr_pair_hash> collision_set;
        for (const auto& [a, b] : active_collisions)
        {
            auto lo = std::min(a, b);
            auto hi = std::max(a, b);
            collision_set.emplace(lo, hi);
        }

        for (int i = 0; i < NUM_OBJECTS; i++)
        {
            for (int j = i + 1; j < NUM_OBJECTS; j++)
            {
                auto lo = std::min(objects[i], objects[j]);
                auto hi = std::max(objects[i], objects[j]);
                sink = collision_set.count({lo, hi}) > 0;
            }
        }
    }
    auto t3 = Clock::now();
    double new_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    printf("=== Gravity Collision Lookup (10 objects, 5 collisions, %d frames) ===\n", FRAMES);
    printf("  OLD (linear scan):     %8.2f ms\n", old_ms);
    printf("  NEW (unordered_set):   %8.2f ms\n", new_ms);
    printf("  Speedup:               %8.1fx\n\n", old_ms / new_ms);

    (void)sink;
}

// ============================================================
// 3. Circle Rendering: pixel-by-pixel vs scanline (iteration count)
// ============================================================

void bench_circle_rendering()
{
    // We can't call SDL, but we can count the number of draw calls
    // which directly maps to SDL overhead

    int radii[] = {10, 25, 50, 75, 100};

    printf("=== Circle Rendering: Draw Calls Per Circle ===\n");
    printf("  %-10s  %12s  %12s  %10s\n", "Radius", "OLD (points)", "NEW (lines)", "Reduction");

    for (int radius : radii)
    {
        // OLD: count pixels inside circle
        long old_calls = 0;
        for (int w = 0; w < radius * 2; w++)
        {
            for (int h = 0; h < radius * 2; h++)
            {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius))
                    old_calls++;
            }
        }

        // NEW: one line per scanline row
        long new_calls = 0;
        for (int dy = -radius; dy <= radius; dy++)
        {
            new_calls++;  // one SDL_RenderDrawLine call
        }

        printf("  %-10d  %12ld  %12ld  %9.0fx\n", radius, old_calls, new_calls,
               static_cast<double>(old_calls) / new_calls);
    }

    // Time the actual loop computation (not SDL calls, but the iteration overhead)
    constexpr int CIRCLES = 10;
    constexpr int FRAMES = 175 * 10;  // 10 seconds
    int radius = 50;

    volatile int sink = 0;

    auto t0 = Clock::now();
    for (int f = 0; f < FRAMES; f++)
    {
        for (int c = 0; c < CIRCLES; c++)
        {
            for (int w = 0; w < radius * 2; w++)
            {
                for (int h = 0; h < radius * 2; h++)
                {
                    int dx = radius - w;
                    int dy = radius - h;
                    if ((dx * dx + dy * dy) <= (radius * radius))
                        sink++;
                }
            }
        }
    }
    auto t1 = Clock::now();

    auto t2 = Clock::now();
    for (int f = 0; f < FRAMES; f++)
    {
        for (int c = 0; c < CIRCLES; c++)
        {
            for (int dy = -radius; dy <= radius; dy++)
            {
                int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
                sink += dx;
            }
        }
    }
    auto t3 = Clock::now();

    double old_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double new_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    printf("\n  Loop overhead (10 circles x r=50, 10s @ 175 FPS):\n");
    printf("  OLD (nested pixel loop): %8.2f ms\n", old_ms);
    printf("  NEW (scanline + sqrt):   %8.2f ms\n", new_ms);
    printf("  Speedup:                 %8.1fx\n", old_ms / new_ms);
    printf(
        "  (Note: real speedup is higher because SDL_RenderDrawPoint overhead is eliminated)\n\n");

    (void)sink;
}

// ============================================================
// Main
// ============================================================

int main()
{
    printf("CSDL Engine Benchmark: Old vs New Implementations\n");
    printf("==================================================\n\n");

    bench_position_history();
    bench_gravity_lookup();
    bench_circle_rendering();

    return 0;
}
