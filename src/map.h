// Liam Wynn, 5/12/2018, CS410p: Full Stack Web Development

/*
	Defines the properties of a basic map. For now, this data
	will be constant, and we will just render walls with a single
	color.
*/

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

// Specifies the colors for the walls. For now, they
// are solid. Note that the first entry will be a
// placeholder. 0 denotes "no wall". It is here to make
// the members of the map array line up nicely.
char textures[3][3] = {
	{0,   0, 0},
	{0,   0, 255},
	{255, 0, 0}
};
