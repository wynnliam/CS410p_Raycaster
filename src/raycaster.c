// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

#include "raycaster.h"
#include "map.h"

#include <stdio.h>
#include <math.h>

/*
	We multiply each value by 128 because 1) dividing the values by
	128 is easy (val >> 7), and 2) Because we can encode more precision
	in them. Consider the following:

	sin(30) = 0.5. Now, say we need 234 * sin(30). This gives us 234 * 0.5,
	which is 117. Now say we store ONLY sin(30) as a char. This would be 0.
	So 234 * 0 = 0. That's WAAAY off.

	Now consider, we do sin(30) * 128. This gives us 64. Now say we do 234 *
	(sin(30) * 128). This gives us 234 * 64 = 14976. Now divide that by 128,
	and we get 117.

	As I said, we can encode to some extent precision in our numbers.
*/
void initialize_lookup_tables() {
	// Stores the angle in radians.
	float curr_angle;
	// Use these to make this procedure more readable.
	float curr_sin, curr_cos, curr_tan;

	int deg;
	for(deg = 0; deg <= 360; ++deg) {
		curr_angle = (float)(deg * M_PI / 180.0);
		curr_sin = (float)sin(curr_angle) * 128.0f;
		curr_cos = (float)cos(curr_angle) * 128.0f;

		// In the raycaster, these values cause problems since you get NaN for
		// some computations.
		if(deg == 0 || deg == 90 || deg == 180 || deg == 270 || deg == 360)
			curr_tan = -1;
		else
			curr_tan = (float)tan(curr_angle) * 128.0f;

		sin128table[deg] = (int)curr_sin;
		cos128table[deg] = (int)curr_cos;
		tan128table[deg] = (int)curr_tan;

		printf("deg %d. Sin: %d, Cos: %d, tan: %d\n", deg, sin128table[deg], cos128table[deg], tan128table[deg]);
	}
}

void get_ray_hit(int ray_angle, int player_x, int player_y, int hit_pos[2]) {
	// Stores the position of the ray as it moved
	// from one grid line to the next.
	int curr_h_x, curr_h_y;
	int curr_v_x, curr_v_y;
	// How much we move from curr_x and curr_y.
	int delt_h_x, delt_h_y;
	int delt_v_x, delt_v_y;

	// First, we must deal with bad angles. These angles will break the raycaster, since
	// it will produce NaN's.
	if(ray_angle == 360)
		ray_angle = 0;
	if(ray_angle == 0 || ray_angle == 90 || ray_angle == 180 || ray_angle == 270)
		ray_angle += 1;

	// Next, we choose our curr and delta vectors according to the quadrant
	// our angle is in.

	// The ray is in quadrant 1.
	if(1 <= ray_angle && ray_angle <= 89) {
	}

	// The ray is in quadrant 2.
	else if(91 <= ray_angle && ray_angle <= 179) {
	}

	// The ray is in quadrant 3.
	else if(181 <= ray_angle && ray_angle <= 269) {
	}

	// The ray is in quadrant 4 (271 <= ray_angle && ray_angle <= 359)
	else {
	}
}

void cast_rays(SDL_Renderer* renderer, int player_x, int player_y, int player_rot) {
	// Stores the precise angle of our current ray.
	float curr_angle = (float)player_rot - FOV_HALF;
	// The curr_angle adjusted to be within 0 and 360.
	float adj_angle;
	int hit_pos[2];

	int i;
	for(i = 0; i < PROJ_W; ++i) {
		adj_angle = curr_angle;

		if(adj_angle < 0)
			adj_angle += 360;
		if(adj_angle > 360)
			adj_angle -= 360;

		get_ray_hit((int)adj_angle, player_x, player_y, hit_pos);

		// TODO: Draw slice!

		curr_angle += ANGLE_BETWEEN_RAYS;
	}
}
