// Liam Wynn, 6/26/2018, CS410p: Full Stack Web Development

#include "animation.h"
#include <SDL2/SDL.h>

void start_anim(struct animdef* anim) {
	if(!anim || anim->bRunning)
		return;

	anim->curr_frame = 0;
	anim->bRunning = 1;
	anim->start_tick = SDL_GetTicks();
}

void update_anim(struct animdef* anim) {
	if(!anim || !anim->bRunning)
		return;

	unsigned int curr_tick = SDL_GetTicks();

	// It's time to update.
	if((curr_tick - anim->start_tick) >= anim->frame_time) {
		// If we are at the last frame, check if we repeat.
		if(anim->curr_frame == anim->num_frames - 1) {
			// If not, we freeze, as the animation is done.
			if(anim->bRepeats)
				anim->curr_frame = 0;
			else
				stop_anim(anim);
		}

		else
			++anim->curr_frame;

		anim->start_tick = curr_tick;
	}
}

void stop_anim(struct animdef* anim) {
	if(!anim || !anim->bRunning)
		return;

	anim->bRunning = 0;
}
