// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

#include "raycaster.h"
//#include "map.h"

#include <stdio.h>
#include <math.h>

int is_tan_undefined_for_angle(int deg) {
	return deg == 0 || deg == 90 || deg == 180 || deg == 270 || deg == 360;
}

int is_angle_in_quadrant_1(int deg) {
	return 1 <= deg && deg <= 89;
}

int is_angle_in_quadrant_2(int deg) {
	return 91 <= deg && deg <= 179;
}

int is_angle_in_quadrant_3(int deg) {
	return 181 <= deg && deg <= 269;
}

int is_angle_in_quadrant_4(int deg) {
	return 271 <= deg && deg <= 359;
}

void compute_tan_lookup_val_for_angle(int deg) {
	if(is_tan_undefined_for_angle(deg))
		tan128table[deg] = -1;
	else
		tan128table[deg] = (int)round(tan(deg * M_PI / 180.0f) * 128);
}

void compute_inverse_tan_lookup_val_for_angle(int deg) {
	if(is_tan_undefined_for_angle(deg))
		tan1table[deg] = -1;
	else
		tan1table[deg] = (int)round(128.0 / tan(deg * M_PI / 180.0f));
}

void compute_delta_lookup_vals_quadrant_1(int deg) {
	// 64 / tan(ray_angle). We must account for the 128.
	delta_h_x[deg] = (1 << 13) / tan128table[deg];
	delta_h_y[deg]= -UNIT_SIZE;

	delta_v_x[deg] = UNIT_SIZE;
	// Compute -tan(angle) * 64
	delta_v_y[deg] = -((tan128table[deg] << UNIT_POWER) >> 7);
}

void compute_delta_lookup_vals_quadrant_2(int deg) {
	// -64, since we are travelling in the negative y direction.
	delta_h_y[deg] = -UNIT_SIZE;
	// Computes -64 / tan(deg). Negative since we're
	// travelling in the negative y direction.
	delta_h_x[deg] = (tan128table[deg - 90] * -UNIT_SIZE) >> 7;

	delta_v_x[deg] = -UNIT_SIZE;
	delta_v_y[deg] = -((1 << 13) / tan128table[deg - 90]);
}

void compute_delta_lookup_vals_quadrant_3(int deg) {
	delta_h_y[deg] = UNIT_SIZE;
	delta_h_x[deg]  = -(1 << 13) / tan128table[deg - 180];

	delta_v_x[deg] = -UNIT_SIZE;
	// Computes 64 * tan(ray_angle).
	delta_v_y[deg]  = (UNIT_SIZE * tan128table[deg - 180]) >> 7;
}

void compute_delta_lookup_vals_quadrant_4(int deg) {
	delta_h_y[deg] = UNIT_SIZE;
	// Computes 64 * tan(ray_angle)
	delta_h_x[deg] = (UNIT_SIZE * tan128table[deg - 270]) >> 7;

	delta_v_x[deg] = UNIT_SIZE;
	// Computes 64 / tan(ray_angle)
	delta_v_y[deg] = (1 << 13) / tan128table[deg - 270];
}

