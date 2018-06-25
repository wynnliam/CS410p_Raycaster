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
	/*map->map_w = 20;
	map->map_h = 10;
	unsigned int temp_layout[200] = {
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,5,3,6,3,3,
		3,1,1,1,3,0,0,0,0,3,0,0,0,0,0,0,0,0,0,3,
		3,1,1,1,0,0,0,0,0,3,3,0,0,0,0,0,0,0,0,3,
		3,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,1,1,1,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,2,2,2,4,0,0,0,0,0,0,0,0,0,0,1,0,0,0,3,
		4,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
	};*/

	map->map_w = 40;
	map->map_h = 24;
	int len = map->map_w * map->map_h;
	unsigned int temp_layout[960] = {
		100,100,101,100,102,100,103,104,100,105,100,106,100,107,100,108,109,100,110,100,111,112,100,113,100,114,100,115,100,116,100,117,100,118,119,100,120,100,100,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,100,
		123,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,121,
		124,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,122,
		100,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,100,
		127,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,125,
		100,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,126,
		128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,136,
		129,  0,  0,  0,  0,  0,  0,  0,  0,100,149,100,150,100,151,100,152,100,153,100,154,100,155,100,156,100,157,100,158,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,159,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,137,
		130,  0,  0,  0,  0,  0,  0,  0,  0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,160,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,138,
		131,  0,  0,  0,  0,  0,  0,  0,  0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,161,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,139,
		132,  0,  0,  0,  0,  0,  0,  0,  0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,162,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,140,
		133,  0,  0,  0,  0,  0,  0,  0,  0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,163,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,141,
		134,  0,  0,  0,  0,  0,  0,  0,  0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,164,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,142,
		135,  0,  0,  0,  0,  0,  0,  0,  0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,  0,  0,  0,  0,  0,  0,  0,  0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,  0,  0,  0,  0,  0,  0,  0,  0,100,
		100,100,143,100,144,100,145,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,146,100,147,100,148,100,100,100,
	};

	map->layout = (unsigned int*)malloc(sizeof(unsigned int) * len);

	int i;
	for(i = 0; i < len; ++i)
		map->layout[i] = temp_layout[i];

	// Set properties of map.
	map->num_wall_tex = 100;
	map->num_floor_ceils = 100;
	map->num_tiles = map->num_wall_tex + map->num_floor_ceils;
	map->num_things = 4;

	// Load walls into memory.
	map->walls[0].surf = SDL_LoadBMP("./src/assests/textures/test/wall1.bmp");

	map->walls[1].surf = SDL_LoadBMP("./src/assests/textures/art/art1.bmp");
	map->walls[2].surf = SDL_LoadBMP("./src/assests/textures/art/art2.bmp");
	map->walls[3].surf = SDL_LoadBMP("./src/assests/textures/art/art3a.bmp");
	map->walls[4].surf = SDL_LoadBMP("./src/assests/textures/art/art3b.bmp");
	map->walls[5].surf = SDL_LoadBMP("./src/assests/textures/art/art4.bmp");

	map->walls[6].surf = SDL_LoadBMP("./src/assests/textures/art/art5.bmp");
	map->walls[7].surf = SDL_LoadBMP("./src/assests/textures/art/art6.bmp");
	map->walls[8].surf = SDL_LoadBMP("./src/assests/textures/art/art7a.bmp");
	map->walls[9].surf = SDL_LoadBMP("./src/assests/textures/art/art7b.bmp");
	map->walls[10].surf = SDL_LoadBMP("./src/assests/textures/art/art8.bmp");

	map->walls[11].surf = SDL_LoadBMP("./src/assests/textures/art/art9a.bmp");
	map->walls[12].surf = SDL_LoadBMP("./src/assests/textures/art/art9b.bmp");
	map->walls[13].surf = SDL_LoadBMP("./src/assests/textures/art/art10.bmp");
	map->walls[14].surf = SDL_LoadBMP("./src/assests/textures/art/art11.bmp");
	map->walls[15].surf = SDL_LoadBMP("./src/assests/textures/art/art13.bmp");

	map->walls[16].surf = SDL_LoadBMP("./src/assests/textures/art/art14.bmp");
	map->walls[17].surf = SDL_LoadBMP("./src/assests/textures/art/art15.bmp");
	map->walls[18].surf = SDL_LoadBMP("./src/assests/textures/art/art16a.bmp");
	map->walls[19].surf = SDL_LoadBMP("./src/assests/textures/art/art16b.bmp");
	map->walls[20].surf = SDL_LoadBMP("./src/assests/textures/art/art17.bmp");

	map->walls[21].surf = SDL_LoadBMP("./src/assests/textures/art/art20a.bmp");
	map->walls[22].surf = SDL_LoadBMP("./src/assests/textures/art/art20b.bmp");

	map->walls[23].surf = SDL_LoadBMP("./src/assests/textures/art/art47a.bmp");
	map->walls[24].surf = SDL_LoadBMP("./src/assests/textures/art/art47b.bmp");
	map->walls[25].surf = SDL_LoadBMP("./src/assests/textures/art/art44.bmp");
	map->walls[26].surf = SDL_LoadBMP("./src/assests/textures/art/art45.bmp");

	map->walls[27].surf = SDL_LoadBMP("./src/assests/textures/art/art18.bmp");
	map->walls[28].surf = SDL_LoadBMP("./src/assests/textures/art/art19.bmp");
	map->walls[29].surf = SDL_LoadBMP("./src/assests/textures/art/art21.bmp");
	map->walls[30].surf = SDL_LoadBMP("./src/assests/textures/art/art22.bmp");
	map->walls[31].surf = SDL_LoadBMP("./src/assests/textures/art/art23.bmp");

	map->walls[32].surf = SDL_LoadBMP("./src/assests/textures/art/art24.bmp");
	map->walls[33].surf = SDL_LoadBMP("./src/assests/textures/art/art25.bmp");
	map->walls[34].surf = SDL_LoadBMP("./src/assests/textures/art/art26.bmp");
	map->walls[35].surf = SDL_LoadBMP("./src/assests/textures/art/art28.bmp");
	map->walls[36].surf = SDL_LoadBMP("./src/assests/textures/art/art29.bmp");

	map->walls[37].surf = SDL_LoadBMP("./src/assests/textures/art/art30.bmp");
	map->walls[38].surf = SDL_LoadBMP("./src/assests/textures/art/art31.bmp");
	map->walls[39].surf = SDL_LoadBMP("./src/assests/textures/art/art32.bmp");
	map->walls[40].surf = SDL_LoadBMP("./src/assests/textures/art/art33.bmp");

	map->walls[41].surf = SDL_LoadBMP("./src/assests/textures/art/art34.bmp");
	map->walls[42].surf = SDL_LoadBMP("./src/assests/textures/art/art35.bmp");
	map->walls[43].surf = SDL_LoadBMP("./src/assests/textures/art/art36.bmp");
	map->walls[44].surf = SDL_LoadBMP("./src/assests/textures/art/art38.bmp");
	map->walls[45].surf = SDL_LoadBMP("./src/assests/textures/art/art39.bmp");

	map->walls[46].surf = SDL_LoadBMP("./src/assests/textures/art/art40.bmp");
	map->walls[47].surf = SDL_LoadBMP("./src/assests/textures/art/art41.bmp");
	map->walls[48].surf = SDL_LoadBMP("./src/assests/textures/art/art42.bmp");

	map->walls[49].surf = SDL_LoadBMP("./src/assests/textures/art/art43.bmp");
	map->walls[50].surf = SDL_LoadBMP("./src/assests/textures/art/art48.bmp");
	map->walls[51].surf = SDL_LoadBMP("./src/assests/textures/art/art49.bmp");
	map->walls[52].surf = SDL_LoadBMP("./src/assests/textures/art/art50.bmp");
	map->walls[53].surf = SDL_LoadBMP("./src/assests/textures/art/art51.bmp");

	map->walls[54].surf = SDL_LoadBMP("./src/assests/textures/art/art52.bmp");
	map->walls[55].surf = SDL_LoadBMP("./src/assests/textures/art/art53.bmp");
	map->walls[56].surf = SDL_LoadBMP("./src/assests/textures/art/art54.bmp");
	map->walls[57].surf = SDL_LoadBMP("./src/assests/textures/art/art55.bmp");
	map->walls[58].surf = SDL_LoadBMP("./src/assests/textures/art/art56.bmp");

	map->walls[59].surf = SDL_LoadBMP("./src/assests/textures/art/art57.bmp");
	map->walls[60].surf = SDL_LoadBMP("./src/assests/textures/art/art58.bmp");
	map->walls[61].surf = SDL_LoadBMP("./src/assests/textures/art/art59.bmp");
	map->walls[62].surf = SDL_LoadBMP("./src/assests/textures/art/art60.bmp");
	map->walls[63].surf = SDL_LoadBMP("./src/assests/textures/art/art61.bmp");
	map->walls[64].surf = SDL_LoadBMP("./src/assests/textures/art/ctm1.bmp");

	// Load floor-ceiling pairs into memory.
	map->floor_ceils[0].floor_surf = SDL_LoadBMP("./src/assests/textures/test/floor.bmp");
	//map->floor_ceils[0].ceil_surf = SDL_LoadBMP("./src/assests/textures/test/ceiling.bmp");
	map->floor_ceils[0].ceil_surf = NULL;

	map->floor_ceils[1].floor_surf = SDL_LoadBMP("./src/assests/textures/test/floor2.bmp");
	map->floor_ceils[1].ceil_surf = SDL_LoadBMP("./src/assests/textures/test/ceiling2.bmp");

	map->floor_ceils[2].floor_surf = SDL_LoadBMP("./src/assests/textures/test/floor.bmp");
	map->floor_ceils[2].ceil_surf = NULL;

	// Initializes the sprites.
	map->things[0].surf = SDL_LoadBMP("./src/assests/textures/test/sprite.bmp");
	map->things[0].position[0] = 128;
	map->things[0].position[1] = 128;

	map->things[1].surf = SDL_LoadBMP("./src/assests/textures/test/sprite2.bmp");
	map->things[1].position[0] = 128;
	map->things[1].position[1] = 448;

	map->things[2].surf = SDL_LoadBMP("./src/assests/textures/test/sprite3.bmp");
	map->things[2].position[0] = 672;
	map->things[2].position[1] = 96;

	map->things[3].surf = SDL_LoadBMP("./src/assests/textures/test/sprite.bmp");
	map->things[3].position[0] = 256;
	map->things[3].position[1] = 460;


	// Load sky texture into memory.
	map->sky_surf = SDL_LoadBMP("./src/assests/textures/skybox/sky1.bmp");

	// Enables transparent pixel 
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void initialize(SDL_Renderer* renderer) {
	player_x = 158;
	player_y = 500;
	player_rot = 0;

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
			if(event.key.keysym.sym == SDLK_p)
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

				if(get_tile(player_x, player_y, map) < 0 || get_tile(player_x, player_y, map) >= map->num_floor_ceils) {
					player_y += (sin128table[player_rot] << 4) >> 7;
					player_x -= (cos128table[player_rot] << 4) >> 7;
				}
			}

			if(event.key.keysym.sym == SDLK_s) {
				player_y += (sin128table[player_rot] << 4) >> 7;
				player_x -= (cos128table[player_rot] << 4) >> 7;

				if(get_tile(player_x, player_y, map) < 0 || get_tile(player_x, player_y, map) >= map->num_floor_ceils) {
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
