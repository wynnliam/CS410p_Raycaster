// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"
#include "raycaster.h"

// Stores the player 
int player_x, player_y;
// The player rotation.
int player_rot;

/*INITIALIZATION PROCEDURES*/

void initialize() {
	player_x = 640;
	player_y = 320;
	player_rot = 30;
	// Initializes all the angle lookup tables.
	initialize_lookup_tables();
}

/*UPDATE PROCEDURES*/

void update() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			if(event.key.keysym.sym == SDLK_a) {
				player_rot -= 1;
			}

			if(event.key.keysym.sym == SDLK_d) {
				player_rot += 1;
			}
		}
	}

	if(player_rot < 0)
		player_rot += 360;
	if(player_rot > 360)
		player_rot -= 360;
}

/*RENDERING PROCEDURES*/
void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	cast_rays(renderer, player_x, player_y, player_rot);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
