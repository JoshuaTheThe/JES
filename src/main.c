#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include <main.h>
#include <ui/main.h>
#include <ui/render.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
	/**
	 *	State is shared across the whole program,
	 *	all access must be locked.
	 * */
	JESState State = {
		.initialX = SDL_WINDOWPOS_CENTERED,
		.initialY = SDL_WINDOWPOS_CENTERED,
		.X = SDL_WINDOWPOS_CENTERED,
		.Y = SDL_WINDOWPOS_CENTERED,
		.initialWidth  = 640,
		.initialHeight = 480,
		.Width = 640,
		.Height = 480,
		.redraw = true,
		.running  = true,
	};
	StartRendering(&State);
	return 0;
}
