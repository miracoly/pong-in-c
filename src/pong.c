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

#define PLAYER_WIDTH 150
#define PLAYER_HEIGHT 15
#define PLAYER_SPEED 750

#define PLAYFIELD_PADDING 15

#define FPS 30
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

typedef struct {
    bool quit;
    bool left;
    bool right;
} pong_input;

static pong_input process_input(pong_input input) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            input.quit = true;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) input.quit = true;
            if (event.key.keysym.sym == SDLK_LEFT) input.left = true;
            if (event.key.keysym.sym == SDLK_a) input.left = true;
            if (event.key.keysym.sym == SDLK_RIGHT) input.right = true;
            if (event.key.keysym.sym == SDLK_d) input.right = true;
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_LEFT) input.left = false;
            if (event.key.keysym.sym == SDLK_a) input.left = false;
            if (event.key.keysym.sym == SDLK_RIGHT) input.right = false;
            if (event.key.keysym.sym == SDLK_d) input.right = false;
    }
    return input;
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
} pong_state;

static pong_state init_game_state(void) {
    return (pong_state) {
            .ball = (ball) {
                    .x = 20.f,
                    .y = 20.f,
            },
            .player_x = WINDOW_WIDTH / 2,
            .last_frame_time = 0
    };
}

static ball update_ball(const ball* old_ball, long double delta_time) {
    return (ball) {
            .x = old_ball->x > WINDOW_WIDTH
                 ? 0.f
                 : (float) (old_ball->x + (BALL_SPEED * delta_time)),
            .y = old_ball->y > WINDOW_HEIGHT
                 ? 0.f
                 : (float) (old_ball->y + (BALL_SPEED * delta_time)),
    };
}

static uint16_t move_player_left(uint16_t old_player_x, long double delta_time) {
    return (uint16_t) (old_player_x - (PLAYER_SPEED * delta_time));
}

static uint16_t move_player_right(uint16_t old_player_x, long double delta_time) {
    return (uint16_t) (old_player_x + (PLAYER_SPEED * delta_time));
}

static uint16_t update_player(uint16_t old_player_x, pong_input input, long double delta_time) {
    uint16_t new_player_x = old_player_x;
    if (input.left) {
        uint16_t new_x = move_player_left(new_player_x, delta_time);
        new_player_x = (new_x < (2 * PLAYFIELD_PADDING) || new_x > WINDOW_WIDTH)
                ? (2 * PLAYFIELD_PADDING)
                : new_x;
    }
    if (input.right) {
        uint16_t new_x = move_player_right(new_player_x, delta_time);
         new_player_x = (new_x > WINDOW_WIDTH - (2 * PLAYFIELD_PADDING) - PLAYER_WIDTH)
                ? WINDOW_WIDTH - (2 * PLAYFIELD_PADDING) - PLAYER_WIDTH
                : new_x;
    }
    return new_player_x;
}

static pong_state update_state(const pong_state* old_state, pong_input input) {
    const uint64_t new_frame_time = SDL_GetTicks64();

    const unsigned time_to_wait =
            FRAME_TARGET_TIME - (new_frame_time - old_state->last_frame_time);
    if (time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    const long double delta_time = (new_frame_time - old_state->last_frame_time) / 1000.0L;
    pong_state new_game_state = {
            .ball = update_ball(&old_state->ball, delta_time),
            .last_frame_time = new_frame_time
    };

    new_game_state.player_x = update_player(old_state->player_x, input, delta_time);

    return new_game_state;
}

static void render_playfield(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    const SDL_Rect playfield = {
            PLAYFIELD_PADDING,
            PLAYFIELD_PADDING,
            WINDOW_WIDTH - 30,
            WINDOW_HEIGHT - 30
    };

    SDL_RenderFillRect(renderer, &playfield);
}

static void render_ball(SDL_Renderer* renderer, const ball* ball) {
    const SDL_Rect ball_rect = {
            (int) ball->x,
            (int) ball->y,
            BALL_WIDTH,
            BALL_HEIGHT
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);
}


static void render_player(SDL_Renderer* renderer, uint16_t player_x) {
    const SDL_Rect player = {
            player_x,
            WINDOW_HEIGHT - PLAYER_HEIGHT - (2 * PLAYFIELD_PADDING),
            PLAYER_WIDTH,
            PLAYER_HEIGHT
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &player);
}

static void render(SDL_Renderer* renderer, const pong_state* state) {
    render_playfield(renderer);
    render_ball(renderer, &(state->ball));
    render_player(renderer, state->player_x);

    SDL_RenderPresent(renderer);
}

void init_pong(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    initialize_window(&window, &renderer);

    pong_state state = init_game_state();
    pong_input input = {
            .left = false,
            .right = false,
            .quit = false
    };

    while (!input.quit) {
        input = process_input(input);
        if (input.quit) break;

        state = update_state(&state, input);
        render(renderer, &state);
    }

    destroy_window(window, renderer);
}
