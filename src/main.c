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

void tick(UIItem *Item)
{
	static int dx = 1, dy = 1;
	Item->items[0]->X += dx;
	Item->items[0]->Y += dy;
	if (Item->items[0]->X +
	    Item->items[0]->W >= Item->W)
		dx = -1;
	else if (Item->items[0]->X <= 0)
		dx = 1;
	if (Item->items[0]->Y +
	    Item->items[0]->H >= Item->H)
		dy = -1;
	else if (Item->items[0]->Y <= 0)
		dy = 1;
	Item->redraw = true;

	if (Item->Events.items)
	{
		/* TODO - Dispatch to the correct children based on position // focus */
		free(Item->Events.items);
		Item->Events.count =
			Item->Events.capacity = 0;
		Item->Events.items = NULL;
	}
}

int main(void)
{
	/**
	 *	State is shared across the whole program,
	 *	all access must be locked.
	 * */
	UIItem *Root = UICreate(NULL, JES_UITYPE_CONTAINER, 0, 0, 0);
	Root->W = 640;
	Root->H = 480;
	Root->as.Container.ColourRGBA = 0x004400FF;
	Root->redraw = true;
	Root->Tick = tick;

	UIItem *X = UICreate(Root, JES_UITYPE_CONTAINER, 10, 10, 0);
	X->W = 32;
	X->H = 32;
	X->as.Container.ColourRGBA = 0xAAAA00FF;
	X->redraw = true;

	JESState State = {
		.initialX = SDL_WINDOWPOS_CENTERED,
		.initialY = SDL_WINDOWPOS_CENTERED,
		.X = SDL_WINDOWPOS_CENTERED,
		.Y = SDL_WINDOWPOS_CENTERED,
		.initialWidth  = 640,
		.initialHeight = 480,
		.Width = 640,
		.Height = 480,
		.FrameDelay = 8,
		.running  = true,
		.root = Root,
	};
	StartRendering(&State);
	UIFree(Root);
	return 0;
}
