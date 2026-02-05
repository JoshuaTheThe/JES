#include<main.h>
#include<ui/main.h>

void UIRecursiveTick(UIItem *Root)
{
	if (!Root)
		return;
	if (Root->Tick)
		Root->Tick(Root);
	for (size_t i = 0; i < Root->count; ++i)
	{
		const bool InBounds =
			Root->State->MouseX >= Root->items[i]->X &&
			Root->State->MouseX <= Root->items[i]->X
				+ Root->items[i]->W &&
			Root->State->MouseY >= Root->items[i]->Y &&
			Root->State->MouseY <= Root->items[i]->Y
				+ Root->items[i]->H;
		if (Root->items[i]->Type == JES_UITYPE_BUTTON && InBounds)
		{
			for (size_t btn = 0; btn < JES_UI_BUTTON_COUNT; ++btn)
			{
				if (!Root->items[i]->as.Button.MouseDown[btn])
					continue;
				if (Root->State->MouseB & (1 << btn))
					Root->items[i]->as.Button.MouseDown[btn](Root->items[i],
						Root->State->MouseX, Root->State->MouseY);
				else if (Root->items[i]->as.Button.Down[btn])
					Root->items[i]->as.Button.MouseUp[btn](Root->items[i],
						Root->State->MouseX, Root->State->MouseY);
				Root->items[i]->as.Button.Down[btn] = Root->State->MouseB & (1 << btn);
			}
		}
		UIRecursiveTick(Root->items[i]);
	}
}