void compute_delta_lookup_vals_for_angle(int deg) {
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

void compute_lookup_vals_for_angle(int deg) {
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

void get_ray_hit(int ray_angle, int player_x, int player_y, struct hitinfo* hit, struct mapdef* map) {
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

	// Now find the point that is a wall by travelling along horizontal gridlines.
	tile = get_tile(curr_h_x, curr_h_y, map);
	while(-1 < tile && tile < map->num_floor_ceils) {
		curr_h_x += delt_h_x;
		curr_h_y += delt_h_y;
		tile = get_tile(curr_h_x, curr_h_y, map);
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
	tile = get_tile(curr_v_x, curr_v_y, map);
	while(-1 < tile && tile < map->num_floor_ceils) {
		curr_v_x += delt_v_x;
		curr_v_y += delt_v_y;
		tile = get_tile(curr_v_x, curr_v_y, map);
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

		hit->dist = sqrt((player_x - hit_v[0]) * (player_x - hit_v[0]) + (player_y - hit_v[1]) * (player_y - hit_v[1]));
	}

	else if(hit_v[0] == -1 && hit_v[1] == -1) {
		hit->hit_pos[0] = hit_h[0];
		hit->hit_pos[1] = hit_h[1];
		hit->is_horiz = 1;

		hit->dist = sqrt((player_x - hit_h[0]) * (player_x - hit_h[0]) + (player_y - hit_h[1]) * (player_y - hit_h[1]));
	}

	else {

		h_dist = sqrt((player_x - hit_h[0]) * (player_x - hit_h[0]) + (player_y - hit_h[1]) * (player_y - hit_h[1]));
		v_dist = sqrt((player_x - hit_v[0]) * (player_x - hit_v[0]) + (player_y - hit_v[1]) * (player_y - hit_v[1]));

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

	hit->wall_type = get_tile(hit->hit_pos[0], hit->hit_pos[1], map);
}

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

void draw_sky(struct mapdef* map, int screen_col, int adj_angle) {
	if(!map || !map->sky_surf)
		return;

	int j;
	for(j = 0; j < 200; ++j)
		raycast_pixels[j * PROJ_W + screen_col] = get_pixel(map->sky_surf, (int)(adj_angle * 1.77) % 640, j);
}

void draw_wall_slice(struct draw_wall_slice_args* args) {
	// The wall texture we will render.
	unsigned char wall;
	// The "correct" distance of the slice. Using this fixes
	// the fish-eye effect.
	unsigned int slice_dist;
	// The height of the slice to render.
	unsigned int slice_height;

	// Stores the column of pixels from the wall texture we
	// wish to render.
	unsigned int tex_col;

	// Where we will render the wall slice along the y axis.
	int screen_slice_y;
	// The height of the slice on the screen
	int screen_slice_h;

	wall = args->hit->wall_type - args->map->num_floor_ceils;
	slice_dist = (args->hit->dist * cos128table[args->correct_angle]) >> 7;

	// Make sure we don't get any issues computing the slice height.
	if(slice_dist == 0)
		slice_dist = 1;

	// Dist to projection * 64 / slice dist.
	slice_height = (DIST_TO_PROJ << 6) / slice_dist;

	// Use a single column of pixels based on where the ray hit.
	tex_col = args->hit->is_horiz ? (args->hit->hit_pos[0] % UNIT_SIZE) :
									(args->hit->hit_pos[1] % UNIT_SIZE);

	// Define the part of the screen we render to such that it is a single column with the
	// slice's middle pixel at the center of the screen.
	screen_slice_y = HALF_PROJ_H - (slice_height >> 1);
	screen_slice_h = (HALF_PROJ_H + (slice_height >> 1)) - screen_slice_y;

	// Manually copies texture from source to portion of screen.
	int j;
	for(j = 0; j < screen_slice_h; ++j) {
		// j + screen_slice_y gives us the position to render the current pixel on the screen.
		if(j + screen_slice_y < 0 || j + screen_slice_y >= 200)
			continue;

		raycast_pixels[(j + screen_slice_y) * PROJ_W + args->screen_col] =
			get_pixel(args->map->walls[wall].surf, tex_col, (j << 6) / screen_slice_h);
	}

	// FLOOR/CEILING CASTING.
	draw_floor_and_ceiling(screen_slice_y, screen_slice_h, args);
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
		p_x = dws->player_x + ((dist_to_point * cos128table[dws->adj_angle]) >> 7);
		p_y = dws->player_y - ((dist_to_point * sin128table[dws->adj_angle]) >> 7);

		floor_ceil_type = get_tile(p_x, p_y, dws->map);

		// Gives us the texture location.
		//t_x = p_x % UNIT_SIZE;
		//t_y = p_y % UNIT_SIZE;

		if(floor_ceil_type >= dws->map->num_floor_ceils)
			continue;

		// Put floor pixel.
		//printf("%d\n", floor_ceil);
		if(dws->map->floor_ceils[floor_ceil_type].floor_surf) {
			floor_ceiling_pixels[j * PROJ_W + dws->screen_col] = get_pixel(dws->map->floor_ceils[floor_ceil_type].floor_surf,
															 		  p_x % UNIT_SIZE, p_y % UNIT_SIZE);
		}

		// Put ceiling pixel.
		if(dws->map->floor_ceils[floor_ceil_type].ceil_surf) {
			floor_ceiling_pixels[(-j + PROJ_H) * PROJ_W + dws->screen_col] = get_pixel(dws->map->floor_ceils[floor_ceil_type].ceil_surf,
																				  p_x % UNIT_SIZE, p_y % UNIT_SIZE);
		}
	}
}

void draw_things(struct mapdef* map, int player_x, int player_y, int player_rot) {
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

		// Compute orientation data.
		// Get relative angle of thing assuming player was facing 90 degrees.
		/*transformed_rotation = (things_sorted[i]->rotation - player_rot) + 90;
		// Correct transformed_rotation
		if(transformed_rotation < 0)
			transformed_rotation += 360;
		if(transformed_rotation >= 360)
			transformed_rotation -= 360;

		if(0 <= transformed_rotation && transformed_rotation <= 22)
			orientation = 6;
		else if(23 <= transformed_rotation && transformed_rotation <= 68)
			orientation = 5;
		else if(69 <= transformed_rotation && transformed_rotation <= 113)
			orientation = 4;
		else if(114 <= transformed_rotation && transformed_rotation <= 158)
			orientation = 3;
		else if(159 <= transformed_rotation && transformed_rotation <= 203)
			orientation = 2;
		else if(204 <= transformed_rotation && transformed_rotation <= 249)
			orientation = 1;
		else if(250 <= transformed_rotation && transformed_rotation <= 295)
			orientation = 0;
		else
			orientation = 6;*/

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

void preprocess_things(struct mapdef* map, int player_x, int player_y) {
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
	sort_things(map, 0, map->num_things - 1);
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

void cast_rays(SDL_Renderer* renderer, struct mapdef* map, int player_x, int player_y, int player_rot) {
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
	struct draw_wall_slice_args dws;

	int i;

	// Begin by clearning the pixel arrays that we copy to.
	for(i = 0; i < 64000; ++i) {
		floor_ceiling_pixels[i] = 0;
		raycast_pixels[i] = 0;
		thing_pixels[i] = 0;
	}

	preprocess_things(map, player_x, player_y);

	// Now loop through each column of pixels on the screen and do ray casting.
	for(i = 0; i < PROJ_W; ++i) {
		adj_angle = get_adjusted_angle((int)curr_angle);

		// Computes the angle relative to the player rotation.
		correct_angle = abs(adj_angle - player_rot);

		z_buffer[i] = 0;
		get_ray_hit(adj_angle, player_x, player_y, &hit, map);
		if(hit.hit_pos[0] != -1 && hit.hit_pos[1] != -1) {
			// SKY CASTING
			draw_sky(map, i, adj_angle);

			z_buffer[i] = hit.dist;

			// WALL, FLOOR, CEILING CASTING
			dws.map = map;
			dws.hit = &hit;
			dws.correct_angle = correct_angle;
			dws.adj_angle = adj_angle;
			dws.screen_col = i;
			dws.player_x = player_x;
			dws.player_y = player_y;
			draw_wall_slice(&dws);

		}

		curr_angle -= ANGLE_BETWEEN_RAYS;
	}

	// THING CASTING
	draw_things(map, player_x, player_y, player_rot);

	// Draw pixel arrays to screen.
	SDL_UpdateTexture(raycast_texture, NULL, raycast_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, raycast_texture, NULL, NULL);

	SDL_UpdateTexture(floor_ceiling_tex, NULL, floor_ceiling_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, floor_ceiling_tex, NULL, NULL);

	SDL_UpdateTexture(thing_texture, NULL, thing_pixels, PROJ_W << 2);
	SDL_RenderCopy(renderer, thing_texture, NULL, NULL);
}

void sort_things(struct mapdef* map, int s, int e) {
	if(e <= s)
		return;

	int m = partition(map, s, e);

	sort_things(map, s, m);
	sort_things(map, m + 1, e);
}

int partition(struct mapdef* map, int s, int e) {
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
