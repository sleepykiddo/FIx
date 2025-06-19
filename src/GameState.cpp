#include "../include/GameState.h"

#include <algorithm>  // For std::shuffle
#include <random>     // For std::random_device, std::mt19937

GameState::GameState()
    : grid(ROWS, std::vector<Cell>(COLS)),
      players(PLAYERS),
      turn_number(0),
      game_over(false),
      winner(std::nullopt) {
    // Initialize player order with randomized turn sequence
    std::vector<int> temp_player_order;
    for (int i = 0; i < PLAYERS; i++) {
        temp_player_order.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());  // Use a named mt19937 engine
    std::shuffle(temp_player_order.begin(), temp_player_order.end(), g);
    player_order   = temp_player_order;
    current_player = player_order[0];

    // Initialize turn messages
    turn_messages = {"Red player's turn - Place your tile!",
                     "Green player's turn - Place your tile!",
                     "Blue player's turn - Place your tile!",
                     "Yellow player's turn - Place your tile!"};

    // Initialize game state flags
    first_moves   = std::vector<bool>(PLAYERS, true);
    players_alive = std::vector<bool>(PLAYERS, true);
}

void GameState::place_tile(int r, int c) {
    if (game_over) return;
    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) return;  // Boundary check

    Cell& cell = grid[r][c];
    // Allow placing a tile if it's the player's first move and the cell is
    // empty, or if it's not their first move and they own the cell.
    if ((first_moves[current_player] && !cell.owner.has_value()) ||
        (!first_moves[current_player] && cell.owner.has_value() &&
         cell.owner.value() == current_player)) {
        cell.owner = current_player;
        if (first_moves[current_player]) {
            cell.power                  = 3;  // First move gives 3 power
            first_moves[current_player] = false;
        } else {
            cell.power++;
        }

        turn_number++;  // Increment turn number after a successful move

        // Trigger explosion if power reaches threshold (e.g., 4)
        if (cell.power >= 4) {
            explode_cell(r, c, current_player);
        }

        check_elimination();
        check_game_over();

        if (!game_over) {
            do {
                current_player = (current_player + 1) % PLAYERS;
            } while (
                !players_alive[current_player]);  // Skip eliminated players
        }
    }
}

void GameState::explode_cell(int r, int c, int exploding_player) {
    Cell& cell = grid[r][c];
    cell.power = 0;             // Reset power
    cell.owner = std::nullopt;  // Cell becomes neutral after explosion

    // Define adjacent cells (up, down, left, right)
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; i++) {
        int nr = r + dr[i];
        int nc = c + dc[i];

        if (nr >= 0 && nr < ROWS && nc >= 0 && nc < COLS) {  // Check bounds
            Cell& neighbor = grid[nr][nc];
            neighbor.owner = exploding_player;  // Capture neighbor
            neighbor.power++;

            // If neighbor also reaches threshold, trigger recursive explosion
            if (neighbor.power >= 4) {
                explode_cell(nr, nc, exploding_player);
            }
        }
    }
}

void GameState::check_elimination() {
    if (turn_number == 0)
        return;  // Don't check for eliminations before any move

    for (int p = 0; p < PLAYERS; p++) {
        if (!players_alive[p]) continue;  // Skip already eliminated players
        if (first_moves[p])
            continue;  // Players who haven't made a move can't be eliminated
                       // yet

        bool has_tiles = false;
        for (const auto& row_vec : grid) {
            for (const auto& cell : row_vec) {
                if (cell.owner.has_value() && cell.owner.value() == p) {
                    has_tiles = true;
                    break;
                }
            }
            if (has_tiles) break;
        }
        if (!has_tiles) {
            players_alive[p] = false;
        }
    }
}

void GameState::check_game_over() {
    if (turn_number == 0 && PLAYERS > 1)
        return;  // Game can't be over before any significant moves unless only
                 // 1 player

    int alive_count       = 0;
    int last_alive_player = -1;

    for (int p = 0; p < PLAYERS; p++) {
        if (players_alive[p]) {
            alive_count++;
            last_alive_player = p;
        }
    }

    // Game over if 1 or 0 players are left (and at least one turn has passed
    // for multi-player games)
    if (alive_count <= 1 && (turn_number > 0 || PLAYERS <= 1)) {
        game_over = true;
        if (alive_count == 1) {
            winner = last_alive_player;
        } else {
            winner = std::nullopt;  // No winner if 0 players left (draw or
                                    // simultaneous elimination)
        }
    }
}