#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include <jes.h>
#include <ui/main.h>

int UIBegin(JESState *);
void UIEnd(JESState *State);
void UIUpdate(JESState *State);

#endif

