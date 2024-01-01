#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define FPS 30
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
    int x;
    int y;
    int width;
    int height;
};

static void setup(ball* b) {
    *b = (ball) {
            .x = 20,
            .y = 20,
            .width = 15,
            .height = 15,
    };
}

static uint32_t update(uint32_t  last_frame_time, ball* b) {
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME));

    (*b).x = (*b).x > WINDOW_WIDTH
             ? 0
             : (*b).x + 10;
    (*b).y = (*b).y > WINDOW_HEIGHT
             ? 0
             : (*b).y + 10;
    return SDL_GetTicks();
}

static void render(SDL_Renderer* renderer, ball* b) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const SDL_Rect ball_rect = {
            (*b).x,
            (*b).y,
            (*b).width,
            (*b).height
    };

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

    SDL_RenderPresent(renderer);
}

int main(void) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    uint32_t last_frame_time = 0;
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
