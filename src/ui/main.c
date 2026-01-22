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
		if (Item->W != (size_t)W || Item->H != (size_t)H)
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
		
		uint8_t A = Item->as.Container.ColourRGBA & 255;
		uint8_t R = Item->as.Container.ColourRGBA >> 24;
		uint8_t G = Item->as.Container.ColourRGBA >> 16;
		uint8_t B = Item->as.Container.ColourRGBA >> 8;
		SDL_SetRenderDrawColor(State->Renderer,
				R, G, B, A);
		SDL_RenderFillRect(State->Renderer,
				&Rect);
		SDL_SetRenderTarget(State->Renderer,
				Old);
		Item->redraw = false;
	}

	SDL_Rect Rect = {
		.x = Item->X,
		.y = Item->Y,
		.w = Item->W,
		.h = Item->H,
	};

	SDL_RenderCopy(State->Renderer,
			Item->Tex,
			NULL,
			&Rect);
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
		SDL_Color fg = { 255, 255, 255, 255 };
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

	if (Item->Tex)
	{
		SDL_Rect Rect = { .x = Item->X, .y = Item->Y, .w = Item->W, .h = Item->H };
		SDL_RenderCopy(State->Renderer, Item->Tex, NULL, &Rect);
	}
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
	case JES_UITYPE_FILESELECTOR:
	default:
		break;
	}

	for (size_t i = 0; i < Item->count; ++i)
	{
		UIRecursiveDraw(Item->items[i], State);
	}
	return;
}

/**
 * Create a UI Object of a given type
 */
UIItem *UICreate(UIItem *Parent, UIType Type, size_t X, size_t Y, size_t Z)
{
	UIItem *Q = (UIItem *)calloc(1, sizeof(*Q));
	if (Q == NULL)
	{
		return NULL;
	}

	Q->X = X;
	Q->Y = Y;
	Q->Z = Z;
	Q->Type = Type;
	Q->Parent = Parent;

	if (Parent != NULL)
		da_append(Parent, Q);
	return Q;
}

/**
 * Recursively Free self and children
 */
void UIFree(UIItem *Root)
{
	if (!Root)
	{
		return;
	}

	for (size_t i = 0; i < Root->count; ++i)
	{
		UIFree(Root->items[i]);
		Root->items[i] = NULL;
	}

	if (Root->Tex)
	{
		if (Root->Type == JES_UITYPE_TEXT)
		{
			TTF_CloseFont(Root->as.Text.Font);
		}
		SDL_DestroyTexture(Root->Tex);
		Root->Tex = NULL;
	}

	free(Root);
}

void UIRecursiveTick(UIItem *Root)
{
	if (!Root)
		return;
	if (Root->Tick)
		Root->Tick(Root);
	for (size_t i = 0; i < Root->count; ++i)
		UIRecursiveTick(Root->items[i]);
}

