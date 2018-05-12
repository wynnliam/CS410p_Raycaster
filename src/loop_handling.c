// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"

void initialize() {
}

void update() {
}

void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color.
	SDL_RenderClear(renderer);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
