// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

/*
	Contains every function, parameter, and so on for performing
	the raycast algorithm.
*/

#include "map.h"

#ifndef RAYCASTER
#define RAYCASTER

// Field of view, in radians. This specifies the angle of what we can
// view. It is also important for computing the rays themselves.
#define FOV					60
// This value occurs in the raycaster enough that we ought to pre-compute
// it.
#define FOV_HALF			30
// The world is essentially a series of cubes. This value is the size of
// each cube or unit as I call it in pixels.
#define UNIT_SIZE			64
// Essentially, UNIT_SIZE = 2 ^ UNIT_POWER. We use this to do effecient
// multiplication and division.
#define UNIT_POWER			6
// Since we use it frequently, we will define half of the unit size and its
// power here
#define HALF_UNIT_SIZE		32
#define HALF_UNIT_POWER		5
// The size of the projection area, which is our screen. I would call it
// "SCREEN_W and SCREEN_H", but the underlying math refers to them as
// the PROJECTION PLANE, so I will use that nomenclature instead.
#define PROJ_W				320
#define PROJ_H				200
// We use half of the projection area frequently, so we will just compute
// those here.
#define HALF_PROJ_W			160
#define HALF_PROJ_H			100
// Distance from the player to the projection plane. This is calculated
// with some basic trigonometry, but we will have it pre-computed here
#define DIST_TO_PROJ		277
// Essentially the FOV / PROJ_W, but we will pre-compute that here.
#define ANGLE_BETWEEN_RAYS	0.1875
// Multiply a radian value to get its approximated degree.
#define RAD_TO_DEG 			57.296

struct hitinfo {
	// Where we hit.
	int hit_pos[2];
	// The squared distance.
	int dist;
	// The wall texture.
	int wall_type;
	// If true, the intersection was along a horizontal grid.
	// Otherwise, it was vertical.
	int is_horiz;
	// Used when correcting for 'fisheye' lens.
	int quadrant;
};

// Used in the function draw_wall_slice to reduce number of arguments.
struct draw_wall_slice_args {
	// Data for ray hitting wall slice.
	struct hitinfo* hit;
	// Used to compute the "correct" distance from the
	// player to the wall slice so as to avoid the "fish-eye"
	// effect.
	int correct_angle;
	// The angle of the ray adjusted to be from 0 to 360.
	int adj_angle;
	// Where we render the wall slice on the screen.
	int screen_col;
	// Coordinates of the player in "world" space.
	int player_x, player_y;
};

// Stores the sin value of every degree from 0 to 360 multiplied by 128.
// This will enable us to preserve enough precision for each number as
// a byte. When we want a value, we can access sin128table[i] >> 7, which
// undoes the multiplication.
int sin128table[361];
// Stores the cos value of every degree from 0 to 360 multiplied by 128.
int cos128table[361];
// Stores every tan value of every degree from 0 to 360 multiplied by 128.
// The values for 0, 90, 180, 270, and 360 will be -1.
int tan128table[361];

// Stores the value of 1/tan(t) * 128 for every degree between 0 and 360.
// The values for 0, 90, 180, 270, and 360 will be -1.
int tan1table[361];
// Stores the value of 1/sin(t) * 128 for every degree between 0 and 360.
int sin1table[361];

// Stores the delta values for horizontal and vertical increments for each
// angle.
int delta_h_x[361];
int delta_h_y[361];
int delta_v_x[361];
int delta_v_y[361];

// Stores the distance each ray is from the player when it hits something.
int z_buffer[PROJ_W];

// What we render for the floor/ceiling
SDL_Texture* floor_ceiling_tex;
// Where we store the floor and ceiling pixels before we render.
unsigned int floor_ceiling_pixels[64000];

// Where we render for skybox and wall pixels.
SDL_Texture* raycast_texture;
// Where we store sky and wall textures before we render.
unsigned int raycast_pixels[64000];

// What we render for the things.
SDL_Texture* thing_texture;
// Where we store thing pixels before we render.
unsigned int thing_pixels[64000];

// Stores the things in a sorted order.
struct thingdef* things_sorted[1000];

/*
	For angles 0 to 360, computes sin(a) * 128, cos(a) * 128, and tan(a) * 128
	and stores them in the angle lookup tables. For angles 0, 90, 180, 270, 360,
	we use the value -1.

	PRECONDITIONS:
		None

	POSTCONDITIONS:
		All values in sin128table, cos128table, and tan128table are computed.
*/
void initialize_lookup_tables();

/*
	Initializes data for tiles in map. Includes things like textures, skybox,
	sprites, etc.

	PRECONDITIONS:
		None

	POSTCONDITIONS:
		All textures for map loaded and initialized.

	ARGUMENTS:
		renderer - needed to convert surfaces to textures.
*/
void initialize_map(SDL_Renderer* renderer);

