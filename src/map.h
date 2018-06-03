// Liam Wynn, 5/12/2018, CS410p: Full Stack Web Development

/*
	Defines the properties of a basic map. For now, this data
	will be constant, and we will just render walls with a single
	color.
*/

#ifndef MAP
#define MAP

#include <SDL2/SDL.h>

#define MAP_W	20
#define MAP_H	10

// Specifies the data for a given wall tile.
struct walldef {
	// Defines the texture for this wall.
	SDL_Texture* texture;
};

// Defines a "thing" in the world. This is for
// static objects in the world, but can easily
// be extended for dynamic objects and/or NPCs.
struct thingdef {
	SDL_Texture* texture;
	int position[2];
	int dist;
	int can_see;
};

// The floor texture loaded into memory.
SDL_Surface* floor_surf;
// The ceiling texture loaded into memory.
SDL_Surface* ceiling_surf;
// What we render for the floor/ceiling
SDL_Texture* floor_ceiling_tex;
// Where we store the floor pixels before we render.
unsigned int floor_ceiling_pixels[64000];

// Specifies the layout of the world.
int map[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

// Define a "Null" walldef for floors.
struct walldef walls[3];

// The list of sprites in the world. We sort
// this every frame by distance from the player.
struct thingdef things[3];

// Stores the things in a sorted order.
struct thingdef* things_sorted[3];

#endif
