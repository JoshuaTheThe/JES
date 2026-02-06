#include <ui/main.h>
#include <ui/window.h>
#include <main.h>

/**
 * UIUpdate()
 * update the UI for a frame
 */
void UIUpdate(JESState *State)
{
	bool redraw = State->root->redraw;
	int32_t FrameDelay = State->FrameDelay;

	SDL_Event ev;
	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
		case SDL_MOUSEMOTION:
			pthread_mutex_lock(&lock);
			State->MouseB = SDL_GetMouseState(&State->MouseX, &State->MouseY);
			pthread_mutex_unlock(&lock);
			break;
		case SDL_QUIT:
			pthread_mutex_lock(&lock);
			State->running = false;
			pthread_mutex_unlock(&lock);
			break;
		case SDL_WINDOWEVENT:
			switch (ev.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				printf("Window resized: %dx%d\n",
				       ev.window.data1, ev.window.data2);
				State->Width = ev.window.data1;
				State->Height = ev.window.data2;
				State->root->redraw = true;
				State->root->W = State->Width;
				State->root->H = State->Height;
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				break;
			}
			break;
		default:
			pthread_mutex_lock(&lock);
			da_append(&State->root->Events, ev);
			pthread_mutex_unlock(&lock);
			break;
		}
	}
	pthread_mutex_lock(&lock);
	redraw = State->root->redraw;
	FrameDelay = State->FrameDelay;
	pthread_mutex_unlock(&lock);
	if (redraw)
	{
		SDL_SetRenderDrawColor(State->Renderer,
				       0x00,
				       0x00,
				       0x00,
				       0xff);
		SDL_RenderClear(State->Renderer);
		/* TODO - Implement thread safety by cloning the tree */
		UIRecursiveDraw(State->root, State);
		SDL_RenderPresent(State->Renderer);
	}
        
	State->MouseB = SDL_GetMouseState(&State->MouseX, &State->MouseY);
	UIRecursiveTick(State->root);
	SDL_Delay(FrameDelay);
}

/**
 * UIBegin()
 * Starts the SDL subsystem and starts rendering
 * This must be called on the main thread
 * error on non-zero
 */
int UIBegin(JESState *State)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		fprintf(stderr, "SDL Error: %s", SDL_GetError());
		return 1;
	}

	if (TTF_Init() < 0)
	{
		fprintf(stderr, "OpenTTF Error\n");
		SDL_Quit();
		return 1;
	}

	SDL_Window *Window = SDL_CreateWindow(
		State->title,
		State->initialX,
		State->initialY,
		State->initialWidth,
		State->initialHeight,
		SDL_WINDOW_SHOWN
	);

	if (Window == NULL)
	{
		fprintf(stderr,
			"SDL Error, Could not create window: %s\n",
			SDL_GetError());
		SDL_Quit();
		return 1;
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
		SDL_DestroyWindow(Window);
		SDL_Quit();
		return 1;
	}

	pthread_mutex_lock(&lock);
	State->Window = Window;
	State->Renderer = Renderer;
	pthread_mutex_unlock(&lock);
	return 0;
}

void UIEnd(JESState *State)
{
	SDL_DestroyRenderer(State->Renderer);
	SDL_DestroyWindow(State->Window);
	SDL_Quit();
}
