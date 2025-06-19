#pragma once
#include <optional>
#include <chrono> // For time-based animations if needed later, though SDL handles rendering loop
#include <utility> // For std::pair

struct Cell {
    std::optional<int> owner;
    int power;
    // SDL animation can be handled differently, e.g., in the main game loop or rendering logic
    // For now, let's remove SFML-specific animation members
    // std::optional<std::chrono::steady_clock::time_point> animation_start;
    // std::optional<std::pair<float, float>> animation_from;
    // bool is_exploding;
    // float animation_delay;

    Cell();
};