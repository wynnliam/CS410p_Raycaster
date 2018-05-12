// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"

#include "params.h"
#include "map.h"

#include <stdio.h>
#include <math.h>

// The player's position
int player_x = 768;
int player_y = 192;
// Player rotation in degrees.
float player_rot = 225.0f;

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
}

/*RENDERING PROCEDURES*/

int isWall(int x, int y) {
	// Gets the location in unit coordinates
	int x_unit = x / UNIT_SIZE;
	int y_unit = y / UNIT_SIZE;

	if(y_unit < 0 || y_unit >= MAP_H || x_unit < 0 || x_unit >= MAP_W)
		return -1;

	else
		return map[y_unit * MAP_W + x_unit] > 0;
}

int getWall(int x, int y) {
	if(!isWall(x, y))
		return -1;

	int x_unit = x / UNIT_SIZE;
	int y_unit = y / UNIT_SIZE;

	return map[y_unit * MAP_W + x_unit];
}

void get_horizontal_intersection(float ray_angle, int intersect_point[2]) {
	// Used when getting the tangent
	float angle_rads = (float)M_PI * ray_angle / 180.0f;
	float sin_angle = (float)sin(angle_rads);
	// Use these values to have our ray trace.
	int curr_x, curr_y;
	int next_x, next_y;
	int delta_x;
	int ret_val;

	curr_x = player_x;
	curr_y = player_y;

	if(sin_angle > 0)
		next_y = (int)floor((float)player_y / (float)UNIT_SIZE) * UNIT_SIZE - 1;
	else
		next_y = (int)floor((float)player_y / (float)UNIT_SIZE) * UNIT_SIZE + UNIT_SIZE;

	if(ray_angle == 90.0f || ray_angle == 270.0f) {
		next_x = player_x;
		delta_x = 0;
	}
	
	else {
		next_x = (int)((player_y - next_y) / (float)tan(angle_rads) + player_x);
		delta_x = (int)(UNIT_SIZE / tan(angle_rads));
	}

	while(!(ret_val = isWall(next_x, next_y))) {
		curr_x = next_x;
		curr_y = next_y;

		next_x += delta_x;
		next_y += UNIT_SIZE;
	}

	if(ret_val == -1) {
		intersect_point[0] = -1;
		intersect_point[1] = -1;
	}

	else {
		intersect_point[0] = next_x;
		intersect_point[1] = next_y;
	}
}

void get_vertical_intersection(float ray_angle, int intersect_point[2]) {
	// Used when getting the tangent
	float angle_rads = (float)M_PI * ray_angle / 180.0f;
	float cos_angle = (float)sin(angle_rads);
	// Use these values to have our ray trace.
	int curr_x, curr_y;
	int next_x, next_y;
	int delta_y;
	int ret_val;

	curr_x = player_x;
	curr_y = player_y;

	if(cos_angle > 0)
		next_x = (int)floor((float)player_x / (float)UNIT_SIZE) * UNIT_SIZE - 1;
	else
		next_x = (int)floor((float)player_x / (float)UNIT_SIZE) * UNIT_SIZE + UNIT_SIZE;

	next_y = (int)(player_y + tan(angle_rads) * (player_x - next_x));
	delta_y = (int)(UNIT_SIZE * tan(angle_rads));

	while(!(ret_val = isWall(next_x, next_y))) {
		curr_x = next_x;
		curr_y = next_y;

		next_x += UNIT_SIZE;
		next_y += delta_y;
	}

	if(ret_val == -1) {
		intersect_point[0] = -1;
		intersect_point[1] = -1;
	}

	else {
		intersect_point[0] = next_x;
		intersect_point[1] = next_y;
	}
}

int isValid(int x, int y) {
	return x != -1 && y != -1;
}

float get_dist(int x1, int y1, int x2, int y2) {
	int dX = x1 - x2;
	int dY = y1 - y2;

	dX *= dX;
	dY *= dY;

	return (float)sqrt(dX + dY);
}

void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	float curr_ray_angle;
	int h_intersect[2];
	int v_intersect[2];
	int wall_x, wall_y;
	float correct_dist;
	int slice_height;
	char wall;

	int i;
	// There are PROJ_W number of rays - one for each column of pixels
	// in the screen.
	for(i = 0; i < PROJ_W; i++) {
		curr_ray_angle = player_rot + ray_angles[i];

		get_horizontal_intersection(curr_ray_angle, h_intersect);
		get_vertical_intersection(curr_ray_angle, v_intersect);

		if(!isValid(h_intersect[0], h_intersect[1]) && !isValid(v_intersect[0], v_intersect[1]))
			continue;

		else if(!isValid(h_intersect[0], h_intersect[1]) && isValid(v_intersect[0], v_intersect[1])) {
			wall_x = v_intersect[0];
			wall_y = v_intersect[1];
		}

		else if(!isValid(h_intersect[0], h_intersect[1]) && isValid(v_intersect[0], v_intersect[1])) {
			wall_x = h_intersect[0];
			wall_y = h_intersect[1];
		}

		else {
			if(get_dist(h_intersect[0], h_intersect[1], player_x, player_y) <
			   get_dist(v_intersect[0], v_intersect[1], player_x, player_y))
			{
				wall_x = h_intersect[0];
				wall_y = h_intersect[1];
			}

			else {
				wall_x = h_intersect[0];
				wall_y = h_intersect[1];
			}
		}

		correct_dist = get_dist(wall_x, wall_y, player_x, player_y) * (float)cos(curr_ray_angle * M_PI / 180.0);
		slice_height = (UNIT_SIZE / correct_dist) * DIST_TO_PROJ;
		wall = getWall(wall_x, wall_y);

		SDL_SetRenderDrawColor(renderer, textures[wall][0], textures[wall][1], textures[wall][2], 255);
		SDL_RenderDrawLine(renderer, i, 100 - slice_height / 2, i, 100 + slice_height / 2);
	}

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
