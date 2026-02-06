#include <main.h>
#include <ui/main.h>

void UIFlexX(UIItem *Item)
{
	int32_t CurrentX = 0;
	int32_t CurrentY = 0;
	int32_t RowHeight = 0;

	foreach (Element, Item, {
		if (!Element->visible)
			continue;

		if (CurrentX + Element->W > Item->W && CurrentX > 0)
		{
			CurrentY += RowHeight;
			CurrentX = 0;
			RowHeight = 0;
		}

		Element->X = CurrentX;
		Element->Y = CurrentY;

		CurrentX += Element->W;

		if (Element->H > RowHeight)
			RowHeight = Element->H;
	})
}
