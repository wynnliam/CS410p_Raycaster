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

// Specifies the data for a given wall tile.
struct walldef {
	// Defines the texture for this wall.
	SDL_Texture* texture;
	// The color to render for a wall.
	// TODO: Nix this!
	int color[3];
};

// The floor texture loaded into memory.
SDL_Surface* floor_surf;
// The ceiling texture loaded into memory.
SDL_Surface* ceiling_surf;
// What we render for the floor.
SDL_Texture* floor_tex;
// Where we store the floor pixels before we render.
unsigned int floor_pixels[64000];
// Where we draw the floor rectangle.
SDL_Rect floor_rect;

// Define a "Null" walldef for floors.
struct walldef walls[3];

#endif
