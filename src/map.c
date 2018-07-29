// Liam Wynn, 7/28/2018, CS410p: Full Stack Web Development

#include "map.h"

struct map_bounds {
	int x_min, y_min, x_max, y_max;
};

/*
	From the list of components that define a map, return rectangular bounds
	such that all components would be enclosed in them.
*/
int calculate_map_bounds(struct component* components, struct map_bounds* bounds);

/*
	Builds the wall and floor/ceiling texture lists for the map, given the components.
*/
int build_texture_lists(struct component* components, struct mapdef* map);

int find_fc_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100]);
int find_wall_tex(char* tex_0, struct walldef walls[100]);

int tex_match(char* mapdef_tex, char* map_data_tex);

int add_floor_ceil_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100], int index);
int add_wall_tex(char* tex_0, struct walldef walls[100], int index);

/*
	Adds each component into the map level.
*/
int place_components_into_mapdef(struct mapdef* map, struct component* components);

int build_mapdef_from_map_data(struct mapdef* mapdef, struct map_data* map_data, int* player_x, int* player_y) {
	struct map_bounds bounds;

	if(!calculate_map_bounds(map_data->component_head, &bounds))
		return 0;

	mapdef->map_w = bounds.x_max - bounds.x_min;
	mapdef->map_h = bounds.y_max - bounds.y_min;
	mapdef->layout = (unsigned int*)malloc(mapdef->map_w * mapdef->map_h * sizeof(unsigned int));


	int i;
	for(i = 0; i < 100; ++i) {
		mapdef->walls[i].path = NULL;
		mapdef->floor_ceils[i].floor_path = NULL;
		mapdef->floor_ceils[i].ceil_path = NULL;
	}

	mapdef->num_wall_tex = 100;
	mapdef->num_floor_ceils = 100;
	mapdef->num_tiles = mapdef->num_wall_tex + mapdef->num_floor_ceils;

	build_texture_lists(map_data->component_head, mapdef);
	place_components_into_mapdef(mapdef, map_data->component_head);

	// TODO: Everything below here is unfinished!
	// TODO: Add things
	mapdef->sky_surf = SDL_LoadBMP("./src/assests/textures/skybox/sky1.bmp");

	*player_x = 64;
	*player_y = 64;

	return 1;
}

int calculate_map_bounds(struct component* components, struct map_bounds* bounds) {
	if(!components || !bounds)
		return 0;

	struct component* curr = components;

	bounds->x_min = curr->x;
	bounds->y_min = curr->y;
	// Adding width and height accounts for additional
	// area that furthest components occupy. The rule is that
	// components grow rightwards and downwards as we add to their
	// perimeter.
	bounds->x_max = curr->x + curr->w;
	bounds->y_max = curr->y + curr->h;

	while(curr) {
		if(curr->x < bounds->x_min)
			bounds->x_min = curr->x;
		if(curr->y < bounds->y_min)
			bounds->y_min = curr->y;

		if((curr->x + curr->w) > bounds->x_max)
			bounds->x_max = curr->x + curr->w;
		if((curr->y + curr->h) > bounds->y_max)
			bounds->y_max = curr->y + curr->h;

		curr = curr->next;
	}

	return 1;
}

int build_texture_lists(struct component* components, struct mapdef* map) {
	if(!components || !map)
		return 0;

	struct component* curr = components;
	int num_wall_tex = 0;
	int num_floor_ceil_tex = 0;

	while(curr) {
		if(curr->is_floor_ceil) {
			if(find_fc_tex(curr->tex_0, curr->tex_1, map->floor_ceils) == -1) {
				add_floor_ceil_tex(curr->tex_0, curr->tex_1, map->floor_ceils, num_floor_ceil_tex);
				++num_floor_ceil_tex;
			}
		}

		else {
			if(find_wall_tex(curr->tex_0, map->walls) == -1) {
				add_wall_tex(curr->tex_0, map->walls, num_wall_tex);
				++num_wall_tex;
			}
		}

		curr = curr->next;
	}

	return 1;
}

int find_fc_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100]) {
	int i;

	for(i = 0; i < 100; ++i) {
		if(tex_match(floor_ceils[i].floor_path, tex_0) && tex_match(floor_ceils[i].ceil_path, tex_1))
			return i;
	}

	return -1;
}

int find_wall_tex(char* tex_0, struct walldef walls[100]) {
	int i;

	for(i = 0; i < 100; ++i) {
		if(tex_match(walls[i].path, tex_0))
			return i;
	}

	return -1;
}

int tex_match(char* mapdef_tex, char* map_data_tex) {
	if(map_data_tex == NULL)
		return 0;
	else if(mapdef_tex == NULL && strcmp(map_data_tex, "0") != 0)
		return 0;
	else if(mapdef_tex == NULL && strcmp(map_data_tex, "0") == 0)
		return 1;
	else if(strcmp(mapdef_tex, map_data_tex) == 0)
		return 1;
	else
		return 0;
}

int add_floor_ceil_tex(char* tex_0, char* tex_1, struct floorcielingdef floor_ceils[100], int index) {
	if(strcmp(tex_0, "0") == 0) {
		floor_ceils[index].floor_path = NULL;
		floor_ceils[index].floor_surf = NULL;
	}

	else {
		floor_ceils[index].floor_path = (char*)malloc(strlen(tex_0) + 1);
		strcpy(floor_ceils[index].floor_path, tex_0);
		floor_ceils[index].floor_surf = SDL_LoadBMP(tex_0);
	}

	if(strcmp(tex_1, "0") == 0) {
		floor_ceils[index].ceil_path = NULL;
		floor_ceils[index].ceil_surf = NULL;
	}

	else {
		floor_ceils[index].ceil_path = (char*)malloc(strlen(tex_1) + 1);
		strcpy(floor_ceils[index].ceil_path, tex_1);
		floor_ceils[index].ceil_surf = SDL_LoadBMP(tex_1);
	}

	return 1;
}

int add_wall_tex(char* tex_0, struct walldef walls[100], int index) {
	if(strcmp(tex_0, "0") == 0) {
		walls[index].path = NULL;
		walls[index].path = NULL;
	}

	else {
		walls[index].path = (char*)malloc(strlen(tex_0) + 1);
		strcpy(walls[index].path, tex_0);
		walls[index].surf = SDL_LoadBMP(tex_0);
	}

	return 1;
}

int place_components_into_mapdef(struct mapdef* map, struct component* components) {
	if(!components || !map)
		return 0;

	struct component* curr = components;
	int x, y;
	unsigned int val;

	while(curr) {
		val = curr->is_floor_ceil ? find_fc_tex(curr->tex_0, curr->tex_1, map->floor_ceils) :
									find_wall_tex(curr->tex_0, map->walls) + 100;

		for(x = curr->x; x < curr->x + curr->w; ++x) {
			for(y = curr->y; y < curr->y + curr->h; ++y) {
				if(x < 0 || x >= map->map_w || y < 0 || y >= map->map_h)
					continue;

				map->layout[y * map->map_w + x] = val;
			}
		}

		curr = curr->next;
	}

	return 1;
}
