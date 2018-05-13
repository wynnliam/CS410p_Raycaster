// Liam Wynn, 5/11/2018, CS410p: Full Stack Web Development

/*
	These functions are the meat and potatos of the system.
	It is where we handle the main rendering steps (initialize,
	update, and render). Pretty much all of the action is handled
	here.
*/

#ifndef LOOP_HANDLER
#define LOOP_HANDLER
#include <SDL2/SDL.h>

// Handles any pre-loop logic we need.
void initialize();

// Handles any non-rendering logic we need
// per frame.
void update();

// Handles per-frame rendering logic.
void render(SDL_Renderer* renderer);
#endif
