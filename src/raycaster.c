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
		if(deg == 0 || deg == 90 || deg == 180 || deg == 270 || deg == 360) {
			curr_tan = -1;

			tan1table[deg] = -1;
			delta_h_x[deg] = 0;
			delta_h_y[deg] = 0;
			delta_v_x[deg] = 0;
			delta_v_y[deg] = 0;
		}

		else {
			curr_tan = (float)round(tan(curr_angle) * 128);

			tan1table[deg] = (int)round(128.0 / tan(curr_angle));
		}

		sin128table[deg] = (int)curr_sin;
		cos128table[deg] = (int)curr_cos;
		tan128table[deg] = (int)curr_tan;

		sin1table[deg] = (int)(round(128.0 / sin(curr_angle)));

		if(1 <= deg && deg <= 89) {
			// 64 / tan(ray_angle). We must account for the 128.
			delta_h_x[deg] = (1 << 13) / tan128table[deg];
			delta_h_y[deg]= -UNIT_SIZE;

			delta_v_x[deg] = UNIT_SIZE;
			// Compute -tan(angle) * 64
			delta_v_y[deg] = -((tan128table[deg] << UNIT_POWER) >> 7);
		}

		else if(91 <= deg && deg <= 179) {
			// -64, since we are travelling in the negative y direction.
			delta_h_y[deg] = -UNIT_SIZE;
			// Computes -64 / tan(deg). Negative since we're
			// travelling in the negative y direction.
			delta_h_x[deg] = (tan128table[deg - 90] * -UNIT_SIZE) >> 7;

			delta_v_x[deg] = -UNIT_SIZE;
			delta_v_y[deg] = -((1 << 13) / tan128table[deg - 90]);
		}

		else if(181 <= deg && deg <= 269) {
			delta_h_y[deg] = UNIT_SIZE;
			delta_h_x[deg]  = -(1 << 13) / tan128table[deg - 180];

			delta_v_x[deg] = -UNIT_SIZE;
			// Computes 64 * tan(ray_angle).
			delta_v_y[deg]  = (UNIT_SIZE * tan128table[deg - 180]) >> 7;
		}

		else {
			delta_h_y[deg] = UNIT_SIZE;
			// Computes 64 * tan(ray_angle)
			delta_h_x[deg] = (UNIT_SIZE * tan128table[deg - 270]) >> 7;

			delta_v_x[deg] = UNIT_SIZE;
			// Computes 64 / tan(ray_angle)
			delta_v_y[deg] = (1 << 13) / tan128table[deg - 270];
		}

		//printf("deg %d. Sin: %d, Cos: %d, tan: %d\n", deg, sin128table[deg], cos128table[deg], tan128table[deg]);
	}
}