/*
	Returns the tile at a given position (x,y). Returns -1 if no tile is at
	that position.

	ARGUMENTS:
		x, y - the position to lookup the tile id for.

	PRECONDITIONS:
		None

	POST CONDITIONS:
		None

	RETURNS:
		-1 if invalid position. Otherwise, the tile at (x,y).
*/
int get_tile(int x, int y);

/*
	Scans through each ray and performs the ray casting algorithm. For each ray,
	we calculate its angle relative to the player, and use that to quickly
	find the wall we want to trace for. Then, we can compute the distance to that
	wall. Once we have that distance, we compute the size of the line we want to render,
	and then render it!

	ARGUMENTS:
		renderer - used to draw lines in SDL.
		player_x, player_y - the position of the player.
		player_rot - the rotation of the player in degrees.

	PRECONDITIONS:
		SDL is initialized, and initialize_lookup_tables is called.

	POSTCONDITIONS:
		A single raycasted scene is rendered.
		
*/
void cast_rays(SDL_Renderer* renderer, int player_x, int player_y, int player_rot);

/*
	Sorts every thing according to distance from the player. This requires updating
	the distance for each thing and then doing quicksort according to distance.

	ARUGMENTS:
		player_x, player_y - the position of the player in "world" space.
*/
void preprocess_things(int player_x, int player_y);

/*
	Computes an angle that is equivalent to the given angle, but adjusted so that
	the raycaster can use it correctly. This includes making sure the angle is between
	0 and 359 (inclusive bounds), and not a "bad" angle: 0, 90, 180, and 270. These
	are bad because they give certain values we cannot divide by or are undefined.

	ARGUMENTS:
		curr_angle: an integer angle (in degrees) we want to adjust.

	RETURNS:
		an equivalent angle to curr_angle that is adjusted.
*/
int get_adjusted_angle(int curr_angle);

/*
	Traces a ray along grid columns to find the first wall it hits. We then return
	this wall slice that is hit so as to render it.

	ARGUMENTS:
		ray_angle - the angle of the ray as an integer (in degrees). We use an imprecise
		value so as to do our lookup's quickly.
		player_x, player_y - where the ray will originate from.
		hit - stores all of the data about this hit.
*/
void get_ray_hit(int ray_angle, int player_x, int player_y, struct hitinfo* hit);

/*
	Renders the sky at a given column of pixels. Given the adjusted angle,
	determine the column of texture pixels of the sky texture, and then render them
	onto the screen.

	ARGUMENTS:
		screen_col - the column on the screen we want to draw sky pixels to.
		adj_angle - used to figure out what column of sky pixels to render.
*/
void draw_sky(int screen_col, int adj_angle);

/*
	Renders a single wall slice for a given ray. When this procedure is called,
	we assume the ray actually hit a wall.

	ARGUMENTES:
		args - Please see the draw_wall_slice struct definition above.
*/
void draw_wall_slice(struct draw_wall_slice_args* args);

/*
	Renders the wall and floor for a given column of pixels. This is called by draw_wall_slice.

	ARGUMENTS:
		screen_slice_y - The y position for a slice on the screen.
		screen_slice_h - The height of the slice on the screen.
		dws - We use quite a few of the values stored in the draw_wall_slice_args.
*/
void draw_floor_and_ceiling(int screen_slice_y, int screen_slice_h, struct draw_wall_slice_args* dws);

/*
	Renders every thing (that we can see). We render them from the furthest to the player to
	the closest. To render them, we do much the same as we would for walls. That is,
	we render each thing column by column. We only render the ones we can see. That is, only render
	the columns of sprites where the corresponding value in the z-buffer is larger (this implies the
	wall slice of a given column is further away).

	ARGUMENTS:
		player_x, player_y - the position of the player in "world" space.
		player_rot - the rotation of the player in degrees.
*/
void draw_things(int player_x, int player_y, int player_rot);

/*
	Grabs the pixel of a surface at point (x, y). If the point is within the dimensions
	of the surface, we return 0. This procedure will construct the unsigned integer
	representation of the pixel value according to the bytes per pixel and the endian-ness
	of SDL.

	The algorithm to handle this was originally implemented here:
	http://sdl.beuc.net/sdl.wiki/Pixel_Access	

	PRECONDITIONS:
		surface loaded into memory.

	POSTCONDITIONS:
		none.

	ARGUMENTS:
		surface - the surface to grab pixels from.
		x, y - the pixel to access.

	RETURNS:
		an unsigned integer representation of the pixel value of the surface
		at (x, y) or 0, if (x, y) is invalid (or surface is NULL).
*/
unsigned int get_pixel(SDL_Surface* surface, int x, int y);

/*
	Sortes every sprite according to distance from the player. We must order the sprites from
	farthest to closest so we render the closest ones last. The algorithm used here is quicksort.

	ARGUMENTS:
		s, e - the starting and ending indecies

	PRECONDITIONS:
		initialize_map was called

	POSTCONDITIONS:
		Order of things array will be modifief.
*/
void sort_things(int s, int e);
int partition(int s, int e);
#endif
