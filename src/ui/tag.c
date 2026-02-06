#include <main.h>
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
                .Name.capacity = strlen(Name),
                .Name.count = strlen(Name),
        };

        da_append(&Item->Tags, tag);
}

void UIRemoveTag(UIItem *Item, const char *Name)
{
        size_t Index;
        UITag *tag = UIFindTag(Item, Name, &Index);

        if (tag)
        {

        }
}
