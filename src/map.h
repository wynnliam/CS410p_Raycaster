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
	int color[3];
};

// Define a "Null" walldef for floors.
struct walldef walls[3];

#endif
