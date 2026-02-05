#include<main.h>
#include<ui/main.h>

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
	Q->State = NULL;

	if (Parent != NULL)
	{
		Q->State = Parent->State;
		da_append(Parent, Q);
	}
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
