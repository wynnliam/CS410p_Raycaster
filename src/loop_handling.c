// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"

#include "params.h"
#include "map.h"

#include <stdio.h>
#include <math.h>

// The player's position
int player_x = 640;
int player_y = 320;
// Player rotation in degrees.
float player_rot = 3.14f;

/*INITIALIZATION PROCEDURES*/

void initialize() {
}

/*UPDATE PROCEDURES*/

void update() {
}

int is_wall(int x, int y) {
	int unit_x = x / UNIT_SIZE;	
	int unit_y = y / UNIT_SIZE;

	if(unit_x < 0) unit_x = 0;
	if(unit_x >= MAP_W) unit_x = MAP_W - 1;
	if(unit_y < 0) unit_y = 0;
	if(unit_y >= MAP_H) unit_y = MAP_H - 1;

	return map[unit_y * MAP_W + unit_x] > 0;
}

void trace(float angle, int hit[2]) {
}

/*RENDERING PROCEDURES*/
void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	int i;
	for(i = 0; i < PROJ_W; ++i) {
	}

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
