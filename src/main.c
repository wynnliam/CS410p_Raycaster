// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdio.h>

#include "map.h"

// What we pass to the loop function
struct context {
	SDL_Renderer* renderer;
};

// The main loop function.
void loop(void* args) {
	struct context* ctx = (struct context*)args;
	SDL_Renderer* renderer = ctx->renderer;

	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color.
	SDL_RenderClear(renderer);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}

int main() {

	printf("%d %d\n", map[19], map[20]);

	SDL_Window* window;
	SDL_Renderer* renderer;

	struct context ctx;
	// Using -1 makes the renderer run as fast as possible.
	const int fps_count = -1;
	// If true, runs the loop infinitely.
	const int run_infinite = 1;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	SDL_CreateWindowAndRenderer(320, 200, 0, &window, &renderer);

	ctx.renderer = renderer;

	// Actually sets the rendering loop.
	emscripten_set_main_loop_arg(loop, &ctx, fps_count, run_infinite);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
