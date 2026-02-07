#include<jes.h>
#include <ui/main.h>
#include <string.h>

UITag *UIFindTag(UIItem *Item, const char *Name, size_t *OutIdx)
{
	foreachr (tag, (&Item->Tags), {
		if (!strncmp(tag->Name.items, Name, tag->Name.count))
		{
			if (OutIdx)
				*OutIdx = __i;
			return tag;
		}
	})

	*OutIdx = -1;
	return NULL;
}

void UICreateTag(UIItem *Item, UIItem *Source, const char *Name)
{
	UITag tag = {
		.Item = Source,
		.Name.items = strdup(Name),
		.Name.capacity = strlen(Name)+1,
		.Name.count = strlen(Name)+1,
	};

	da_append(&Item->Tags, tag);
}

void UIRemoveTag(UIItem *Item, const char *Name)
{
	size_t Index;
	UITag *tag = UIFindTag(Item, Name, &Index);

	if (tag && Index != (size_t)-1)
	{
		free(tag->Name.items);

		for (size_t i = Index; i < Item->Tags.count - 1; i++)
		Item->Tags.items[i] = Item->Tags.items[i + 1];

		Item->Tags.count--;
	}
}
