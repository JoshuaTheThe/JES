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
	size_t targetY = Item->items[0]->Y - Item->items[2]->H / 2;
	size_t speed = 1;
	if (Item->items[2]->Y < targetY)
		Item->items[2]->Y += speed;
	else if (Item->items[2]->Y > targetY)
		Item->items[2]->Y -= speed;
	Item->items[2]->X = Item->W - 10 - Item->items[2]->W;
	Item->items[1]->X = 10;

	if (Item->Events.items)
	{
		/* TODO - Dispatch to the correct children based on position // focus */

		for (size_t i = 0; i < Item->Events.count; ++i)
		{
			if (Item->Events.items[i].type == SDL_MOUSEMOTION)
			{
				Item->items[1]->Y = Item->Events.items[i].motion.y;
			}
		}
		free(Item->Events.items);
		Item->Events.count =
			Item->Events.capacity = 0;
		Item->Events.items = NULL;
	}
}

void ball(UIItem *Item)
{
	static int dx = 1, dy = 1;
	static int scoreA = 0, scoreB = 0;

	// Ball -- Wall
	if (Item->X +
	    Item->W >= Item->Parent->W)
	{
		dx = -1;
		scoreA += 1;
		printf("yippee\n");
		Item->X = Item->Parent->W / 2;
		Item->Y = Item->Parent->H / 2;
		Item->Parent->redraw = true;
		free(Item->Parent->items[3]->as.Text.items);
		Item->Parent->items[3]->as.Text.items = NULL;
		Item->Parent->items[3]->as.Text.count =
			Item->Parent->items[3]->as.Text.capacity = 0;
		Item->Parent->items[3]->as.Text.items = malloc(1024);
		Item->Parent->items[3]->redraw = true;
		sprintf(Item->Parent->items[3]->as.Text.items, "%d", scoreA - scoreB);
		return;
	}
	else if (Item->X <= 0)
	{
		dx = 1;
		scoreB += 1;
		printf("oh no\n");
		Item->X = Item->Parent->W / 2;
		Item->Y = Item->Parent->H / 2;
		Item->Parent->redraw = true;
		free(Item->Parent->items[3]->as.Text.items);
		Item->Parent->items[3]->as.Text.items = NULL;
		Item->Parent->items[3]->as.Text.count =
			Item->Parent->items[3]->as.Text.capacity = 0;
		Item->Parent->items[3]->as.Text.items = malloc(1024);
		Item->Parent->items[3]->redraw = true;
		sprintf(Item->Parent->items[3]->as.Text.items, "%d", scoreA - scoreB);
		return;
	}
	if (Item->Y +
	    Item->H >= Item->Parent->H)
		dy = -1;
	else if (Item->Y <= 0)
		dy = 1;
	
	UIItem *Ball = Item,
	       *PaddleA = Item->Parent->items[1],
	       *PaddleB = Item->Parent->items[2];

	// Ball -- PaddleA
	if (Ball->X < PaddleA->X + PaddleA->W &&
	    Ball->X + Ball->W > PaddleA->X &&
	    Ball->Y < PaddleA->Y + PaddleA->H &&
	    Ball->Y + Ball->H > PaddleA->Y)
	{
		dx = abs(dx);
	}

	// Ball -- PaddleB
	if (Ball->X < PaddleB->X + PaddleB->W &&
	    Ball->X + Ball->W > PaddleB->X &&
	    Ball->Y < PaddleB->Y + PaddleB->H &&
	    Ball->Y + Ball->H > PaddleB->Y)
	{
		dx = -abs(dx);
	}

	Item->X += dx;
	Item->Y += dy;
	Item->Parent->redraw = true;
}

void textInit(UIItem *Item)
{
	static bool initialised = false;
	if (!initialised)
	{
		Item->as.Text.Font = TTF_OpenFont("bin/dos.ttf", Item->as.Text.FontSize);
		initialised = true;
	}
}

int main(void)
{
	/**
	 *	State is shared across the whole program,
	 *	all access must be locked.
	 * */

	const int32_t WW = 500, WH = 480, BW = 8, BH = 8,
		      PW = 8,   PH = 64;

	UIItem *Root = UICreate(NULL, JES_UITYPE_CONTAINER, 0, 0, 0);
	Root->W = WW;
	Root->H = WH;
	Root->as.Container.ColourRGBA = 0x004400FF;
	Root->redraw = true;
	Root->Tick = tick;

	UIItem *Ball = UICreate(Root, JES_UITYPE_CONTAINER, WW / 2, WH / 2, 0);
	Ball->W = BW;
	Ball->H = BH;
	Ball->as.Container.ColourRGBA = 0xAAAA00FF;
	Ball->redraw = true;
	Ball->Tick = ball;

	UIItem *PaddleA = UICreate(Root, JES_UITYPE_CONTAINER, 10, WH / 2, 0);
	PaddleA->W = PW;
	PaddleA->H = PH;
	PaddleA->as.Container.ColourRGBA = 0xAAAA00FF;
	PaddleA->redraw = true;

	UIItem *PaddleB = UICreate(Root, JES_UITYPE_CONTAINER, WW - 10 - PW, WH / 2, 0);
	PaddleB->W = PW;
	PaddleB->H = PH;
	PaddleB->as.Container.ColourRGBA = 0xAAAA00FF;
	PaddleB->redraw = true;

	UIItem *Text = UICreate(Root, JES_UITYPE_TEXT, 0, 0, 0);
	Text->as.Text.items = strdup("0");
	Text->as.Text.FontSize = 24;
	Text->W = 64;
	Text->H = 24;
	Text->redraw = true;
	Text->Tick = textInit;

	JESState State = {
		.initialX = SDL_WINDOWPOS_CENTERED,
		.initialY = SDL_WINDOWPOS_CENTERED,
		.X = SDL_WINDOWPOS_CENTERED,
		.Y = SDL_WINDOWPOS_CENTERED,
		.initialWidth  = WW,
		.initialHeight = WH,
		.Width = WW,
		.Height = WH,
		.FrameDelay = 1,
		.running  = true,
		.root = Root,
		.title = "JES v0.1",
	};
	StartRendering(&State);
	UIFree(Root);
	return 0;
}
