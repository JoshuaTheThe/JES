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

	if (TTF_Init() < 0)
	{
		fprintf(stderr, "OpenTTF Error\n");
		SDL_Quit();
		return;
	}

	SDL_Window *Window = SDL_CreateWindow(State->title,
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
			UIRecursiveDraw(State->root, State);
			UIRecursiveTick(State->root);
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
