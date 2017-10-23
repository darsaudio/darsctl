#include <stdlib.h>
#include <string.h>
#include "topbar.h"

struct topbar {
    int active_index;
    WINDOW *scr;

};

topbar_t *
topbar_new(WINDOW *scr)
{
    topbar_t *tb = NULL;

    if (!scr)
        return NULL;

    tb = malloc(sizeof(topbar_t));
    if (!tb)
        return NULL;
    memset(tb, 0, sizeof(topbar_t));

    tb->active_index = 1;
    tb->scr = scr;

    return tb;
}

int 
topbar_del(topbar_t *tb)
{
    if (!tb)
        return -1;

    free(tb);

    return 0;
}

int 
topbar_set_active_index(topbar_t *tb, int i)
{
    if (!tb)
        return -1;

    tb->active_index = i;
    
    return 0;
}


int topbar_draw_refresh(topbar_t *tb)
{
    WINDOW *scr = NULL;
    int w;

    if (!tb || !tb->scr)
        return -1;

    scr = tb->scr;
    w = COLS/12;

    mvprintw(0, 0, "Main(F1)");
    mvprintw(0, w, "EQ(F2)");
    mvprintw(0, w*2, "Bass(F3)");
    mvprintw(0, w*3, "Treble(F4)");
    mvprintw(0, w*4, "Surround(F5)");
    mvprintw(0, w*5, "AGC(F6)");
    mvprintw(0, w*6, "Compressor(F7)");
    mvprintw(0, w*7, "Reverb(F8)");
    mvprintw(0, w*8, "Impulse(F9)");
    mvprintw(0, w*9, "Misc(F10)");


    wrefresh(scr);
}


