// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

#include "raycaster.h"
//#include "map.h"

#include <stdio.h>
#include <math.h>

// TODO: Remove constants.

// RAYCASTER STATE VARIABLES
static int player_x, player_y;
static struct mapdef* map;

static int is_tan_undefined_for_angle(const int deg) {
	return deg == 0 || deg == 90 || deg == 180 || deg == 270 || deg == 360;
}

int is_angle_in_quadrant_1(const int deg) {
	return 1 <= deg && deg <= 89;
}

int is_angle_in_quadrant_2(const int deg) {
	return 91 <= deg && deg <= 179;
}

int is_angle_in_quadrant_3(const int deg) {
	return 181 <= deg && deg <= 269;
}

int is_angle_in_quadrant_4(const int deg) {
	return 271 <= deg && deg <= 359;
}

void compute_tan_lookup_val_for_angle(const int deg) {
	if(is_tan_undefined_for_angle(deg))
		tan128table[deg] = -1;
	else
		tan128table[deg] = (int)round(tan(deg * M_PI / 180.0f) * 128);
}

void compute_inverse_tan_lookup_val_for_angle(const int deg) {
	if(is_tan_undefined_for_angle(deg))
		tan1table[deg] = -1;
	else
		tan1table[deg] = (int)round(128.0 / tan(deg * M_PI / 180.0f));
}

void compute_delta_lookup_vals_quadrant_1(const int deg) {
	// 64 / tan(ray_angle). We must account for the 128.
	delta_h_x[deg] = (1 << 13) / tan128table[deg];
	delta_h_y[deg]= -UNIT_SIZE;

	delta_v_x[deg] = UNIT_SIZE;
	// Compute -tan(angle) * 64
	delta_v_y[deg] = -((tan128table[deg] << UNIT_POWER) >> 7);
}

void compute_delta_lookup_vals_quadrant_2(const int deg) {
	// -64, since we are travelling in the negative y direction.
	delta_h_y[deg] = -UNIT_SIZE;
	// Computes -64 / tan(deg). Negative since we're
	// travelling in the negative y direction.
	delta_h_x[deg] = (tan128table[deg - 90] * -UNIT_SIZE) >> 7;

	delta_v_x[deg] = -UNIT_SIZE;
	delta_v_y[deg] = -((1 << 13) / tan128table[deg - 90]);
}

void compute_delta_lookup_vals_quadrant_3(const int deg) {
	delta_h_y[deg] = UNIT_SIZE;
	delta_h_x[deg]  = -(1 << 13) / tan128table[deg - 180];

	delta_v_x[deg] = -UNIT_SIZE;
	// Computes 64 * tan(ray_angle).
	delta_v_y[deg]  = (UNIT_SIZE * tan128table[deg - 180]) >> 7;
}

void compute_delta_lookup_vals_quadrant_4(const int deg) {
	delta_h_y[deg] = UNIT_SIZE;
	// Computes 64 * tan(ray_angle)
	delta_h_x[deg] = (UNIT_SIZE * tan128table[deg - 270]) >> 7;

	delta_v_x[deg] = UNIT_SIZE;
	// Computes 64 / tan(ray_angle)
	delta_v_y[deg] = (1 << 13) / tan128table[deg - 270];
}

void compute_delta_lookup_vals_for_angle(const int deg) {
	if(is_tan_undefined_for_angle(deg)) {
		delta_h_x[deg] = 0;
		delta_h_y[deg] = 0;
		delta_v_x[deg] = 0;
		delta_v_y[deg] = 0;
	}

	if(is_angle_in_quadrant_1(deg)) {
		compute_delta_lookup_vals_quadrant_1(deg);
	} else if(is_angle_in_quadrant_2(deg)) {
		compute_delta_lookup_vals_quadrant_2(deg);
	} else if(is_angle_in_quadrant_3(deg)) {
		compute_delta_lookup_vals_quadrant_3(deg);
	} else if(is_angle_in_quadrant_4(deg)) {
		compute_delta_lookup_vals_quadrant_4(deg);
	}
}

