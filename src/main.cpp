#include <SDL.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL_ttf.h>

#include <iostream>
#include <string>
#include <vector>

#include "../include/GameState.h"

// Define colors for SDL (SDL_Color has r, g, b, a components)
SDL_Color get_player_color(int player) {
    switch (player) {
        case 0:
            return {255, 0, 0, 255};  // Red
        case 1:
            return {0, 255, 0, 255};  // Green
        case 2:
            return {0, 0, 255, 255};  // Blue
        case 3:
            return {255, 255, 0, 255};  // Yellow
        default:
            return {255, 255, 255, 255};  // White
    }
}

void draw_board(SDL_Renderer* renderer, GameState& state, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black background
    SDL_RenderClear(renderer);

    // Cell outline and fill
    SDL_Rect cell_rect;
    cell_rect.w = static_cast<int>(CELL_SIZE - 2.0f);
    cell_rect.h = static_cast<int>(CELL_SIZE - 2.0f);

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            cell_rect.x = static_cast<int>(c * CELL_SIZE + BOARD_MARGIN);
            cell_rect.y = static_cast<int>(r * CELL_SIZE + BOARD_MARGIN);

            // Draw cell background (grey)
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
            SDL_RenderFillRect(renderer,
                               reinterpret_cast<const SDL_FRect*>(&cell_rect));

            // Draw cell outline (white)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer,
                           reinterpret_cast<const SDL_FRect*>(&cell_rect));

            Cell& cell = state.grid[r][c];
            if (cell.owner.has_value()) {
                int owner       = cell.owner.value();
                SDL_Color color = get_player_color(owner);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b,
                                       color.a);

                // Draw a smaller filled rectangle for the player tile
                SDL_Rect tile_rect;
                tile_rect.w = static_cast<int>(CELL_SIZE / 1.5f);
                tile_rect.h = static_cast<int>(CELL_SIZE / 1.5f);
                tile_rect.x = cell_rect.x + static_cast<int>(CELL_SIZE / 6.0f);
                tile_rect.y = cell_rect.y + static_cast<int>(CELL_SIZE / 6.0f);
                SDL_RenderFillRect(
                    renderer, reinterpret_cast<const SDL_FRect*>(&tile_rect));
            }

            if (cell.power > 0) {
                SDL_Color text_color        = {255, 255, 255, 255};  // White
                std::string power_text_str  = std::to_string(cell.power);
                SDL_Surface* surfaceMessage = TTF_RenderText_Solid(
                    font, power_text_str.c_str(),
                    {text_color.r, text_color.g, text_color.b, text_color.a});
                SDL_Texture* Message =
                    SDL_CreateTextureFromSurface(renderer, surfaceMessage);

                SDL_Rect Message_rect;  // create a rect
                Message_rect.x = cell_rect.x +
                                 static_cast<int>(CELL_SIZE / 2.0f) -
                                 12;  // controls the rect's x coordinate
                Message_rect.y = cell_rect.y +
                                 static_cast<int>(CELL_SIZE / 2.0f) -
                                 12;  // controls the rect's y coordinte
                Message_rect.w = 24;  // controls the width of the rect
                Message_rect.h = 24;  // controls the height of the rect

                SDL_RenderTexture(
                    renderer, Message, NULL,
                    reinterpret_cast<const SDL_FRect*>(&Message_rect));
                SDL_DestroySurface(surfaceMessage);
                SDL_DestroyTexture(Message);
            }
        }
    }

    // Draw turn message
    SDL_Color text_color;
    std::string message_str;
    if (state.game_over) {
        if (state.winner.has_value()) {
            int winner  = state.winner.value();
            message_str = "Player " + std::to_string(winner) + " wins!";
            text_color  = get_player_color(winner);
        }
    } else {
        message_str = state.turn_messages[state.current_player];
        text_color  = get_player_color(state.current_player);
    }

    if (!message_str.empty() && font) {
        SDL_Surface* surfaceMessage =
            TTF_RenderText_Solid(font, message_str.c_str(), text_color);
        SDL_Texture* Message =
            SDL_CreateTextureFromSurface(renderer, surfaceMessage);
        SDL_Rect Message_rect;
        Message_rect.x = static_cast<int>(BOARD_MARGIN);
        Message_rect.y = static_cast<int>(BOARD_MARGIN / 2);
        TTF_SizeText(font, message_str.c_str(), &Message_rect.w,
                     &Message_rect.h);
        SDL_RenderTexture(renderer, Message, NULL,
                          reinterpret_cast<const SDL_FRect*>(&Message_rect));
        SDL_DestroySurface(surfaceMessage);
        SDL_DestroyTexture(Message);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError()
                  << std::endl;
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: "
                  << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Color War SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        static_cast<int>(COLS * CELL_SIZE + BOARD_MARGIN * 2),
        static_cast<int>(ROWS * CELL_SIZE + 2 * BOARD_MARGIN), 0);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED_VSYNC);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: "
                  << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font =
        TTF_OpenFont("Arial.ttf", 24);  // Ensure Arial.ttf is in the execution
                                        // directory or provide full path
    if (font == nullptr) {
        std::cerr << "Failed to load font! SDL_Error: " << SDL_GetError()
                  << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    GameState state;
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            } else if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    int x = e.button.x - static_cast<int>(BOARD_MARGIN);
                    int y = e.button.y - static_cast<int>(BOARD_MARGIN);

                    if (x >= 0 && y >= 0) {
                        int row = static_cast<int>(y / CELL_SIZE);
                        int col = static_cast<int>(x / CELL_SIZE);

                        if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
                            state.place_tile(row, col);
                        }
                    }
                }
            }
        }

        draw_board(renderer, state, font);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}