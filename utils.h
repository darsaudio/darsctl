#ifndef __UTILS_H__
#define __UTILS_H__
#include <ncurses.h>


typedef struct {
    int y;
    int x;
    int h;
    char *label;

    float min;
    float max;
    float value;

} sliderbar_conf_t;

int draw_sliderbar(WINDOW *scr, sliderbar_conf_t *conf);














#endif      // __UTILS_H__