void compute_lookup_vals_for_angle(const int deg) {
	// Stores the angle in radians.
	float curr_angle;

	compute_tan_lookup_val_for_angle(deg);
	compute_inverse_tan_lookup_val_for_angle(deg);
	sin128table[deg] = (int)round(sin(deg * M_PI / 180.0f) * 128);
	cos128table[deg] = (int)round(cos(deg * M_PI / 180.0f) * 128);
	sin1table[deg] = (int)(round(128.0 / sin(deg * M_PI / 180.0f)));

	compute_delta_lookup_vals_for_angle(deg);
}

void initialize_lookup_tables() {
	int deg;

	for(deg = 0; deg <= 360; ++deg)
		compute_lookup_vals_for_angle(deg);
}

void initialize_render_textures(SDL_Renderer* renderer) {
	floor_ceiling_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
	raycast_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
	thing_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);

	SDL_SetTextureBlendMode(floor_ceiling_tex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(raycast_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(thing_texture, SDL_BLENDMODE_BLEND);
}

// TODO: Add to some kind of utils file.
int get_tile(int x, int y, struct mapdef* map) {
	int grid_x = x >> UNIT_POWER;
	int grid_y = y >> UNIT_POWER;

	if(grid_x < 0 || grid_x > map->map_w - 1)
		return -1;
	if(grid_y < 0 || grid_y > map->map_h - 1)
		return -1;

	return map->layout[grid_y * map->map_w + grid_x];
}

int get_dist_sqrd(int x1, int y1, int x2, int y2) {
	int d_x, d_y;

	d_x = x1 - x2;
	d_y = y1 - y2;

	d_x *= d_x;
	d_y *= d_y;

	return d_x + d_y;
}

static void compute_initial_ray_pos_when_angle_in_quad_1(const int ray_angle, int curr_h[2], int curr_v[2]) {
	int alpha = ray_angle;

	// Divide player_y by 64, floor that, multiply by 64, and then subtract 1.
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
	// Multiply player_y and curr_h[1] by 128, then divide by the tan * 128. This will
	// undo the the 128 multiplication without having a divide by 0 (For example, tan128table[1]).
	curr_h[0] = (((player_y - curr_h[1]) * tan1table[alpha]) >> 7) + player_x;

	// Divide player_x by 64, floor the result, multiply by 64 and add 64.
	curr_v[0]= ((player_x >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Get the tan(curr_v[0] - player_x) and subtract that from player_y.
	curr_v[1]= player_y - (((curr_v[0]- player_x) * tan128table[alpha]) >> 7);
}

static void compute_initial_ray_pos_when_angle_in_quad_2(const int ray_angle, int curr_h[2], int curr_v[2]) {
	// Adjusts the angle so its between 1 and 89.
	int alpha = ray_angle - 90;
	// Compute floor(player_y / 64) * 64 - 1.
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) - 1;
	// Computes player_x - (player_y * curr_h[1]) * tan(ray_angle).
	curr_h[0] = player_x - ((tan128table[alpha] * (player_y - curr_h[1])) >> 7);

	// Compute floor(player_x / 64) * 64 - 1.
	curr_v[0] = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
	// Compute player_y - (player_x - curr_v[0]) / tan(ray_angle).
	curr_v[1] = player_y - (((player_x - curr_v[0]) * tan1table[alpha]) >> 7);
}

static void compute_initial_ray_pos_when_angle_in_quad_3(const int ray_angle, int curr_h[2], int curr_v[2]) {
	// Adjusts the angle so its between 1 and 89.
	int alpha = ray_angle - 180;

	// Computes floor(player_y / 64) * 64 + 64.
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Computes player_x - (curr_h[1] - player_y / tan(ray_angle).
	curr_h[0] = player_x - (((curr_h[1]- player_y) * tan1table[alpha]) >> 7);

	// Computes floor(player x / 64) * 64 - 1.
	curr_v[0] = ((player_x >> UNIT_POWER) << UNIT_POWER) - 1;
	// Computes tan(ray_angle) * (player_x - curr_v[0]) + player_y.
	curr_v[1] = ((tan128table[alpha] * (player_x  - curr_v[0])) >> 7) + player_y;
}

static void compute_initial_ray_pos_when_angle_in_quad_4(const int ray_angle, int curr_h[2], int curr_v[2]) {
	// Adjusts the angle so its between 1 and 89.
	int alpha = ray_angle - 270;

	// Computes floor(player_y / 64) * 64 + 64
	curr_h[1] = ((player_y >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Computes (curr_h[1] - player_y) * tan(ray_angle) + player_x
	curr_h[0] = (((curr_h[1] - player_y) * tan128table[alpha]) >> 7) + player_x;

	// Computes floor(player_x / 64) * 64 + 64
	curr_v[0] = ((player_x >> UNIT_POWER) << UNIT_POWER) + UNIT_SIZE;
	// Computes (curr_v[0] - player_x) / tan(ray_angle) + player_y.
	curr_v[1] = (((curr_v[0] - player_x) * tan1table[alpha]) >> 7) + player_y;
}

static int compute_initial_ray_pos(const int ray_angle, int curr_h[2], int curr_v[2]) {
	if(is_tan_undefined_for_angle(ray_angle))
		return 0;

	int alpha;

	if(is_angle_in_quadrant_1(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_1(ray_angle, curr_h, curr_v);
	} else if(is_angle_in_quadrant_2(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_2(ray_angle, curr_h, curr_v);
	} else if(is_angle_in_quadrant_3(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_3(ray_angle, curr_h, curr_v);
	} else if(is_angle_in_quadrant_4(ray_angle)) {
		compute_initial_ray_pos_when_angle_in_quad_4(ray_angle, curr_h, curr_v);
	}

	return 1;
}

static void compute_ray_delta_vectors(const int ray_angle, int delta_h[2], int delta_v[2]) {
	delta_h[0] = delta_h_x[ray_angle];
	delta_h[1] = delta_h_y[ray_angle];
	delta_v[0] = delta_v_x[ray_angle];
	delta_v[1] = delta_v_y[ray_angle];
}

static int tile_is_floor_ceil(const int tile) {
	return -1 < tile && tile < map->num_floor_ceils;
}

static void move_ray_pos(int ray_pos[2], int ray_delta[2]) {
	ray_pos[0] += ray_delta[0];
	ray_pos[1] += ray_delta[1];
}

static void compute_ray_hit_position(int curr_pos[2], int delta[2], int hit[2]) {
	int tile;

	tile = get_tile(curr_pos[0], curr_pos[1], map);
	while(tile_is_floor_ceil(tile)) {
		move_ray_pos(curr_pos, delta);
		tile = get_tile(curr_pos[0], curr_pos[1], map);
	}

	// We went outside the bounds of the map.
	if(tile == -1) {
		hit[0] = -1;
		hit[1] = -1;
	} else {
		hit[0] = curr_pos[0];
		hit[1] = curr_pos[1];
	}
}

static int both_ray_horizontal_and_vertical_hit_pos_invalid(int hit_h[2], int hit_v[2]) {
	return hit_h[0] == -1 && hit_h[1] == -1 && hit_v[0] == -1 && hit_v[1] == -1;
}

static int ray_hit_pos_is_invalid(int hit_pos[2]) {
	return hit_pos[0] == -1 && hit_pos[1] == -1;
}

static void set_hit(struct hitinfo* to_set, int hit_pos[2], const int is_horiz) {
	to_set->hit_pos[0] = hit_pos[0];
	to_set->hit_pos[1] = hit_pos[1];
	to_set->is_horiz = is_horiz;
	to_set->dist = sqrt(get_dist_sqrd(player_x, player_y, hit_pos[0], hit_pos[1]));
}

static void choose_ray_pos_according_to_shortest_dist(struct hitinfo* hit, int hit_h[2], int hit_v[2]) {
	int h_dist;
	int v_dist;
	
	h_dist = get_dist_sqrd(player_x, player_y, hit_h[0], hit_h[1]);
	v_dist = get_dist_sqrd(player_x, player_y, hit_v[0], hit_v[1]);

	if(h_dist < v_dist) {
		set_hit(hit, hit_h, 1);
	} else {
		set_hit(hit, hit_v, 0);
	}
}

static void choose_ray_horizontal_or_vertical_hit_pos(int hit_h[2], int hit_v[2], struct hitinfo* hit) {
	if(both_ray_horizontal_and_vertical_hit_pos_invalid(hit_h, hit_v)) {
		hit->hit_pos[0] = -1;
		hit->hit_pos[1] = -1;
	} else if(ray_hit_pos_is_invalid(hit_h)) {
		set_hit(hit, hit_v, 0);
	} else if(ray_hit_pos_is_invalid(hit_v)) {
		set_hit(hit, hit_h, 1);
	} else {
		choose_ray_pos_according_to_shortest_dist(hit, hit_h, hit_v);
	}
}

void get_ray_hit(int ray_angle, struct hitinfo* hit) {
	// Stores the position of the ray as it moves
	// from one grid line to the next. x is 0, y is 1
	int curr_h[2];
	int curr_v[2];
	// How much we move from curr_x and curr_y.
	int delta_h[2];
	int delta_v[2];
	// Where the final ray position is traveling along
	// horizontal and vertical grid lines.
	int hit_h[2];
	int hit_v[2];

	if(compute_initial_ray_pos(ray_angle, curr_h, curr_v) == 0) {
		hit->hit_pos[0] = -1;
		hit->hit_pos[1] = -1;
		return;
	}

	compute_ray_delta_vectors(ray_angle, delta_h, delta_v);

	// Now find the point that is a wall by travelling along horizontal gridlines.
	compute_ray_hit_position(curr_h, delta_h, hit_h);
	// Now find the point that is a wall by travelling along vertical gridlines.
	compute_ray_hit_position(curr_v, delta_v, hit_v);

	// Now choose either the horizontal or vertical intersection
	// point. Or choose -1, -1 to denote an error.
	choose_ray_horizontal_or_vertical_hit_pos(hit_h, hit_v, hit);

	hit->wall_type = get_tile(hit->hit_pos[0], hit->hit_pos[1], map);
}

// TODO: Move this elsewhere.
unsigned int get_pixel(SDL_Surface* surface, int x, int y) {
	if(!surface)
		return 0;
	if(x < 0 || x >= surface->w)
		return 0;
	if(y < 0 || y >= surface->h)
		return 0;

	// Stores the channels of the pixel color.
	unsigned char* channels;
	// Used to compute channels and decides how we construct result.
	int bytes_per_pixel = surface->format->BytesPerPixel;
	// What we will return.
	unsigned int result;

	channels = (unsigned char*)surface->pixels + y * surface->pitch + x * bytes_per_pixel;

	switch(bytes_per_pixel) {
		case 1:
			result = 0xFF000000 | channels[0] << 16 | channels[1] << 8 | channels[2];
			break;
		case 2:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			result = *(unsigned short*)(channels);
			break;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				result = 0xFF000000 | channels[0] << 16 | channels[1] << 8 | channels[2];
			else
				result = 0xFF000000 | channels[2] << 16 | channels[1] << 8 | channels[0];
			break;
		case 4:
			result = *(unsigned int*)channels;
			break;
		default:
			result = 0;
			break;
	}

	return result;
}

void draw_sky_slice(const int screen_col, const int adj_angle) {
	if(!map || !map->sky_surf)
		return;

	// A skybox has 640 columns of pixels.
	// There are 360 degrees in a circle.
	// So to convert an angle to the corresponding column,
	// we do 640 / 360, which is roughly 1.77. We can just round this
	// to two.
	int adj_angle_to_pixel_col = (adj_angle << 1) % SKYBOX_TEX_WIDTH;

	int j;
	for(j = 0; j < PROJ_H; ++j)
		raycast_pixels[j * PROJ_W + screen_col] = get_pixel(map->sky_surf, adj_angle_to_pixel_col, j);
}


/*struct hitinfo {
	// Where we hit.
	int hit_pos[2];
	// The squared distance.
	int dist;
	// The wall texture.
	int wall_type;
	// If true, the intersection was along a horizontal grid.
	// Otherwise, it was vertical.
	int is_horiz;
	// Used when correcting for 'fisheye' lens.
	int quadrant;
};*/

/*struct draw_wall_slice_args {
	// Data for ray hitting wall slice.
	struct hitinfo* hit;
	// Used to compute the "correct" distance from the
	// player to the wall slice so as to avoid the "fish-eye"
	// effect.
	int correct_angle;
	// The angle of the ray adjusted to be from 0 to 360.
	int adj_angle;
	// Where we render the wall slice on the screen.
	int screen_col;
};*/

struct slice {
	// The row of pixels on the screen we want to render from (top-most row going down).
	int screen_row;
	// The column of pixels on the screen we want to render from.
	int screen_col;
	// The height in pixels of the slice. No width since a slice is just a single line of pixels.
	int screen_height;
	// The texture we want to render.
	int wall_tex;
	// The column of pixels we want to render.
	int tex_col;
};

static void draw_wall_slice(struct slice* slice) {

	// Manually copies texture from source to portion of screen.
	int j;
	for(j = 0; j < slice->screen_height; ++j) {
		// j + screen_slice_y gives us the position to render the current pixel on the screen.
		if(j + slice->screen_row < 0 || j + slice->screen_row  >= PROJ_H)
			continue;

		raycast_pixels[(j + slice->screen_row) * PROJ_W + slice->screen_col] =
			get_pixel(map->walls[slice->wall_tex].surf, slice->tex_col, (j << 6) / slice->screen_height);
	}
}

static unsigned int correct_hit_dist_for_fisheye_effect(const int hit_dist, const int correct_angle) {
	unsigned int correct_dist = (hit_dist * cos128table[correct_angle]) >> 7;

	// Make sure we don't get any issues computing the slice height.
	if(correct_dist == 0)
		correct_dist = 1;

	return correct_dist;
}

static void compute_wall_slice_render_data_from_hit_and_screen_col(struct hitinfo* hit, const int screen_col, struct slice* slice) {
	// Height of the slice in the world
	unsigned int slice_height;

	// Dist to projection * 64 / slice dist.
	slice_height = (DIST_TO_PROJ << 6) / hit->dist;

	// Define the part of the screen we render to such that it is a single column with the
	// slice's middle pixel at the center of the screen.
	slice->screen_row  = HALF_PROJ_H - (slice_height >> 1);
	slice->screen_col = screen_col;
	slice->screen_height = (HALF_PROJ_H + (slice_height >> 1)) - slice->screen_row;

	slice->wall_tex = hit->wall_type - map->num_floor_ceils;
	// Use a single column of pixels based on where the ray hit.
	slice->tex_col = hit->is_horiz ? (hit->hit_pos[0] % UNIT_SIZE) : (hit->hit_pos[1] % UNIT_SIZE);
}

// TODO: Have struct for player data.
void cast_rays(SDL_Renderer* renderer, struct mapdef* curr_map, int curr_player_x, int curr_player_y, int player_rot) {
	// Stores the precise angle of our current ray.
	float curr_angle = (float)(player_rot + FOV_HALF);
	// The curr_angle adjusted to be within 0 and 360.
	int adj_angle;
	// The angle used to compute the "corrected" distance so
	// we avoid the fisheye effect.
	int correct_angle;

	// Returns info about the hit.
	struct hitinfo hit;
	// Data needed to render a wall slice.
	struct slice wall_slice;

	int i;

	// Update all state variables.
	player_x = curr_player_x;
	player_y = curr_player_y;
	map = curr_map;

	// Begin by clearning the pixel arrays that we copy to.
	for(i = 0; i < 64000; ++i) {
		floor_ceiling_pixels[i] = 0;
		raycast_pixels[i] = 0;
		thing_pixels[i] = 0;
	}

	preprocess_things();

	// Now loop through each column of pixels on the screen and do ray casting.
	for(i = 0; i < PROJ_W; ++i) {
		adj_angle = get_adjusted_angle((int)curr_angle);

		z_buffer[i] = 0;
		get_ray_hit(adj_angle, &hit);
		if(hit.hit_pos[0] != -1 && hit.hit_pos[1] != -1) {

			z_buffer[i] = hit.dist;

			// Computes the angle relative to the player rotation.
			correct_angle = abs(adj_angle - player_rot);
			hit.dist = correct_hit_dist_for_fisheye_effect(hit.dist, correct_angle);

			// SKY CASTING
			draw_sky_slice(i, adj_angle);

			// WALL, FLOOR, CEILING CASTING

			compute_wall_slice_render_data_from_hit_and_screen_col(&hit, i, &wall_slice);
			draw_wall_slice(&wall_slice);
			//draw_floor_and_ceiling(screen_slice_y, screen_slice_h, args);
		}

		curr_angle -= ANGLE_BETWEEN_RAYS;
	}

	// THING CASTING
	draw_things(player_rot);

	// Draw pixel arrays to screen.
	SDL_UpdateTexture(raycast_texture, NULL, raycast_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, raycast_texture, NULL, NULL);

	SDL_UpdateTexture(floor_ceiling_tex, NULL, floor_ceiling_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, floor_ceiling_tex, NULL, NULL);

	SDL_UpdateTexture(thing_texture, NULL, thing_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, thing_texture, NULL, NULL);
}

void draw_floor_and_ceiling(int screen_slice_y, int screen_slice_h, struct draw_wall_slice_args* dws) {
	int straight_dist;
	int dist_to_point;

	// The floor/ceiling position in "world" space.
	int p_x, p_y;

	// The floor/ceiling texture type to draw.
	unsigned int floor_ceil_type;

	// screen_slice_h + screen_slice_y == bottom of the wall
	int j = 0;
	for(j = screen_slice_y + screen_slice_h; j < PROJ_H; ++j) {
		// Compute the distance from the player to the point.
		straight_dist = (int)(DIST_TO_PROJ * HALF_UNIT_SIZE / (j - HALF_PROJ_H));
		dist_to_point = (straight_dist << 7) / (cos128table[dws->correct_angle]);

		// Use adjusted so it gives us the direction of the "true" ray angle.
		p_x = player_x + ((dist_to_point * cos128table[dws->adj_angle]) >> 7);
		p_y = player_y - ((dist_to_point * sin128table[dws->adj_angle]) >> 7);

		floor_ceil_type = get_tile(p_x, p_y, map);

		if(floor_ceil_type >= map->num_floor_ceils)
			continue;

		// Put floor pixel.
		//printf("%d\n", floor_ceil);
		if(map->floor_ceils[floor_ceil_type].floor_surf) {
			floor_ceiling_pixels[j * PROJ_W + dws->screen_col] = get_pixel(map->floor_ceils[floor_ceil_type].floor_surf,
															 		  p_x % UNIT_SIZE, p_y % UNIT_SIZE);
		}

		// Put ceiling pixel.
		if(map->floor_ceils[floor_ceil_type].ceil_surf) {
			floor_ceiling_pixels[(-j + PROJ_H) * PROJ_W + dws->screen_col] = get_pixel(map->floor_ceils[floor_ceil_type].ceil_surf,
																				  p_x % UNIT_SIZE, p_y % UNIT_SIZE);
		}
	}
}

void draw_things(int player_rot) {
	// The texture point.
	int t_x, t_y;
	// RGB value of the sprite texture.
	unsigned int t_color;

	int x_diff, y_diff;

	// Used to find the sprite position on the screen.
	int theta_temp;
	// The position of the sprite on the screen.
	int scr_x, scr_y;

	// Defines the sprite's screen dimensions and position.
	SDL_Rect thing_rect;
	// Defines the column of pixels of the sprite we want.
	SDL_Rect thing_src_rect;

	// For handling animations.
	unsigned int curr_anim;
	// How much we add to t_x, t_y to get the correct animation frame.
	int frame_offset_x, frame_offset_y;

	int i, j, k, m;

	for(i = 0; i < map->num_things; ++i) {
		if(things_sorted[i]->type == 0)
			continue;

		x_diff = things_sorted[i]->position[0] - player_x;
		y_diff = things_sorted[i]->position[1] - player_y;

		theta_temp = (int)(atan2(-y_diff, x_diff) * RAD_TO_DEG);

		// Make sure the angle is between 0 and 360.
		if(theta_temp < 0)
			theta_temp += 360;

		scr_y = player_rot + FOV_HALF - theta_temp;


		if(theta_temp > 270 && player_rot < 90)
			scr_y = player_rot + FOV_HALF - theta_temp + 360;
		if(player_rot > 270 && theta_temp < 90)
			scr_y = player_rot + FOV_HALF - theta_temp - 360;

		scr_x = scr_y * PROJ_W / FOV;

		thing_rect.w = (int)(UNIT_SIZE / sqrt(things_sorted[i]->dist) * DIST_TO_PROJ);
		thing_rect.h = thing_rect.w;
		thing_rect.y = HALF_PROJ_H - (thing_rect.h >> 1);
		thing_rect.x = scr_x - (thing_rect.w >> 1);

		// The column for the scaled texture.
		m = 0;

		curr_anim = things_sorted[i]->curr_anim;
		// Take starting position and multiply by 64 to go from unit coordinates to pixel coordinates.
		// This puts us in the correct position for the animation as a whole.
		frame_offset_x = (int)(things_sorted[i]->anims[curr_anim].start_x) << 6;
		frame_offset_y = (int)(things_sorted[i]->anims[curr_anim].start_y) << 6;
		// Add the current frame to the offset so that we have the correct frame.
		// Note that since animations progress only horizontally, we don't need to
		// do anything to the y part of the offset.
		frame_offset_x += things_sorted[i]->anims[curr_anim].curr_frame << 6;

		for(j = thing_rect.x; j < thing_rect.x + thing_rect.w; ++j) {
			if(j >= 0 && j < PROJ_W) {
				// Render the current slice of sprite only if infront of wall.
				if(sqrt(things_sorted[i]->dist) - 1 < z_buffer[j]) {
					thing_src_rect.x = (m << 6) / thing_rect.w;
					thing_src_rect.y = 0;
					thing_src_rect.w = 1;
					thing_src_rect.h = UNIT_SIZE;

					// Render the column of sprites.
					//SDL_RenderCopy(renderer, things_sorted[i]->texture, &thing_src_rect, &thing_dest_rect);
					for(k = 0; k < thing_rect.h; ++k) {
						if(k + thing_rect.y < 0 || k + thing_rect.y >= 200)
							continue;

						t_x = thing_src_rect.x;
						t_y = (k << 6) / thing_rect.h;
						//t_color = (unsigned char*)(things_sorted[i]->surf->pixels + t_y * things_sorted[i]->surf->pitch + t_x * 4);
						t_color = get_pixel(things_sorted[i]->surf, t_x + frame_offset_x, t_y + frame_offset_y);
						// Only put a pixel if it is not transparent.
						if(((unsigned char*)(&t_color))[3] > 0)
							thing_pixels[(k + thing_rect.y) * PROJ_W + j] = t_color;
					}
				}
			}

			++m;
		}
	}
}

void preprocess_things() {
	unsigned int i;

	// Compute the distance between each thing and the player.
	for(i = 0; i < map->num_things; ++i) {
		map->things[i].dist = get_dist_sqrd(map->things[i].position[0], map->things[i].position[1],
											player_x, player_y);

		if(map->things[i].dist == 0)
			map->things[i].dist = 1;

		// Add the thing to the sorted list.
		things_sorted[i] = &(map->things[i]);
	}

	// Now, sort the things according to distance.
	sort_things(0, map->num_things - 1);
}

int get_adjusted_angle(int curr_angle) {
	int adj_angle = curr_angle;

	// Make the angle between 0 and 360.
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

	return adj_angle;
}


void sort_things(int s, int e) {
	if(e <= s)
		return;

	int m = partition(s, e);

	sort_things(s, m);
	sort_things(m + 1, e);
}

int partition(int s, int e) {
	int i = s - 1;
	int j = e + 1;

	int p_dist = things_sorted[s]->dist;

	struct thingdef* temp;

	while(1) {
		do {
			i += 1;
		} while(things_sorted[i]->dist > p_dist);

		do {
			j -= 1;
		} while(things_sorted[j]->dist < p_dist);

		if(i >= j)
			return j;

		if(things_sorted[i]->dist < things_sorted[j]->dist) {
			temp = things_sorted[i];
			things_sorted[i] = things_sorted[j];
			things_sorted[j] = temp;
		}
	}
}
