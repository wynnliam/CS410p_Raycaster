// Liam Wynn, 5/12/2018, CS410p: Full Stack Web Development

/*
	Defines data structures and procedures relating to
	animations.

	Let's discuss the nature of animations a little bit here.
	We'll begin by defining a couple of things. We say that
	an animation is a series of frames, and each frame is a 64 x 64
	pixel square component of an overall sprite sheet. So in
	concrete terms, when we render an animation for a sprite, we
	render a square component of a larget image.

	There are a couple of requirements for animations for our system here.
	First, as I stated previously, all animation frames must be 64 by 64 pixels.
	Second, an animation goes from left to right. This means that the first frame
	of an animation sequence is the left most 64 by 64 pixel square. The rightmost
	one is its last.
*/

struct animdef {
	// TODO: Finish me!
	// The number of 64 x 64 pixel square.
	unsigned int num_frames;
	// The time in milliseconds between frames.
	unsigned int frame_time;
	unsigned int curr_frame;
	// If we repeat the animation or not.
	int bRepeats;
	// If the animation is currently running or not.
	int bRunning;

	// When (SDL_GetTicks() - start_tick) >= frame_time, we update
	// our current frame.
	unsigned int start_tick;

	// The position of where this animation sequence starts on the
	// sprite sheet in unit values. That is, 1 is 64 pixels, 2 is 128, etc.
	unsigned char start_x, start_y;
};
