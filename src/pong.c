#include "pong.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BALL_WIDTH 15
#define BALL_HEIGHT 15
#define BALL_SPEED 150

#define PLAYER_SPEED 150

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

static void initialize_window(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        perror("Error initializing SDL.\n");
        exit(EXIT_FAILURE);
    }

    *window = SDL_CreateWindow(
            NULL,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_BORDERLESS);
    if (!*window) {
        perror("Error creating SDL Window.\n");
        exit(EXIT_FAILURE);
    }

    *renderer = SDL_CreateRenderer(*window, -1, 0);
    if (!*renderer) {
        perror("Error creating SDL Renderer.\n");
        exit(EXIT_FAILURE);
    }
}

typedef enum {
    idle, quit, left, right
} pong_event;

static pong_event process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            return quit;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) return quit;
            if (event.key.keysym.sym == SDLK_LEFT) return left;
            if (event.key.keysym.sym == SDLK_a) return left;
            if (event.key.keysym.sym == SDLK_RIGHT) return right;
            if (event.key.keysym.sym == SDLK_d) return right;
    }
    return idle;
}

static void destroy_window(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

typedef struct {
    float x;
    float y;
    uint16_t angle;
} ball;

typedef struct {
    ball ball;
    uint16_t player_x;
    uint64_t last_frame_time;
} game_state;

static game_state init_game_state(void) {
    return (game_state) {
            .ball = (ball) {
                    .x = 20.f,
                    .y = 20.f,
            },
            .player_x = WINDOW_WIDTH / 2,
            .last_frame_time = 0
    };
}

static ball update_ball(const ball old_ball, long double delta_time) {
    return (ball) {
            .x = old_ball.x > WINDOW_WIDTH
                 ? 0.f
                 : (float) (old_ball.x + (BALL_SPEED * delta_time)),
            .y = old_ball.y > WINDOW_HEIGHT
                 ? 0.f
                 : (float) (old_ball.y + (BALL_SPEED * delta_time)),
    };
}

static uint16_t move_player_left(uint16_t old_player_x, long double delta_time) {
    return  (uint16_t) (old_player_x - (PLAYER_SPEED * delta_time));
}

static uint16_t move_player_right(uint16_t old_player_x, long double delta_time) {
    return  (uint16_t) (old_player_x + (PLAYER_SPEED * delta_time));
}

static game_state update_state(const game_state old_state, pong_event event) {
    const uint64_t new_frame_time = SDL_GetTicks64();

    const unsigned time_to_wait =
            FRAME_TARGET_TIME - (new_frame_time - old_state.last_frame_time);
    if (time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    const long double delta_time = (new_frame_time - old_state.last_frame_time) / 1000.0L;
    game_state new_game_state = {
            .ball = update_ball(old_state.ball, delta_time),
            .last_frame_time = new_frame_time
    };

    switch (event) {
        case left:
            new_game_state.player_x = move_player_left(old_state.player_x, delta_time);
            break;
        case right:
            new_game_state.player_x = move_player_right(old_state.player_x, delta_time);
            break;
        case idle:
        case quit:
            new_game_state.player_x = old_state.player_x;
    }

    return new_game_state;
}


static void render_playfield(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    const SDL_Rect playfield = {
            15,
            15,
            WINDOW_WIDTH - 30,
            WINDOW_HEIGHT - 30
    };

    SDL_RenderFillRect(renderer, &playfield);
}

static void render(SDL_Renderer* renderer, game_state state) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    render_playfield(renderer);

    const SDL_Rect ball_rect = {
            (int) (state).ball.x,
            (int) (state).ball.y,
            BALL_WIDTH,
            BALL_HEIGHT
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    SDL_RenderPresent(renderer);
}

void init_pong(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    initialize_window(&window, &renderer);

    game_state state = init_game_state();

    while (true) {
        const pong_event event = process_input();
        if (event == quit) break;

        state = update_state(state, event);
        render(renderer, state);
    }

    destroy_window(window, renderer);
}
