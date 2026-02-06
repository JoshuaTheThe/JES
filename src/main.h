#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

struct UIItem;
typedef struct UIItem UIItem;

typedef struct JESState
{
	UIItem *root;
	size_t count, capacity;
	const char *title;
	const int32_t initialWidth, initialHeight;
	const int32_t initialX, initialY;
	int32_t Width, Height;
	int32_t X, Y, FrameDelay;
	int32_t MouseX, MouseY, MouseB;

	bool running;
	SDL_Renderer *Renderer;
	SDL_Window *Window;
} JESState;

extern pthread_mutex_t lock;

#define foreach(iter, ar, body)		      \
	do					   \
	{					    \
		size_t __i;			  \
		typeof(ar->items[0]) iter;	    \
		for (__i = 0; __i < ar->count; ++__i) \
		{				    \
			iter = ar->items[__i];	\
			body			 \
		}				    \
	} while (0);

#endif
