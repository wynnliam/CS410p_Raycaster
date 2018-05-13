// Liam Wynn, 5/12/2018, CS410p: Full Stack Web Development

/*
	Contains a series of parameters, constants, and pre-computed values
	making the raycast efficient.
*/

// Field of view, in radians. This specifies the angle of what we can
// view. It is also important for computing the rays themselves.
#define FOV					1.05
// This value occurs in the raycaster enough that we ought to pre-compute
// it.
#define FOV_HALF			0.525
// The world is essentially a series of cubes. This value is the size of
// each cube or unit as I call it in pixels.
#define UNIT_SIZE			64
// The size of the projection area, which is our screen. I would call it
// "SCREEN_W and SCREEN_H", but the underlying math refers to them as
// the PROJECTION PLANE, so I will use that nomenclature instead.
#define PROJ_W				320
#define PROJ_H				200
// Distance from the player to the projection plane. This is calculated
// with some basic trigonometry, but we will have it pre-computed here
#define DIST_TO_PROJ		277
// Essentially the FOV / PROJ_W, but we will pre-compute that here.
#define ANGLE_BETWEEN_RAYS	0.03

// Holds the angle of each ray (there are projection plane width number of them).
// The value stored here is the angle if the player has a rotation of 0. This way,
// we need only add the player's rotation to these values. They are computed in
// the initialize function.
float ray_angles[PROJ_W];
