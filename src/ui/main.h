#ifndef UI_H
#define UI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	JES_UITYPE_NONE,
	JES_UITYPE_FILESELECTOR,
	JES_UITYPE_TEXT,
	JES_UITYPE_IMAGE,
	JES_UITYPE_TEXTBOX,
	JES_UITYPE_BUTTON,
	JES_UITYPE_CONTAINER,
} UIType;

struct UIItem;
typedef struct UIItem UIItem;

/* UIItem owns its children.
 * UI tree is immutable during draw.
 * All mutations occur under lock.
 */
typedef struct UIItem
{
	UIType Type;
	size_t X, Y, Z, W, H;
	UIItem *items;
	size_t count, capacity;
} UIItem;

#define da_append(xs, x)                                                                           \
        do                                                                                         \
        {                                                                                          \
                if ((xs)->count >= (xs)->capacity)                                                 \
                {                                                                                  \
                        if ((xs)->capacity == 0)                                                   \
                                (xs)->capacity = 256;                                              \
                        else                                                                       \
                                (xs)->capacity *= 2;                                               \
                        (xs)->items = realloc((xs)->items, (xs)->capacity * sizeof(*(xs)->items)); \
                }                                                                                  \
                                                                                                   \
                (xs)->items[(xs)->count++] = (x);                                                  \
        } while (0)

#define da_find(xs, x, res)                                    \
        do                                                     \
        {                                                      \
                res = -1;                                      \
                for (size_t _n_ = 0; _n_ < (xs)->count; ++_n_) \
                        if ((xs)->items[_n_] == x)             \
                                res = _n_;                     \
        } while (0)

#define da_free(xs)                               \
        do                                        \
        {                                         \
                free((xs)->items);                \
                (xs)->items = NULL;               \
                (xs)->count = (xs)->capacity = 0; \
        } while (0)


void UIRecursiveDraw(UIItem *Item);

#endif

