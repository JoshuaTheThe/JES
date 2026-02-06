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
	JES_UITYPE_TEXT,
	JES_UITYPE_IMAGE,
	JES_UITYPE_BUTTON,
	JES_UITYPE_CONTAINER,
} UIType;

struct UIItem;
typedef struct UIItem UIItem;
struct JESState;
typedef struct JESState JESState;

enum
{
	JES_UI_BUTTON_DEV,
	JES_UI_BUTTON_LEFT,
	JES_UI_BUTTON_MIDDLE,
	JES_UI_BUTTON_RIGHT,
	JES_UI_BUTTON_COUNT,
};

typedef struct UIButton
{
	void (*MouseDown[JES_UI_BUTTON_COUNT])(UIItem *Self, size_t X, size_t Y);
	void (*MouseUp[JES_UI_BUTTON_COUNT])(UIItem *Self, size_t X, size_t Y);
	bool Down[JES_UI_BUTTON_COUNT];
} UIButton;

typedef struct UIText
{
	TTF_Font *Font;
	size_t FontSize;
	char *items;
	size_t count, capacity;
} UIText;

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
	JESState *State;
	SDL_Texture *Tex;
	UIType Type;
	int32_t X, Y, Z, W, H;
	uint32_t ColourRGBA;
	size_t count, capacity;
	bool redraw, focused, visible;

	union
	{
		UIText Text;
		UIButton Button;
	} as;

	void (*Tick)(UIItem *Self);
} UIItem;

#define da_append(xs, x)									   \
	do											 \
	{											  \
		if ((xs)->count >= (xs)->capacity)						 \
		{										  \
			if ((xs)->capacity == 0)						   \
				(xs)->capacity = 256;					      \
			else								       \
				(xs)->capacity *= 2;					       \
			(xs)->items = realloc((xs)->items, (xs)->capacity * sizeof(*(xs)->items)); \
		}										  \
												   \
		(xs)->items[(xs)->count++] = (x);						  \
	} while (0)

#define da_find(xs, x, res)				    \
	do						     \
	{						      \
		res = -1;				      \
		for (size_t _n_ = 0; _n_ < (xs)->count; ++_n_) \
			if ((xs)->items[_n_] == x)	     \
				res = _n_;		     \
	} while (0)

#define da_free(xs)			       \
	do					\
	{					 \
		free((xs)->items);		\
		(xs)->items = NULL;	       \
		(xs)->count = (xs)->capacity = 0; \
	} while (0)


UIItem *UICreate(UIItem *Parent, UIType Type, size_t X, size_t Y, size_t Z);
void UIRecursiveDraw(UIItem *Item, JESState *State);
void UIFree(UIItem *Root);
void UIRecursiveTick(UIItem *Root);
int UICompareItem(const void *A, const void *B);
void UIFlexX(UIItem *Item);
void UITranslate(UIItem *Item, int32_t *OutX, int32_t *OutY);
UIItem *UIRoot(UIItem *Item);

#endif

