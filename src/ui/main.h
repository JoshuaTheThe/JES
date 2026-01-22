#ifndef MAIN_UI_H
#define MAIN_UI_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

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
struct JESState;
typedef struct JESState JESState;

typedef struct UIText
{
	TTF_Font *Font;
	char *Text;
	size_t TextSize;
	size_t FontSize;
} UIText;

typedef struct UIContainer
{
	uint32_t ColourRGBA;
	size_t MouseX, MouseY; /* Position within the window */
} UIContainer;

/**
 * If not cleared or sorted, we will run out of memory.
 * please process events, even if your just deleting them.
 */

typedef struct UIEventQueue
{
	SDL_Event *items;
	size_t count, capacity;
} UIEventQueue;

/* UIItem owns its children.
 * UI tree is immutable during draw.
 * All mutations occur under lock.
 */
typedef struct UIItem
{
	UIEventQueue Events;
	UIItem **items, *Parent;
	SDL_Texture *Tex;
	UIType Type;
	size_t X, Y, Z, W, H;
	size_t count, capacity;
	bool redraw, focused;

	union
	{
		UIText Text;
		UIContainer Container;
	} as;

	void (*Tick)(UIItem *Self);
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


UIItem *UICreate(UIItem *Parent, UIType Type, size_t X, size_t Y, size_t Z);
void UIRecursiveDraw(UIItem *Item, JESState *State);
void UIFree(UIItem *Root);
void UIRecursiveTick(UIItem *Root);

#endif

