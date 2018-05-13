// Liam Wynn, 5/13/2018, CS410p: Full Stack Web Development

/*
	Contains every function, parameter, and so on for performing
	the raycast algorithm.
*/


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
// The size of the projection area, which is our screen. I would call it
// "SCREEN_W and SCREEN_H", but the underlying math refers to them as
// the PROJECTION PLANE, so I will use that nomenclature instead.
#define PROJ_W				320
#define PROJ_H				200
// Distance from the player to the projection plane. This is calculated
// with some basic trigonometry, but we will have it pre-computed here
#define DIST_TO_PROJ		277
// Essentially the FOV / PROJ_W, but we will pre-compute that here.
#define ANGLE_BETWEEN_RAYS	0.1875

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
