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
		curr_angle = (float)(deg * M_PI / 180.0f);
		curr_sin = (float)round(sin(curr_angle) * 128);
		curr_cos = (float)round(cos(curr_angle) * 128);

		// In the raycaster, these values cause problems since you get NaN for
		// some computations.
		if(deg == 0 || deg == 90 || deg == 180 || deg == 270 || deg == 360)
			curr_tan = -1;
		else
			curr_tan = (float)round(tan(curr_angle) * 128);

		sin_table[deg] = curr_sin / 128.0f;
		cos_table[deg] = curr_cos / 128.0f;
		tan_table[deg] = curr_tan / 128.0f;

		sin128table[deg] = (int)curr_sin;
		cos128table[deg] = (int)curr_cos;
		tan128table[deg] = (int)curr_tan;

		//printf("deg %d. Sin: %d, Cos: %d, tan: %d\n", deg, sin128table[deg], cos128table[deg], tan128table[deg]);
	}
}

// TODO: Add documentation for this
int get_tile(int x, int y) {
	int grid_x = x >> UNIT_POWER;
	int grid_y = y >> UNIT_POWER;

	if(grid_x < 0 || grid_x > MAP_W - 1)
		return -1;
	if(grid_y < 0 || grid_y > MAP_H - 1)
		return -1;

	return map[grid_y * MAP_W + grid_x];
}

int get_dist_sqrd(int x1, int y1, int x2, int y2) {
	int d_x, d_y;

	d_x = x1 - x2;
	d_y = y1 - y2;

	d_x *= d_x;
	d_y *= d_y;

	return d_x + d_y;
}

