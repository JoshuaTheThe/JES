#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include <main.h>
#include <ui/main.h>
#include <ui/window.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void DropDown(UIItem *Self, size_t X, size_t Y)
{
        (void)X;
        (void)Y;
        printf(" [INFO] Hello, from %p\n", Self);
}

void ButtonToggleDropDown(UIItem *Self, size_t X, size_t Y)
{
        (void)X;
        (void)Y;

        UIItem *container = Self->Parent;
        bool newVisibility = !container->items[1]->visible;

        foreach(i, container, {
                if (__i > 0)
                {
                        i->visible = newVisibility;
                        i->interactable = newVisibility;
                        i->redraw = true;
                }
        })

        const size_t RowH = 20, RowS = 9;
        if (newVisibility)
                container->H = RowH * RowS;
        else
                container->H = RowH;

        Self->redraw = true;
        container->redraw = true;
        UIRoot(container)->redraw = true;
        printf(" [INFO] Drop Down\n");
}

int main(void)
{
        /**
         *	State is shared across the whole program,
         *	all access must be locked.
         * */

        const int32_t WW = 512, WH = 512;

        JESState State = {.initialX = SDL_WINDOWPOS_CENTERED,
                          .initialY = SDL_WINDOWPOS_CENTERED,
                          .initialWidth = WW,
                          .initialHeight = WH,
                          .title = "JES v0.1"};
        if (UIBegin(&State) > 0)
                exit(1);

        UIItem *Root = UICreate(NULL, JES_UITYPE_CONTAINER, 0, 0, 0);
        Root->W = WW;
        Root->H = WH;
        Root->ColourRGBA = 0x000000FF;
        Root->redraw = true;
        Root->State = &State;
        Root->visible = true;
        Root->interactable = true;

        for (int32_t i = 0; i < WW / 128; ++i)
        {
                const size_t RowH = 20, RowS = 9;

                /**
                 * Primary Container
                 */

                UIItem *Con = UICreate(Root, JES_UITYPE_CONTAINER, 0, 0, 1000 + i);
                Con->W = 128;
                Con->H = RowH * RowS;
                Con->redraw = true;
                Con->ColourRGBA = 0x00F0F0FF;
                Con->visible = true;
                Con->interactable = true;

                /**
                 * Button && Button Text
                 */

                UIItem *Btn = UICreate(Con, JES_UITYPE_BUTTON, 0, 0, 1000);
                Btn->W = 128;
                Btn->H = RowH;
                Btn->redraw = true;
                Btn->ColourRGBA = 0x00F0F0FF;
                Btn->visible = true;
                Btn->as.Button.MouseDown[JES_UI_BUTTON_LEFT] = ButtonToggleDropDown;
                Btn->interactable = true;

                UIItem *Text = UICreate(Btn, JES_UITYPE_TEXT, 0, 0, 0);
                Text->as.Text.items = strdup("Click Me!");
                Text->as.Text.FontSize = 16;
                Text->ColourRGBA = 0x000000FF;
                Text->redraw = true;
                Text->as.Text.Font = TTF_OpenFont("assets/dos.ttf", Text->as.Text.FontSize);
                Text->visible = true;

                /* SubMenu */

                for (size_t j = 0; j < 8; ++j)
                {
                        UIItem *Sub = UICreate(Con, JES_UITYPE_BUTTON, 0, 0, j);
                        Sub->W = 128;
                        Sub->H = RowH;
                        Sub->redraw = true;
                        Sub->ColourRGBA = 0x00F0F0FF;
                        Sub->visible = false;
                        Sub->as.Button.MouseDown[JES_UI_BUTTON_LEFT] = DropDown;
                        Sub->interactable = false;

                        UIItem *Text = UICreate(Sub, JES_UITYPE_TEXT, 0, 0, 1);
                        Text->as.Text.items = strdup("Sub-Menu");
                        Text->as.Text.FontSize = 16;
                        Text->ColourRGBA = 0x000000FF;
                        Text->redraw = true;
                        Text->as.Text.Font = TTF_OpenFont("assets/dos.ttf", Text->as.Text.FontSize);
                        Text->visible = true;
                }

                UIFlexX(Con);
                Con->H = RowH;
        }

        UIFlexX(Root);

        State.X = SDL_WINDOWPOS_CENTERED;
        State.Y = SDL_WINDOWPOS_CENTERED;
        State.Width = WW;
        State.Height = WH;
        State.FrameDelay = 1;
        State.running = true;
        State.root = Root;

        while (State.running)
        {
                UIUpdate(&State);
        }
        UIEnd(&State);
        UIFree(Root);
        return 0;
}
