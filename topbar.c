#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "topbar.h"

struct topbar {
    int active_index;
    WINDOW *scr;

};

extern int attr_active;

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

    tb->active_index = 0;
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
    char *str1_2 = "Volume(F1)";
    char *str1_1 = "VOL(F1)";
    char *str1_0 = "V(F1)";
    char *str1 = str1_2;

    char *str2_2 = "Equalizer(F2)";
    char *str2_1 = "EQZ(F2)";
    char *str2_0 = "E(F2)";
    char *str2 = str2_2;

    char *str3_2 = "Bass(F3)";
    char *str3_1 = "BAS(F3)";
    char *str3_0 = "B(F3)";
    char *str3 = str3_2;

    char *str4_2 = "Treble(F4)";
    char *str4_1 = "TBL(F4)";
    char *str4_0 = "T(F4)";
    char *str4 = str4_2;

    char *str5_2 = "Surround(F5)";
    char *str5_1 = "SRD(F5)";
    char *str5_0 = "S(F5)";
    char *str5 = str5_2;

    char *str6_2 = "AutoGain(F6)";
    char *str6_1 = "AGC(F5)";
    char *str6_0 = "A(F5)";
    char *str6 = str6_2;

    char *str7_2 = "Compressor(F7)";
    char *str7_1 = "CPS(F7)";
    char *str7_0 = "C(F7)";
    char *str7 = str7_2;

    char *str8_2 = "Reverb(F8)";
    char *str8_1 = "RVB(F8)";
    char *str8_0 = "R(F8)";
    char *str8 = str8_2;

    char *str9_2 = "Impulse(F9)";
    char *str9_1 = "IPS(F9)";
    char *str9_0 = "I(F9)";
    char *str9 = str9_2;

    char *str10_2 = "Miscs(F10)";
    char *str10_1 = "MIC(F10)";
    char *str10_0 = "M(F10)";
    char *str10 = str10_2;

    if (!tb || !tb->scr)
        return -1;

    scr = tb->scr;
    w = COLS/10;

    if (COLS >= 150) {
        str1 = str1_2;
        str2 = str2_2;
        str3 = str3_2;
        str4 = str4_2;
        str5 = str5_2;
        str6 = str6_2;
        str7 = str7_2;
        str8 = str8_2;
        str9 = str9_2;
        str10 = str10_2;
    }
    else if (COLS >= 100) {
        str1 = str1_1;
        str2 = str2_1;
        str3 = str3_1;
        str4 = str4_1;
        str5 = str5_1;
        str6 = str6_1;
        str7 = str7_1;
        str8 = str8_1;
        str9 = str9_1;
        str10 = str10_1;
    }
    else {
        str1 = str1_0;
        str2 = str2_0;
        str3 = str3_0;
        str4 = str4_0;
        str5 = str5_0;
        str6 = str6_0;
        str7 = str7_0;
        str8 = str8_0;
        str9 = str9_0;
        str10 = str10_0;
    }


    if (tb->active_index == 1) {
        attron(attr_active);
        mvprintw(0, 0, str1);
        attroff(attr_active);
    }
    else {
        mvprintw(0, 0, str1);
    }

    if (tb->active_index == 2) {
        attron(attr_active);
        mvprintw(0, w, str2);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w, str2);
    }

    if (tb->active_index == 3) {
        attron(attr_active);
        mvprintw(0, w*2, str3);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*2, str3);
    }

    if (tb->active_index == 4) {
        attron(attr_active);
        mvprintw(0, w*3, str4);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*3, str4);
    }

    if (tb->active_index == 5) {
        attron(attr_active);
        mvprintw(0, w*4, str5);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*4, str5);
    }

    if (tb->active_index == 6) {
        attron(attr_active);
        mvprintw(0, w*5, str6);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*5, str6);
    }

    if (tb->active_index == 7) {
        attron(attr_active);
        mvprintw(0, w*6, str7);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*6, str7);
    }

    if (tb->active_index == 8) {
        attron(attr_active);
        mvprintw(0, w*7, str8);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*7, str8);
    }

    if (tb->active_index == 9) {
        attron(attr_active);
        mvprintw(0, w*8, str9);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*8, str9);
    }

    if (tb->active_index == 10) {
        attron(attr_active);
        mvprintw(0, w*9, str10);
        attroff(attr_active);
    }
    else {
        mvprintw(0, w*9, str10);
    }
    wrefresh(scr);
}


