#include<jes.h>
#include<ui/main.h>

static bool UIRecursiveTickFor(UIItem *Item)
{
	int32_t X, Y;
	UITranslate(Item, &X, &Y);

	const bool InBounds =
		Item->State->MouseX >= X &&
		Item->State->MouseX < X + Item->W &&
		Item->State->MouseY >= Y &&
		Item->State->MouseY < Y + Item->H;
	
	const bool was_handled = UIRecursiveTick(Item);
	if (was_handled)
		return was_handled;
	
	if (Item->Type == JES_UITYPE_BUTTON && Item->interactable)
	{
		for (size_t btn = 0; btn < JES_UI_BUTTON_COUNT; ++btn)
		{
			if (InBounds && 
				Item->as.Button.MouseDown[btn] &&
				!Item->as.Button.Down[btn] &&
				Item->State->MouseB & SDL_BUTTON(btn))
			{
				Item->as.Button.MouseDown[btn](Item,
					Item->State->MouseX, Item->State->MouseY);
			}
			
			if (InBounds &&
				Item->as.Button.MouseUp[btn] &&
				Item->as.Button.Down[btn] &&
				!(Item->State->MouseB & SDL_BUTTON(btn)))
			{
				Item->as.Button.MouseUp[btn](Item,
					Item->State->MouseX, Item->State->MouseY);
			}
			
			Item->as.Button.Down[btn] = InBounds && (Item->State->MouseB & SDL_BUTTON(btn));
		}
		
		if (InBounds)
			return true;
	}
	
	return false;
}

bool UIRecursiveTick(UIItem *Root)
{
	if (!Root || !Root->visible || !Root->interactable)
		return false;
	if (Root->Tick)
		Root->Tick(Root);
	UIItem **Items = calloc(Root->count, sizeof(Root->items[0]));
	memcpy(Items, Root->items, Root->count * sizeof(Root->items[0]));
	qsort(Items, Root->count, sizeof(Root->items[0]), UICompareReverse);

	bool Handled = false;
	for (size_t i = 0; i < Root->count; ++i)
	{
		Handled = UIRecursiveTickFor(Items[i]);
		if (Handled)
			break;
	}

	free(Items);
	return Handled;
}
