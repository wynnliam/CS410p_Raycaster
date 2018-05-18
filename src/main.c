// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include <emscripten.h>
#include <stdio.h>

#include "loop_handling.h"

// What we pass to the loop function
struct context {
	SDL_Renderer* renderer;
};

// The main loop function.
void loop(void* args) {
	struct context* ctx = (struct context*)args;
	SDL_Renderer* renderer = ctx->renderer;

	// Handle all non-rendering logic before we update.
	// Things like keyboard input and user movement.
	update();
	// Do a ray-casting rendering step.
	render(renderer);

	//emscripten_cancel_main_loop();
}

int main() {
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

	/*SDL_Surface* wall1 = SDL_LoadBMP("./src/assests/wall1.bmp");
	if(wall1) {
		printf("Ye wall!\n");
		SDL_FreeSurface(wall1);
	}

	else {
		printf("No wall!\n");
		printf("ERROR: %s\n", SDL_GetError());
	}*/

	// Initialize any non-SDL logic
	initialize();

	// Actually sets the rendering loop.
	emscripten_set_main_loop_arg(loop, &ctx, fps_count, run_infinite);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
