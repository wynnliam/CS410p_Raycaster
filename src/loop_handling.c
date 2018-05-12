// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"

#include "params.h"
#include "map.h"
#include <stdio.h>

// Computes all of the ray angles as if the player had a rotation
// of 0.
void compute_ray_angles();

/*INITIALIZATION PROCEDURES*/

void initialize() {
	compute_ray_angles();

	int i;
	for(i = 0; i < PROJ_W; ++i) {
		printf("%d: %f\n", i, ray_angles[i]);
	}
}

/*
	Computes the angle of each ray as if the player had a rotation
	of 0 degrees. If the field of view is 60, we want the the largest
	angle to be 30, and the smallest to be -30 so that way the rays are
	within the field of view of 60. Since we know the angle between the rays,
	it becomes fairly trivial to compute the ray of angle i, which is:

		FOV_HALF - (ANGLE_BETWEEN_RAYS * (i + 1))

	Note that we can either add 1 to i, or don't. By doing so, the ray at
	PROJ_W - 1 has an angle of -FOV_HALF degrees. Without it, the ray at
	0 will have an angle of FOV_HALF degrees.

	POST_CONDITIONS:
		the ray_angles parameter is computed, and can thus be used in a ray casting
		step.
*/
void compute_ray_angles() {

	int i;
	for(i = 0; i < PROJ_W; ++i) {
		ray_angles[i] = FOV_HALF - (ANGLE_BETWEEN_RAYS * (i + 1));
	}
}

/*UPDATE PROCEDURES*/

void update() {
}

/*RENDERING PROCEDURES*/

void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color.
	SDL_RenderClear(renderer);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
