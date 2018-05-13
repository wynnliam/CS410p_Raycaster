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
float player_rot = 180.0f;

// Computes all of the ray angles as if the player had a rotation
// of 0.
void compute_ray_angles();

// When a ray is being traced, it intersects units by both horiontal and
// vertical boundaries. We find the closest of these that hit a wall, and
// render our ray accordingly.
void get_horizontal_intersection(float ray_angle, int intersect_point[2]);
void get_vertical_intersection(float ray_angle, int intersect_point[2]);
// Tells us if a given position is a wall. Used by raycasting to know when
// to stop tracing
int isWall(int x, int y);
int getWall(int x, int y);
// Tells us if a given position is valid (not -1, -1)
int isValid(int x, int y);
// Returns the distance between two points.
float get_dist(int x1, int y1, int x2, int y2);

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
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			if(event.key.keysym.sym == SDLK_a) {
				player_rot -= 1;
			}

			if(event.key.keysym.sym == SDLK_d) {
				player_rot += 1;
			}

			if(player_rot < 0.0f)
				player_rot += 360.0f;
			if(player_rot > 360.0f)
				player_rot -= 360.0f;

			printf("Player rotation: %f\n", player_rot);
		}
	}
}

/*RENDERING PROCEDURES*/

int is_wall(int x, int y) {
	int grid_x = x / UNIT_SIZE;
	int grid_y = y / UNIT_SIZE;

	if(grid_x < 0) grid_x = 0;
	if(grid_x > MAP_W - 1) grid_x = MAP_W - 1;
	if(grid_y < 0) grid_y = 0;
	if(grid_y > MAP_H - 1) grid_y = MAP_H - 1;

	return map[grid_y * MAP_W + grid_x] > 0;
}

int get_map_unit(int x, int y) {
	int grid_x = x / UNIT_SIZE;
	int grid_y = y / UNIT_SIZE;

	if(grid_x < 0) grid_x = 0;
	if(grid_x > MAP_W - 1) grid_x = MAP_W - 1;
	if(grid_y < 0) grid_y = 0;
	if(grid_y > MAP_H - 1) grid_y = MAP_H - 1;

	return map[grid_y * MAP_W + grid_x];
}

void cast_ray(float ray_angle, int hit[2]) {
	int next_x, next_y;
	int delta_x, delta_y;
	float ray_rad = (float)(ray_angle * M_PI / 180.0);

	next_x = player_x;
	next_y = player_y;
	delta_x = (int)(UNIT_SIZE * cos(ray_rad));
	delta_y = (int)(UNIT_SIZE * sin(ray_rad));

	while(!is_wall(next_x, next_y)) {
		next_x += delta_x;
		next_y += delta_y;
	}

	hit[0] = next_x;
	hit[1] = next_y;
}

float get_dist(int x1, int y1, int x2, int y2) {
	int d_x, d_y;

	d_x = x1 - x2;
	d_y = y1 - y2;

	d_x *= d_x;
	d_y *= d_y;

	return (float)sqrt(d_x + d_y);
}

void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	float curr_angle = player_rot - FOV_HALF;
	int hit[2];
	float dist, correct_dist;
	int proj_height;
	int wall;

	int i;
	for(i = 0; i < PROJ_W; ++i) {
		if(curr_angle < 0)
			curr_angle += 360.0f;
		if(curr_angle > 360.0f)
			curr_angle -= 360.0f;

		// TODO: Rewrite this so it's more effecient and accurate.
		cast_ray(curr_angle, hit);
		wall = get_map_unit(hit[0], hit[1]);

		hit[0] = hit[0] / UNIT_SIZE * UNIT_SIZE;
		hit[1] = hit[1] / UNIT_SIZE * UNIT_SIZE;

		dist = get_dist(hit[0], hit[1], player_x, player_y);
		//correct_dist = dist;
		correct_dist = dist * (float)cos((curr_angle - player_rot) * M_PI / 180);

		proj_height = (int)(UNIT_SIZE / correct_dist * DIST_TO_PROJ);

		SDL_SetRenderDrawColor(renderer, textures[wall][0], textures[wall][1], textures[wall][2], 255);
		SDL_RenderDrawLine(renderer, i, 100 - (proj_height / 2), i, 100 + (proj_height / 2));

		curr_angle += ANGLE_BETWEEN_RAYS;
	}

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
