// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"
#include "raycaster.h"

#include <stdio.h>

// Stores the player 
int player_x, player_y;
// The player rotation.
int player_rot;

// Temporary storage for map.
struct mapdef* map;

/*INITIALIZATION PROCEDURES*/

// TODO: This whole function should be elsewhere.
void initialize_map(struct mapdef* map, SDL_Renderer* renderer) {
	unsigned int temp_layout[200] = {
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
		3,1,1,1,3,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3,
		3,1,1,1,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,3,
		3,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,1,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,2,2,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
	};

	map->map_w = 20;
	map->map_h = 10;
	map->layout = (unsigned int*)malloc(sizeof(unsigned int) * 200);

	int i;
	for(i = 0; i < 200; ++i)
		map->layout[i] = temp_layout[i];

	// Set properties of map.
	map->num_wall_tex = 2;
	map->num_floor_ceils = 3;
	map->num_tiles = map->num_wall_tex + map->num_floor_ceils;
	map->num_things = 4;

	// Load walls into memory.
	map->walls[0].surf = SDL_LoadBMP("./src/assests/wall1.bmp");
	map->walls[1].surf = SDL_LoadBMP("./src/assests/wall2.bmp");

	// Load floor-ceiling pairs into memory.
	map->floor_ceils[0].floor_surf = SDL_LoadBMP("./src/assests/floor.bmp");
	map->floor_ceils[0].ceil_surf = SDL_LoadBMP("./src/assests/ceiling.bmp");

	map->floor_ceils[1].floor_surf = SDL_LoadBMP("./src/assests/floor2.bmp");
	map->floor_ceils[1].ceil_surf = SDL_LoadBMP("./src/assests/ceiling2.bmp");

	map->floor_ceils[2].floor_surf = SDL_LoadBMP("./src/assests/floor.bmp");
	map->floor_ceils[2].ceil_surf = NULL;

	// Initializes the sprites.
	map->things[0].surf = SDL_LoadBMP("./src/assests/sprite.bmp");
	map->things[0].position[0] = 128;
	map->things[0].position[1] = 128;

	map->things[1].surf = SDL_LoadBMP("./src/assests/sprite2.bmp");
	map->things[1].position[0] = 128;
	map->things[1].position[1] = 448;

	map->things[2].surf = SDL_LoadBMP("./src/assests/sprite3.bmp");
	map->things[2].position[0] = 672;
	map->things[2].position[1] = 96;

	map->things[3].surf = SDL_LoadBMP("./src/assests/sprite.bmp");
	map->things[3].position[0] = 256;
	map->things[3].position[1] = 460;


	// Load sky texture into memory.
	map->sky_surf = SDL_LoadBMP("./src/assests/skybox.bmp");

	// Enables transparent pixel 
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void initialize(SDL_Renderer* renderer) {
	player_x = 256;
	player_y = 160;
	player_rot = 180;

	map = (struct mapdef*)malloc(sizeof(struct mapdef));

	// Initializes all the angle lookup tables.
	initialize_lookup_tables();
	// Intializes the rendering textures.
	initialize_render_textures(renderer);
	// Initialize the map data.
	initialize_map(map, renderer);
}

/*UPDATE PROCEDURES*/

void update() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			if(event.key.keysym.sym == SDLK_c)
				exit(0);

			if(event.key.keysym.sym == SDLK_a) {
				player_rot += 2;

				if(player_rot < 0)
					player_rot += 360;
				if(player_rot > 360)
					player_rot -= 360;
			}

			if(event.key.keysym.sym == SDLK_d) {
				player_rot -= 2;

				if(player_rot < 0)
					player_rot += 360;
				if(player_rot > 360)
					player_rot -= 360;
			}

			if(event.key.keysym.sym == SDLK_w) {
				player_y -= (sin128table[player_rot] << 4) >> 7;
				player_x += (cos128table[player_rot] << 4) >> 7;

				if(get_tile(player_x, player_y, map) < 0 || get_tile(player_x, player_y, map) >= 3) {
					player_y += (sin128table[player_rot] << 4) >> 7;
					player_x -= (cos128table[player_rot] << 4) >> 7;
				}
			}

			if(event.key.keysym.sym == SDLK_s) {
				player_y += (sin128table[player_rot] << 4) >> 7;
				player_x -= (cos128table[player_rot] << 4) >> 7;

				if(get_tile(player_x, player_y, map) < 0 || get_tile(player_x, player_y, map) >= 3) {
					player_y -= (sin128table[player_rot] << 4) >> 7;
					player_x += (cos128table[player_rot] << 4) >> 7;
				}
			}

			if(event.key.keysym.sym == SDLK_c) {
				printf("Player position = [%d, %d]. Player rotation = %d\n", player_x, player_y, player_rot);
			}
		}
	}

}

/*RENDERING PROCEDURES*/
void render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);
	// Fills the screen with the current render draw color, which is
	// cornflower blue.
	SDL_RenderClear(renderer);

	cast_rays(renderer, map, player_x, player_y, player_rot);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
