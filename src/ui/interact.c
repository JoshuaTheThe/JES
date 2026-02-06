#include<jes.h>
#include<ui/main.h>

void UIRecursiveTick(UIItem *Root)
{
	if (!Root || !Root->visible || !Root->interactable)
		return;
	if (Root->Tick)
		Root->Tick(Root);
	for (size_t i = 0; i < Root->count; ++i)
	{
		int32_t X, Y;
		UITranslate(Root->items[i], &X, &Y);

		const bool InBounds =
			Root->State->MouseX >= X &&
			Root->State->MouseX < X
				+ Root->items[i]->W &&
			Root->State->MouseY >= Y &&
			Root->State->MouseY < Y
				+ Root->items[i]->H;
		if (Root->items[i]->Type == JES_UITYPE_BUTTON && InBounds && Root->items[i]->interactable)
		{
			for (size_t btn = 0; btn < JES_UI_BUTTON_COUNT; ++btn)
			{
				if (Root->items[i]->as.Button.MouseDown[btn] &&
				   !Root->items[i]->as.Button.Down[btn] &&
				    Root->State->MouseB & SDL_BUTTON(btn))
					Root->items[i]->as.Button.MouseDown[btn](Root->items[i],
						Root->State->MouseX, Root->State->MouseY);
				if (Root->items[i]->as.Button.MouseUp[btn] &&
				    Root->items[i]->as.Button.Down[btn] &&
				    !(Root->State->MouseB & SDL_BUTTON(btn)))
				    Root->items[i]->as.Button.MouseUp[btn](Root->items[i],
						Root->State->MouseX, Root->State->MouseY);
				Root->items[i]->as.Button.Down[btn] = Root->State->MouseB & SDL_BUTTON(btn);
			}
		}
		UIRecursiveTick(Root->items[i]);
	}
}
