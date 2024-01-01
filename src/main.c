#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

static bool initialize_window(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        perror("Error initializing SDL.\n");
        return false;
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
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, 0);
    if (!*renderer) {
        perror("Error creating SDL Renderer.\n");
        return false;
    }

    return true;
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

typedef struct ball ball;
struct ball {
    float x;
    float y;
    uint16_t width;
    uint16_t height;
};

static void setup(ball* b) {
    *b = (ball) {
            .x = 20.f,
            .y = 20.f,
            .width = 15,
            .height = 15,
    };
}

static uint64_t update(uint64_t last_frame_time, ball* b) {
    while (!SDL_TICKS_PASSED(SDL_GetTicks64(), last_frame_time + FRAME_TARGET_TIME));

    const long double delta_time = (SDL_GetTicks64() - last_frame_time) / 1000.0L;

    (*b).x = (*b).x > WINDOW_WIDTH
             ? 0.f
             : (float) ((*b).x + (70 * delta_time));
    (*b).y = (*b).y > WINDOW_HEIGHT
             ? 0.f
             : (float) ((*b).y + (50 * delta_time));

    return SDL_GetTicks64();
}

static void render(SDL_Renderer* renderer, ball* b) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const SDL_Rect ball_rect = {
            (int) (*b).x,
            (int) (*b).y,
            (int) (*b).width,
            (int) (*b).height
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    SDL_RenderPresent(renderer);
}

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    uint64_t last_frame_time = 0;
    bool is_game_running = initialize_window(&window, &renderer);
    ball b = {0};

    setup(&b);

    while (is_game_running) {
        is_game_running = process_input();
        last_frame_time = update(last_frame_time, &b);
        render(renderer, &b);
    }

    destroy_window(window, renderer);

    return EXIT_SUCCESS;
}
