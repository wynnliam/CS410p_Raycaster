// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"

#include "params.h"
#include "map.h"

#include <stdio.h>
#include <math.h>

/*INITIALIZATION PROCEDURES*/

void initialize() {
}

/*UPDATE PROCEDURES*/

void update() {
}

/*RENDERING PROCEDURES*/
void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
