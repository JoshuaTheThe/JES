#include<main.h>
#include<ui/main.h>

/**
 * Simply draw the container
 */
static void UIDrawContainer(UIItem *Item, JESState *State)
{
create:
	if (!Item->Tex)
	{
		Item->Tex = SDL_CreateTexture(State->Renderer,
				SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET,
				Item->W, Item->H);
	}

	if (Item->Tex && Item->redraw)
	{
		int32_t W, H;
		SDL_QueryTexture(Item->Tex, NULL, NULL, &W, &H);
		if (Item->W != W || Item->H != H)
		{
			SDL_DestroyTexture(Item->Tex);
			Item->Tex = NULL;
			goto create;
		}
		SDL_Texture *Old = SDL_GetRenderTarget(State->Renderer);
		SDL_SetRenderTarget(State->Renderer,
				Item->Tex);
		/* ... */
		SDL_Rect Rect = {
			.x = 0, .y = 0,
			.w = Item->W, .h = Item->H,
		};

		uint8_t A = Item->ColourRGBA & 255;
		uint8_t R = Item->ColourRGBA >> 24;
		uint8_t G = Item->ColourRGBA >> 16;
		uint8_t B = Item->ColourRGBA >> 8;
		
		SDL_SetRenderDrawColor(State->Renderer,
				R, G, B, A);
		SDL_RenderFillRect(State->Renderer,
				&Rect);
		SDL_SetRenderTarget(State->Renderer,
				Old);
		Item->redraw = false;
	}
}

static void UIDrawText(UIItem *Item, JESState *State)
{
	if (!Item->as.Text.Font)
		return;
create:
	if (Item->redraw)
	{
		if (Item->Tex)
		{
			SDL_DestroyTexture(Item->Tex);
			Item->Tex = NULL;
			goto create;
		}
		
		uint8_t A = Item->ColourRGBA & 255;
		uint8_t R = Item->ColourRGBA >> 24;
		uint8_t G = Item->ColourRGBA >> 16;
		uint8_t B = Item->ColourRGBA >> 8;
		SDL_Color fg = { R, G, B, A };
		SDL_Surface *Surface = TTF_RenderText_Solid(Item->as.Text.Font, Item->as.Text.items, fg);
		if (!Surface)
		{
			fprintf(stderr, "TTF_RenderText_Solid failed: %s\n", TTF_GetError());
			return;
		}
		Item->Tex = SDL_CreateTextureFromSurface(State->Renderer, Surface);
		if (!Item->Tex)
		{
			fprintf(stderr, "SDL_CreateTextureFromSurface failed: %s\n", SDL_GetError());
		}

		Item->W = Surface->w;
		Item->H = Surface->h;
		SDL_FreeSurface(Surface);
		Item->redraw = false;
	}
}

/**
 * UIDrawImage - Simply perform a RenderCopy
 * if valid
 * */
void UIDrawImage(UIItem *Item, JESState *State)
{
	/**
	 * Early return
	 * */
	if (!Item->Tex)
		return;
	/**
	 * Literally do nothing
	 */
}

int UICompareItem(const void *A, const void *B)
{
	if (((UIItem *)A)->Z < ((UIItem *)B)->Z)
		return 1;
	return -1;
}

/**
 * Recursively draw all children
 * */
void UIRecursiveDraw(UIItem *Item, JESState *State)
{
	if (!Item)
	{
		return;
	}

	qsort(Item->items, Item->count, sizeof(Item->items[0]), UICompareItem);

	switch (Item->Type)
	{
		/**
		 * Error Type and/or placeholder or other degen/unrendered type
		 */
	case JES_UITYPE_NONE:
		break;
	case JES_UITYPE_CONTAINER:
		UIDrawContainer(Item, State);
		break;
	case JES_UITYPE_TEXT:
		UIDrawText(Item, State);
		break;
	case JES_UITYPE_IMAGE:
		UIDrawImage(Item, State);
		break;
	case JES_UITYPE_BUTTON:
		UIDrawContainer(Item, State);
		break;
	default:
		break;
	}

	if (!Item->Tex)
		return;

	SDL_Texture *Old = SDL_GetRenderTarget(State->Renderer);
	SDL_SetRenderTarget(State->Renderer,
			    Item->Tex);
	for (size_t i = 0; i < Item->count; ++i)
	{
		UIRecursiveDraw(Item->items[i], State);
	}
	SDL_SetRenderTarget(State->Renderer, Old);

	SDL_Rect Rect = {
		.x = Item->X,
		.y = Item->Y,
		.w = Item->W,
		.h = Item->H,
	};
	SDL_RenderCopy(State->Renderer, Item->Tex, NULL, &Rect);
}
