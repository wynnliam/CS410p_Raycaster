// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

#include "loop_handling.h"
#include "raycaster.h"

#include <stdio.h>

// Stores the player 
int player_x, player_y;
// The player rotation.
int player_rot;

/*INITIALIZATION PROCEDURES*/

// TODO: This whole function should be elsewhere.
void initialize_map(SDL_Renderer* renderer) {

	// Set properties of map.
	num_wall_tex = 2;
	num_floor_ceils = 3;
	num_tiles = num_wall_tex + num_floor_ceils;

	num_things = 4;

	// Load walls into memory.
	walls[0].surf = SDL_LoadBMP("./src/assests/wall1.bmp");
	walls[1].surf = SDL_LoadBMP("./src/assests/wall2.bmp");

	// Load floor-ceiling pairs into memory.
	floor_ceils[0].floor_surf = SDL_LoadBMP("./src/assests/floor.bmp");
	floor_ceils[0].ceil_surf = SDL_LoadBMP("./src/assests/ceiling.bmp");

	floor_ceils[1].floor_surf = SDL_LoadBMP("./src/assests/floor2.bmp");
	floor_ceils[1].ceil_surf = SDL_LoadBMP("./src/assests/ceiling2.bmp");

	floor_ceils[2].floor_surf = SDL_LoadBMP("./src/assests/floor.bmp");
	floor_ceils[2].ceil_surf = NULL;

	// Initializes the sprites.
	things[0].surf = SDL_LoadBMP("./src/assests/sprite.bmp");
	things[0].position[0] = 128;
	things[0].position[1] = 128;

	things[1].surf = SDL_LoadBMP("./src/assests/sprite2.bmp");
	things[1].position[0] = 128;
	things[1].position[1] = 448;

	things[2].surf = SDL_LoadBMP("./src/assests/sprite3.bmp");
	things[2].position[0] = 672;
	things[2].position[1] = 96;

	things[3].surf = SDL_LoadBMP("./src/assests/sprite.bmp");
	things[3].position[0] = 256;
	things[3].position[1] = 460;

	floor_ceiling_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
	raycast_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);
	thing_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 320, 200);

	// Load sky texture into memory.
	sky_surf = SDL_LoadBMP("./src/assests/skybox.bmp");

	// Enables transparent pixel 
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(floor_ceiling_tex, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(raycast_texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(thing_texture, SDL_BLENDMODE_BLEND);
}

void initialize(SDL_Renderer* renderer) {
	player_x = 256;
	player_y = 160;
	player_rot = 180;

	// Initializes all the angle lookup tables.
	initialize_lookup_tables();
	// Initialize the map data.
	initialize_map(renderer);
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

				if(get_tile(player_x, player_y) < 0 || get_tile(player_x, player_y) >= 3) {
					player_y += (sin128table[player_rot] << 4) >> 7;
					player_x -= (cos128table[player_rot] << 4) >> 7;
				}
			}

			if(event.key.keysym.sym == SDLK_s) {
				player_y += (sin128table[player_rot] << 4) >> 7;
				player_x -= (cos128table[player_rot] << 4) >> 7;

				if(get_tile(player_x, player_y) < 0 || get_tile(player_x, player_y) >= 3) {
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

	cast_rays(renderer, player_x, player_y, player_rot);

	// Forces the screen to be updated.
	SDL_RenderPresent(renderer);
}
