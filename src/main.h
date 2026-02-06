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

typedef struct STRING
{
        char *items;
	size_t count, capacity;
} STRING;

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

#define foreachr(iter, ar, body)		      \
	do					   \
	{					    \
		size_t __i;			  \
		typeof(ar->items[0]) *iter;	    \
		for (__i = 0; __i < ar->count; ++__i) \
		{				    \
			iter = &ar->items[__i];	\
			body			 \
		}				    \
	} while (0);

#define da_append(xs, x)									   \
	do											 \
	{											  \
		if ((xs)->count >= (xs)->capacity)						 \
		{										  \
			if ((xs)->capacity == 0)						   \
				(xs)->capacity = 256;					      \
			else								       \
				(xs)->capacity *= 2;					       \
			(xs)->items = realloc((xs)->items, (xs)->capacity * sizeof(*(xs)->items)); \
		}										  \
												   \
		(xs)->items[(xs)->count++] = (x);						  \
	} while (0)

#define da_find(xs, x, res)				    \
	do						     \
	{						      \
		res = -1;				      \
		for (size_t _n_ = 0; _n_ < (xs)->count; ++_n_) \
			if ((xs)->items[_n_] == x)	     \
				res = _n_;		     \
	} while (0)

#define da_free(xs)			       \
	do					\
	{					 \
		free((xs)->items);		\
		(xs)->items = NULL;	       \
		(xs)->count = (xs)->capacity = 0; \
	} while (0)

#endif