void get_ray_hit(int ray_angle, int player_x, int player_y, struct hitinfo* hit) {
	// Stores the position of the ray as it moved
	// from one grid line to the next.
	int curr_h_x, curr_h_y;
	int curr_v_x, curr_v_y;
	// How much we move from curr_x and curr_y.
	int delt_h_x, delt_h_y;
	int delt_v_x, delt_v_y;

	int hit_h[2];
	int hit_v[2];
	int h_dist;
	int v_dist;
	int tile;

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
		hit->quadrant = 1;
		// Divide player_y by 64, floor that, multiply by 64, and then subtract 1.
		curr_h_y = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
		// Multiply player_y and curr_h_y by 128, then divide by the tan * 128. This will
		// undo the the 128 multiplication without having a divide by 0 (For example, tan128table[1]).
		curr_h_x = (((player_y - curr_h_y) << 7) / tan128table[ray_angle]) + player_x;

		// Divide player_x by 64, floor the result, multiply by 64 and add 64.
		curr_v_x = ((player_x >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
		// Get the tan(curr_v_x - player_x) and subtract that from player_y.
		curr_v_y = player_y - (((curr_v_x - player_x) * tan128table[ray_angle]) >> 7);

		// 64 / tan(ray_angle). We must account for the 128.
		delt_h_x = (1 << 13) / tan128table[ray_angle];
		delt_h_y = -UNIT_SIZE;

		delt_v_x = UNIT_SIZE;
		// Compute -tan(angle) * 64
		delt_v_y = -((tan128table[ray_angle] << UNIT_POWER) >> 7);
	}

	// The ray is in quadrant 2.
	else if(91 <= ray_angle && ray_angle <= 179) {
		hit->quadrant = 2;

		// Adjusts the angle so its between 1 and 89.
		ray_angle = ray_angle - 90;

		// Compute floor(player_y / 64) * 64 - 1.
		curr_h_y = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
		// Computes player_x - (player_y * curr_h_y) * tan(ray_angle).
		curr_h_x = player_x - ((tan128table[ray_angle] * (player_y - curr_h_y)) >> 7);

		// Compute floor(player_x / 64) * 64 - 1.
		curr_v_x = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
		// Compute player_y - (player_x - curr_v_x) / tan(ray_angle).
		curr_v_y = player_y - ((player_x - curr_v_x) << 7) / tan128table[ray_angle];

		// -64, since we are travelling in the negative y direction.
		delt_h_y = -UNIT_SIZE;
		// -64 * tan(ray_angle), since we are travelling in the negative x direction.
		delt_h_x = -((UNIT_SIZE * tan128table[ray_angle]) >> 7);

		
		delt_v_x = -UNIT_SIZE;
		delt_v_y = -((1 << 13) / tan128table[ray_angle]);
	}

	// The ray is in quadrant 3.
	else if(181 <= ray_angle && ray_angle <= 269) {
		hit->quadrant = 3;

		// Adjusts the angle so its between 1 and 89.
		ray_angle = ray_angle - 180;

		// Computes floor(player_y / 64) * 64 + 64.
		curr_h_y = ((player_y >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
		// Computes player_x - (curr_h_y - player_y / tan(ray_angle).
		curr_h_x = player_x - (((curr_h_y - player_y) << 7) / tan128table[ray_angle]);

		// Computes floor(player x / 64) * 64 - 1.
		curr_v_x = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
		// Computes tan(ray_angle) * (player_x - curr_v_x) + player_y.
		curr_v_y = ((tan128table[ray_angle] * (player_x  - curr_v_x)) >> 7) + player_y;

		delt_h_y = UNIT_SIZE;
		delt_h_x = -(1 << 13) / tan128table[ray_angle];

		delt_v_x = -UNIT_SIZE;
		delt_v_y = (UNIT_SIZE * tan128table[ray_angle]) >> 7;
	}

	// The ray is in quadrant 4 (271 <= ray_angle && ray_angle <= 359)
	/*else if(271 <= ray_angle && ray_angle <= 359) {
	}*/

	else {
		hit->hit_pos[0] = -1;
		hit->hit_pos[1] = -1;
		return;
	}

	//printf("ray angle: %d | ", ray_angle);
	//printf("ch: %d %d | ", curr_h_x, curr_h_y);
	//printf("dh: %d %d | ", delt_h_x, delt_h_y);
	//printf("cv: %d %d | ", curr_v_x, curr_v_y);
	//printf("dv: %d %d\n", delt_v_x, delt_v_y);

	// Now find the point that is a wall by travelling along horizontal gridlines.
	tile = get_tile(curr_h_x, curr_h_y);
	while(tile == 0) {
		curr_h_x += delt_h_x;
		curr_h_y += delt_h_y;
		tile = get_tile(curr_h_x, curr_h_y);
	}

	// We went outside the bounds of the map.
	if(tile == -1) {
		hit_h[0] = -1;
		hit_h[1] = -1;
	}

	else {
		hit_h[0] = curr_h_x;
		hit_h[1] = curr_h_y;
	}

	// Now find the point that is a wall by travelling along vertical gridlines.
	tile = get_tile(curr_v_x, curr_v_y);
	while(tile == 0) {
		curr_v_x += delt_v_x;
		curr_v_y += delt_v_y;
		tile = get_tile(curr_v_x, curr_v_y);
	}

	// We went outside the bounds of the map.
	if(tile == -1) {
		hit_v[0] = -1;
		hit_v[1] = -1;
	}

	else {
		hit_v[0] = curr_v_x;
		hit_v[1] = curr_v_y;
	}

	//printf("hit h: {%d, %d}\n", hit_h[0], hit_h[1]);
	//printf("hit v: {%d, %d}\n", hit_v[0], hit_v[1]);

	// Now choose either the horizontal or vertical intersection
	// point. Or choose -1, -1 to denote an error.
	if(hit_h[0] == -1 && hit_h[1] == -1 && hit_v[0] == -1 && hit_v[1] == -1) {
		hit->hit_pos[0] = -1;
		hit->hit_pos[1] = -1;
	}

	else if(hit_h[0] == -1 && hit_h[1] == -1) {
		hit->hit_pos[0] = hit_v[0];
		hit->hit_pos[1] = hit_v[1];
		hit->is_horiz = 0;
	}

	else if(hit_v[0] == -1 && hit_v[1] == -1) {
		hit->hit_pos[0] = hit_h[0];
		hit->hit_pos[1] = hit_h[1];
		hit->is_horiz = 1;
	}

	else {
		h_dist = get_dist_sqrd(hit_h[0], hit_h[1], player_x, player_y);
		v_dist = get_dist_sqrd(hit_v[0], hit_v[1], player_x, player_y);

		if(h_dist < v_dist) {
			hit->hit_pos[0] = hit_h[0];
			hit->hit_pos[1] = hit_h[1];

			hit->is_horiz = 1;
		}

		else {
			hit->hit_pos[0] = hit_v[0];
			hit->hit_pos[1] = hit_v[1];

			hit->is_horiz = 0;
		}
	}

	//printf("Final hit: {%d, %d}\n", hit->hit_pos[0], hit->hit_pos[1]);

	hit->dist = get_dist_sqrd(hit->hit_pos[0], hit->hit_pos[1], player_x, player_y);
	hit->wall_type = get_tile(hit->hit_pos[0], hit->hit_pos[1]);
}

void cast_rays(SDL_Renderer* renderer, int player_x, int player_y, int player_rot) {
	// Stores the precise angle of our current ray.
	float curr_angle = (float)player_rot + FOV_HALF;
	// The curr_angle adjusted to be within 0 and 360.
	float adj_angle;
	int correct_angle;
	int slice_dist;
	int slice_height;
	int wall;

	struct hitinfo hit;

	int i;

	////printf("player_rot: %d\n", player_rot);

	for(i = 0; i < PROJ_W; ++i) {
		adj_angle = curr_angle;

		if(adj_angle < 0)
			adj_angle += 360;
		if(adj_angle > 360)
			adj_angle -= 360;

		correct_angle = (int)adj_angle - player_rot;
		if(correct_angle < 0)
			correct_angle += 360;
		if(correct_angle > 360)
			correct_angle -= 360;

		get_ray_hit((int)adj_angle, player_x, player_y, &hit);

		if(hit.hit_pos[0] != -1 && hit.hit_pos[1] != -1) {
			wall = hit.wall_type;
			//slice_dist = (int)sqrt(hit.dist) + 1;

			//slice_dist *= cos128table[correct_angle];
			//slice_dist = slice_dist >> 7;
			slice_dist = (int)(sqrt(hit.dist) * cos_table[correct_angle]);

			//printf("curr angle %f ", curr_angle);
			//printf("adj angle: %d ", (int)adj_angle);
			//printf("dist: %d ", slice_dist);
			//printf("wall: %d ", wall);
			//printf("\n");

			slice_height = (int)(64.0f / slice_dist * DIST_TO_PROJ);

			SDL_SetRenderDrawColor(renderer, textures[wall][0], textures[wall][1], textures[wall][2], 255);
			SDL_RenderDrawLine(renderer, i, 100 - (slice_height >> 1), i, 100 + (slice_height >> 1));
		}

		curr_angle -= ANGLE_BETWEEN_RAYS;
	}
}
