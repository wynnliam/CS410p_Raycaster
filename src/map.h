// Liam Wynn, 5/12/2018, CS410p: Full Stack Web Development

/*
	Defines the properties of a basic map. For now, this data
	will be constant, and we will just render walls with a single
	color.
*/

#ifndef MAP
#define MAP

#include <SDL2/SDL.h>
#include "animation.h"

#define MAP_W	20
#define MAP_H	10

// Specifies the data for a given wall tile.
struct walldef {
	// Defines the texture for this wall.
	SDL_Surface* surf;
};

// Specifies floor and ceiling textures for a given
// point. If a texture is NULL, we do not render it.
struct floorcielingdef {
	SDL_Surface* floor_surf;
	SDL_Surface* ceil_surf;
};

// Defines a "thing" in the world. This is for
// static objects in the world, but can easily
// be extended for dynamic objects and/or NPCs.
struct thingdef {
	// The texture to render.
	SDL_Surface* surf;
	struct animdef anims[100];
	// The global position.
	int position[2];
	// The global rotation in degrees.
	int rotation;
	// The distance from the player.
	int dist;

	unsigned int num_anims;
	unsigned int curr_anim;
};

struct mapdef {
	// Specifies the layout of the world.
	unsigned int* layout;

	// Assume an upper bound of 100 wall textures.
	struct walldef walls[100];
	// Assume also an upper bound of 100 floor and ceiling textures.
	struct floorcielingdef floor_ceils[100];

	// Stores the sky texture used for the map.
	SDL_Surface* sky_surf;

	// The list of sprites in the world. We sort
	// this every frame by distance from the player.
	// We will assume there can be at most 1000 sprites
	// in a level.
	struct thingdef things[1000];

	// Specifies the dimensions of the world.
	unsigned int map_w, map_h;

	// Stores the number of tile types (num_wall_tex + num_foor_ceils)
	unsigned int num_tiles;
	// Stores the number of wall textures used by the system.
	unsigned int num_wall_tex;
	// Stores the number of floor ceiling pairs used in the map.
	unsigned int num_floor_ceils;

	// Stores the number of things in the world.
	unsigned int num_things;
};

// Specifies the layout of the world.
/*int map[200] = {
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
};*/
#endif
