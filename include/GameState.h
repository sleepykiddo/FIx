#pragma once
#include <optional>
#include <string>
#include <tuple>  // For std::tuple if animations are re-implemented
#include <vector>

#include "Cell.h"

// Game constants - these can be adjusted as needed
const int ROWS           = 8;
const int COLS           = 8;
const float CELL_SIZE    = 50.0f;  // Used for rendering calculations
const int PLAYERS        = 4;
const float BOARD_MARGIN = 40.0f;  // Adjusted margin for SDL rendering

class GameState {
   public:
    std::vector<std::vector<Cell>> grid;
    int players;
    int current_player;
    // Animations might be handled differently in SDL, e.g., within the main
    // loop or rendering functions std::vector<std::tuple<int, int,
    // std::pair<float, float>>> animations;
    std::vector<int> player_order;
    int turn_number;
    std::vector<std::string> turn_messages;
    std::vector<bool> first_moves;
    std::vector<bool> players_alive;
    bool game_over;
    std::optional<int> winner;

    GameState();
    void place_tile(int r, int c);
    void explode_cell(int r, int c, int exploding_player);
    void check_elimination();
    void check_game_over();
    // Potentially add methods for SDL-specific updates or event handling if
    // needed
};