void initialize_map(SDL_Renderer* renderer) {
	SDL_Surface* surface;

	// Initialize the map data.
	// "null" walldef.
	walls[0].texture = NULL;
	walls[0].color[0] = 0;
	walls[0].color[1] = 0;
	walls[0].color[2] = 0;
	// first wall.
	surface = SDL_LoadBMP("./src/assests/wall1.bmp");
	walls[1].texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	walls[1].color[0] = 0;
	walls[1].color[1] = 122;
	walls[1].color[2] = 255;
	// second wall.
	surface = SDL_LoadBMP("./src/assests/wall2.bmp");
	walls[2].texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	walls[2].color[0] = 255;
	walls[2].color[1] = 0;
	walls[2].color[2] = 0;

	// Initialize the floor.
	floor_surf = SDL_LoadBMP("./src/assests/floor.bmp");
	// Initialize the ceiling.
	ceiling_surf = SDL_LoadBMP("./src/assests/ceiling.bmp");

	floor_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);

	floor_rect.x = 0;
	floor_rect.y = 100;
	floor_rect.w = 320;
	floor_rect.h = 100;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(floor_tex, SDL_BLENDMODE_BLEND);
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

	// Stores the ray angle adjusted for its quadrant. We use this angle to compute
	// the curr_h and curr_v values.
	int alpha;

	int hit_h[2];
	int hit_v[2];
	int h_dist;
	int v_dist;
	int tile;

	// Next, we choose our curr and delta vectors according to the quadrant
	// our angle is in.

	// The ray is in quadrant 1.
	if(1 <= ray_angle && ray_angle <= 89) {
		hit->quadrant = 1;
		alpha = ray_angle;

		// Divide player_y by 64, floor that, multiply by 64, and then subtract 1.
		curr_h_y = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
		// Multiply player_y and curr_h_y by 128, then divide by the tan * 128. This will
		// undo the the 128 multiplication without having a divide by 0 (For example, tan128table[1]).
		curr_h_x = (((player_y - curr_h_y) * tan1table[alpha]) >> 7) + player_x;

		// Divide player_x by 64, floor the result, multiply by 64 and add 64.
		curr_v_x = ((player_x >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
		// Get the tan(curr_v_x - player_x) and subtract that from player_y.
		curr_v_y = player_y - (((curr_v_x - player_x) * tan128table[alpha]) >> 7);

	}

	// The ray is in quadrant 2.
	else if(91 <= ray_angle && ray_angle <= 179) {
		hit->quadrant = 2;

		// Adjusts the angle so its between 1 and 89.
		alpha = ray_angle - 90;

		// Compute floor(player_y / 64) * 64 - 1.
		curr_h_y = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
		// Computes player_x - (player_y * curr_h_y) * tan(ray_angle).
		curr_h_x = player_x - ((tan128table[alpha] * (player_y - curr_h_y)) >> 7);

		// Compute floor(player_x / 64) * 64 - 1.
		curr_v_x = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
		// Compute player_y - (player_x - curr_v_x) / tan(ray_angle).
		curr_v_y = player_y - (((player_x - curr_v_x) * tan1table[alpha]) >> 7);
	}

	// The ray is in quadrant 3.
	else if(181 <= ray_angle && ray_angle <= 269) {
		hit->quadrant = 3;

		// Adjusts the angle so its between 1 and 89.
		alpha = ray_angle - 180;

		// Computes floor(player_y / 64) * 64 + 64.
		curr_h_y = ((player_y >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
		// Computes player_x - (curr_h_y - player_y / tan(ray_angle).
		curr_h_x = player_x - (((curr_h_y - player_y) * tan1table[alpha]) >> 7);

		// Computes floor(player x / 64) * 64 - 1.
		curr_v_x = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
		// Computes tan(ray_angle) * (player_x - curr_v_x) + player_y.
		curr_v_y = ((tan128table[alpha] * (player_x  - curr_v_x)) >> 7) + player_y;
	}

	// The ray is in quadrant 4 (271 <= ray_angle && ray_angle <= 359)
	else if(271 <= ray_angle && ray_angle <= 359) {
		hit->quadrant = 4;

		// Adjusts the angle so its between 1 and 89.
		alpha = ray_angle - 270;

		// Computes floor(player_y / 64) * 64 + 64
		curr_h_y = ((player_y >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
		// Computes (curr_h_y - player_y) * tan(ray_angle) + player_x
		curr_h_x = (((curr_h_y - player_y) * tan128table[alpha]) >> 7) + player_x;

		// Computes floor(player_x / 64) * 64 + 64
		curr_v_x = ((player_x >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
		// Computes (curr_v_x - player_x) / tan(ray_angle) + player_y.
		curr_v_y = (((curr_v_x - player_x) * tan1table[alpha]) >> 7) + player_y;
	}

	else {
		hit->hit_pos[0] = -1;
		hit->hit_pos[1] = -1;
		return;
	}

	delt_h_x = delta_h_x[ray_angle];
	delt_h_y = delta_h_y[ray_angle];
	delt_v_x = delta_v_x[ray_angle];
	delt_v_y = delta_v_y[ray_angle];

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

		if(hit->quadrant == 1 || hit->quadrant == 3)
			hit->dist = (abs(player_y - hit_v[1]) * sin1table[alpha]) >> 7;
		else
			hit->dist = (abs(player_x - hit_v[0]) * sin1table[alpha]) >> 7;
	}

	else if(hit_v[0] == -1 && hit_v[1] == -1) {
		hit->hit_pos[0] = hit_h[0];
		hit->hit_pos[1] = hit_h[1];
		hit->is_horiz = 1;

		if(hit->quadrant == 1 || hit->quadrant == 3)
			hit->dist = (abs(player_y - hit_h[1]) * sin1table[alpha]) >> 7;
		else
			hit->dist = (abs(player_x - hit_h[0]) * sin1table[alpha]) >> 7;
	}

	else {
		// Compute dist = abs(player_x - hit_x) / cos(alpha).
		if(hit->quadrant == 1 || hit->quadrant == 3) {
			h_dist = (abs(player_y - hit_h[1]) * sin1table[alpha]) >> 7;
			v_dist = (abs(player_y - hit_v[1]) * sin1table[alpha]) >> 7;
		}

		// Compute dist = abs(player_y - hit_y) / cos(alpha).
		else {
			h_dist = (abs(player_x - hit_h[0]) * sin1table[alpha]) >> 7;
			v_dist = (abs(player_x - hit_v[0]) * sin1table[alpha]) >> 7;
		}

		if(h_dist < v_dist) {
			hit->hit_pos[0] = hit_h[0];
			hit->hit_pos[1] = hit_h[1];
			hit->dist = h_dist;
			hit->is_horiz = 1;
		}

		else {
			hit->hit_pos[0] = hit_v[0];
			hit->hit_pos[1] = hit_v[1];
			hit->dist = v_dist;
			hit->is_horiz = 0;
		}
	}

	//printf("Final hit: {%d, %d}\n", hit->hit_pos[0], hit->hit_pos[1]);
	//printf("sin128table[%d] = %d\n", alpha, sin128table[alpha]);

	hit->wall_type = get_tile(hit->hit_pos[0], hit->hit_pos[1]);
}

void cast_rays(SDL_Renderer* renderer, int player_x, int player_y, int player_rot) {
	// Stores the precise angle of our current ray.
	float curr_angle = (float)(player_rot + FOV_HALF);
	// The curr_angle adjusted to be within 0 and 360.
	int adj_angle;
	// The angle used to compute the "corrected" distance so
	// we avoid the fisheye effect.
	int correct_angle;
	// Distance from player to the wall, adjusted by the correct_angle.
	int slice_dist;
	// Height of the line to render.
	int slice_height;
	// The type of wall we hit.
	int wall;

	// Returns info about the hit.
	struct hitinfo hit;

	// Used for rendering textures
	SDL_Rect src, dest;

	// Used for rendering floors/ceilings.
	// Straight distance from the player to the floor/ceiling point
	// to render.
	int straight_dist;
	// The actual distance to the floor/ceiling point.
	int dist_to_point;
	// The floor/ceiling point in "global space"
	int p_x, p_y;
	// The texture point.
	int t_x, t_y;
	// RGB value of texture.
	unsigned char* t_color;

	int i, j;

	// Begin by clearning the floor/ceiling texture.
	for(i = 0; i < 64000; ++i)
		floor_pixels[i] = 0;

	for(i = 0; i < PROJ_W; ++i) {
		adj_angle = (int)curr_angle;

		if(adj_angle < 0)
			adj_angle += 360;
		if(adj_angle > 360)
			adj_angle -= 360;

		// First, we must deal with bad angles. These angles will break the raycaster, since
		// it will produce NaN's.
		if(adj_angle == 360)
			adj_angle = 0;
		if(adj_angle == 0 || adj_angle == 90 || adj_angle == 180 || adj_angle == 270)
			adj_angle += 1;

		correct_angle = abs(adj_angle - player_rot);

		get_ray_hit(adj_angle, player_x, player_y, &hit);

		if(hit.hit_pos[0] != -1 && hit.hit_pos[1] != -1) {
			// WALL CASTING
			wall = hit.wall_type;

			slice_dist = (hit.dist * cos128table[correct_angle]) >> 7;
			slice_height = (int)(64.0f / slice_dist * DIST_TO_PROJ);

			src.y = 0;
			src.w = 1;
			src.h = 63;
			src.x = hit.is_horiz ? (hit.hit_pos[0] % UNIT_SIZE) : (hit.hit_pos[1] % UNIT_SIZE);

			dest.x = i;
			dest.y = 100 - (slice_height >> 1);
			dest.w = 1;
			dest.h = (100 + (slice_height >> 1)) - dest.y;

			SDL_RenderCopy(renderer, walls[wall].texture, &src, &dest);

			// FLOOR/CEILING CASTING.
			// dest.h + dest.y == bottom of the wall
			for(j = dest.h + dest.y + 1; j < PROJ_H; ++j) {
				straight_dist = (int)(DIST_TO_PROJ * 32 / (j - 100));
				dist_to_point = (straight_dist << 7) / (cos128table[correct_angle]);

				// Use adjusted so it gives us the direction of the "true" ray angle.
				p_x = player_x + ((dist_to_point * cos128table[adj_angle]) >> 7);
				p_y = player_y - ((dist_to_point * sin128table[adj_angle]) >> 7);

				// Gives us the texture location.
				t_x = p_x % UNIT_SIZE;
				t_y = p_y % UNIT_SIZE;

				// Put floor pixel.
				t_color = (unsigned char*)floor_surf->pixels + t_y * floor_surf->pitch + t_x * 3;
				floor_pixels[j * PROJ_W + i] = 0xFF000000 | t_color[0] << 16 | t_color[1] << 8 | t_color[2];
				// Put ceiling pixel.
				t_color = (unsigned char*)ceiling_surf->pixels + t_y * ceiling_surf->pitch + t_x * 3;
				floor_pixels[(-j + 200)  * PROJ_W + i] = 0xFF000000 | t_color[0] << 16 | t_color[1] << 8 | t_color[2];
				//SDL_SetRenderDrawColor(renderer, t_color[0], t_color[1], t_color[2], 255);
				//SDL_RenderDrawPoint(renderer, i, j);
			}
		}

		curr_angle -= ANGLE_BETWEEN_RAYS;
	}

	SDL_UpdateTexture(floor_tex, NULL, floor_pixels, 320 * 4);
	SDL_RenderCopy(renderer, floor_tex, NULL, NULL);
}
