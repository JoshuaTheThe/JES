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

void ButtonToggleDropDown(UIItem *Self, size_t X, size_t Y)
{
        (void)X;
        (void)Y;
        const size_t ROWH = 20, ROWS = 9;

        UIItem *container = Self->Parent;
        bool newVisibility = !container->items[1]->visible;

        foreach(i, container, {
                if (__i > 0)
                {
                        i->visible = newVisibility;
                        i->redraw = true;
                }
        })

        if (newVisibility)
        {
                container->H = ROWH * ROWS;
        }
        else
        {
                container->H = ROWH;
        }

        UIFlexX(container);
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

        for (int32_t i = 0; i < WW / 128; ++i)
        {
                const size_t RowH = 20;

                /**
                 * Primary Container
                 */

                UIItem *Con = UICreate(Root, JES_UITYPE_CONTAINER, 0, 0, WW / 128 - i);
                Con->W = 128;
                Con->H = RowH;
                Con->redraw = true;
                Con->ColourRGBA = 0x00F0F0FF;
                Con->visible = true;

                /**
                 * Button && Button Text
                 */

                UIItem *Btn = UICreate(Con, JES_UITYPE_BUTTON, 0, 0, 1);
                Btn->W = 128;
                Btn->H = RowH;
                Btn->redraw = true;
                Btn->ColourRGBA = 0x00F0F0FF;
                Btn->visible = true;
                Btn->as.Button.MouseDown[JES_UI_BUTTON_LEFT] = ButtonToggleDropDown;

                UIItem *Text = UICreate(Btn, JES_UITYPE_TEXT, 0, 0, 1);
                Text->as.Text.items = strdup("Click Me!");
                Text->as.Text.FontSize = 16;
                Text->ColourRGBA = 0x000000FF;
                Text->redraw = true;
                Text->as.Text.Font = TTF_OpenFont("assets/dos.ttf", Text->as.Text.FontSize);
                Text->visible = true;

                /* SubMenu */

                for (size_t j = 0; j < 8; ++j)
                {
                        UIItem *Sub = UICreate(Con, JES_UITYPE_BUTTON, 0, 0, 8 - j);
                        Sub->W = 128;
                        Sub->H = RowH;
                        Sub->redraw = true;
                        Sub->ColourRGBA = 0x00F0F0FF;
                        Sub->visible = false;

                        UIItem *Text = UICreate(Sub, JES_UITYPE_TEXT, 0, 0, 1);
                        Text->as.Text.items = strdup("Sub-Menu");
                        Text->as.Text.FontSize = 16;
                        Text->ColourRGBA = 0x000000FF;
                        Text->redraw = true;
                        Text->as.Text.Font = TTF_OpenFont("assets/dos.ttf", Text->as.Text.FontSize);
                        Text->visible = true;
                }

                UIFlexX(Con);
        }

        UIItem *Body = UICreate(Root, JES_UITYPE_CONTAINER, 0, 0, 0);
        Body->W = WW;
        Body->H = WH - 20;
        Body->ColourRGBA = 0x0000FFFF;
        Body->redraw = true;
        Body->visible = true;

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
