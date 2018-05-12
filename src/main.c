// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include <SDL2/SDL.h>
#include <emscripten.h>
#include <stdio.h>

// What we pass to the loop function
struct context {
	SDL_Renderer* renderer;
};

// The main loop function.
void loop(void* args) {
	struct context* ctx = (struct context*)args;
	SDL_Renderer* renderer = ctx->renderer;

	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	SDL_RenderClear(renderer);

	SDL_RenderPresent(renderer);
}

int main() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_CreateWindowAndRenderer(320, 200, 0, &window, &renderer);

	struct context ctx;
	ctx.renderer = renderer;

	emscripten_set_main_loop_arg(loop, &ctx, -1, 1);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
