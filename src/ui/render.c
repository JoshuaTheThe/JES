#include <ui/main.h>
#include <ui/render.h> 
#include <main.h>

/**
 * StartRendering()
 * Starts the SDL subsystem and starts rendering
 * This must be called on the main thread
 */
void StartRendering(JESState *State)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr, "SDL Error: %s", SDL_GetError());
		return;
	}

	SDL_Window *Window = SDL_CreateWindow("JES v0.1",
			State->initialX,
			State->initialY,
			State->initialWidth,
			State->initialHeight,
			SDL_WINDOW_SHOWN);

	if (Window == NULL)
	{
		fprintf(stderr,
			"SDL Error, Could not create window: %s\n",
			SDL_GetError());
		goto WindowFail;
	}

	SDL_Renderer *Renderer = SDL_CreateRenderer(Window,
			-1,
			SDL_RENDERER_ACCELERATED |
			SDL_RENDERER_PRESENTVSYNC |
			SDL_RENDERER_TARGETTEXTURE);

	if (Renderer == NULL)
	{
		fprintf(stderr,
			"SDL Error, Could not create renderer: %s\n",
			SDL_GetError());
		goto RendererFail;
	}

	pthread_mutex_lock(&lock);
	State->Window = Window;
	State->Renderer = Renderer;
	bool running = State->running, redraw = State->root->redraw;
	int32_t FrameDelay = State->FrameDelay;
	pthread_mutex_unlock(&lock);
	while (running)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
				case SDL_QUIT:
					pthread_mutex_lock(&lock);
					State->running = false;
					pthread_mutex_unlock(&lock);
					break;
				default:
					pthread_mutex_lock(&lock);
					da_append(&State->root->Events, ev);
					pthread_mutex_unlock(&lock);
					break;
			}
		}
		pthread_mutex_lock(&lock);
		running = State->running;
		redraw = State->root->redraw;
		FrameDelay = State->FrameDelay;
		pthread_mutex_unlock(&lock);
		if (redraw)
		{
			SDL_SetRenderDrawColor(Renderer,
					0x00,
					0x00,
				    	0x00,
					0xff);
			SDL_RenderClear(Renderer);
			/* TODO - Implement thread safety by cloning the tree */
			UIRecursiveTick(State->root);
			UIRecursiveDraw(State->root, State);
			SDL_RenderPresent(Renderer);
		}
		SDL_Delay(FrameDelay);
	}

	SDL_DestroyRenderer(Renderer);
RendererFail:
	SDL_DestroyWindow(Window);
WindowFail:
	SDL_Quit();

	return;
}
