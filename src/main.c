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
#include <ui/window.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
	/**
	 *	State is shared across the whole program,
	 *	all access must be locked.
	 * */

	const int32_t WW = 512, WH = 512;

	JESState State = {.initialX = SDL_WINDOWPOS_CENTERED,
			  .initialY = SDL_WINDOWPOS_CENTERED,
			  .initialWidth = WW,
			  .initialHeight = WH,
			  .title = "JES v0.1"};
	if (UIBegin(&State) > 0)
		exit(1);

	UIItem *Root = UICreate(NULL, JES_UITYPE_CONTAINER, 0, 0, 0);
	Root->W = WW;
	Root->H = WH;
	Root->ColourRGBA = 0x000000FF;
	Root->redraw = true;
	Root->State = &State;

        for (size_t i = 0; i < WW / 128; ++i)
        {
        	UIItem *Btn = UICreate(Root, JES_UITYPE_BUTTON, 0, 0, 0);
        	Btn->W = 128;
        	Btn->H = 20;
        	Btn->redraw = true;
        	Btn->ColourRGBA = 0x00F0F0FF;

        	UIItem *Text = UICreate(Btn, JES_UITYPE_TEXT, 0, 0, 1);
        	Text->as.Text.items = strdup("Click Me!");
        	Text->as.Text.FontSize = 16;
        	Text->ColourRGBA = 0x000000FF;
        	Text->redraw = true;
        	Text->as.Text.Font = TTF_OpenFont("assets/dos.ttf", Text->as.Text.FontSize);
        }

	UIItem *Body = UICreate(Root, JES_UITYPE_CONTAINER, 0, 0, 0);
        Body->W = WW;
        Body->H = WH - 20;
	Body->ColourRGBA = 0x0000FFFF;
	Body->redraw = true;

        UIFlexX(Root);

	State.X = SDL_WINDOWPOS_CENTERED;
	State.Y = SDL_WINDOWPOS_CENTERED;
	State.Width = WW;
	State.Height = WH;
	State.FrameDelay = 1;
	State.running  = true;
	State.root = Root;

	while (State.running)
	{
		UIUpdate(&State);
	}
	UIEnd(&State);
	UIFree(Root);
	return 0;
}
