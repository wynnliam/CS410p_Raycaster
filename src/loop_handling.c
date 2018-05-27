// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"
#include "raycaster.h"

#include <stdio.h>

// Stores the player 
int player_x, player_y;
// The player rotation.
int player_rot;

/*INITIALIZATION PROCEDURES*/

void initialize(SDL_Renderer* renderer) {
	player_x = 396;
	player_y = 259;
	player_rot = 133;

	// Initializes all the angle lookup tables.
	initialize_lookup_tables();
	// Initialize the map data.
	initialize_map(renderer);
}

/*UPDATE PROCEDURES*/

void update() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			if(event.key.keysym.sym == SDLK_a) {
				player_rot += 1;

				if(player_rot < 0)
					player_rot += 360;
				if(player_rot > 360)
					player_rot -= 360;
			}

			if(event.key.keysym.sym == SDLK_d) {
				player_rot -= 1;

				if(player_rot < 0)
					player_rot += 360;
				if(player_rot > 360)
					player_rot -= 360;
			}

			if(event.key.keysym.sym == SDLK_w) {
				player_y -= (sin128table[player_rot] << 2) >> 7;
				player_x += (cos128table[player_rot] << 2) >> 7;

				if(get_tile(player_x, player_y) != 0) {
					player_y += (sin128table[player_rot] << 2) >> 7;
					player_x -= (cos128table[player_rot] << 2) >> 7;
				}
			}

			if(event.key.keysym.sym == SDLK_s) {
				player_y += (sin128table[player_rot] << 2) >> 7;
				player_x -= (cos128table[player_rot] << 2) >> 7;

				if(get_tile(player_x, player_y) != 0) {
					player_y -= (sin128table[player_rot] << 2) >> 7;
					player_x += (cos128table[player_rot] << 2) >> 7;
				}
			}

			if(event.key.keysym.sym == SDLK_c) {
				printf("Player position = [%d, %d]. Player rotation = %d\n", player_x, player_y, player_rot);
			}
		}
	}

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
