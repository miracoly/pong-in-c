#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define BALL_WIDTH 15
#define BALL_HEIGHT 15
#define BALL_SPEED 150

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

static bool process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            return false;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) return false;
            break;
    }
    return true;
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

static game_state update(const game_state old_state) {
    const unsigned time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks64() - old_state.last_frame_time);
    if (time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    const long double delta_time = (SDL_GetTicks64() - old_state.last_frame_time) / 1000.0L;

    return (game_state) {
            .ball.x = old_state.ball.x > WINDOW_WIDTH
                      ? 0.f
                      : (float) (old_state.ball.x + (BALL_SPEED * delta_time)),
            .ball.y = old_state.ball.y > WINDOW_HEIGHT
                      ? 0.f
                      : (float) (old_state.ball.y + (BALL_SPEED * delta_time)),
            .player_x = 0,
            .last_frame_time = SDL_GetTicks64()

    };
}

static void render(SDL_Renderer* renderer, game_state* state) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const SDL_Rect ball_rect = {
            (int) (*state).ball.x,
            (int) (*state).ball.y,
            BALL_WIDTH,
            BALL_HEIGHT
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    SDL_RenderPresent(renderer);
}

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    initialize_window(&window, &renderer);
    game_state state = init_game_state();

    bool is_game_running = true;

    while (is_game_running) {
        is_game_running = process_input();
        state = update(state);
        render(renderer, &state);
    }

    destroy_window(window, renderer);

    return EXIT_SUCCESS;
}
