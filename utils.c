#include <string.h>
#include "utils.h"

int 
draw_sliderbar(WINDOW *scr, sliderbar_conf_t *conf)
{
    int label_len;
    int level;

    if (!scr || !conf)
        return -1;

    label_len = strlen(conf->label);

    if (conf->value > conf->max)
        conf->value = conf->max;

    mvwprintw(scr, conf->y, conf->x - label_len/2, conf->label);
    level = (int)((conf->value-conf->min)*conf->h / (conf->max - conf->min));

    for (int i = 0; i < conf->h; i++) {
        mvwaddch(scr, conf->y-2-i, conf->x-1, ACS_VLINE);
        mvwaddch(scr, conf->y-2-i, conf->x+2, ACS_VLINE);
    }

    mvwaddch(scr, conf->y-2, conf->x-1, ACS_LLCORNER);
    mvwaddch(scr, conf->y-2, conf->x, ACS_HLINE);
    mvwaddch(scr, conf->y-2, conf->x+1, ACS_HLINE);
    mvwaddch(scr, conf->y-2, conf->x+2, ACS_LRCORNER);

    mvwaddch(scr, conf->y-2-conf->h, conf->x-1, ACS_ULCORNER);
    mvwaddch(scr, conf->y-2-conf->h, conf->x, ACS_HLINE);
    mvwaddch(scr, conf->y-2-conf->h, conf->x+1, ACS_HLINE);
    mvwaddch(scr, conf->y-2-conf->h, conf->x+2, ACS_URCORNER);

    for (int i = 0; i < level; i++) {
        mvwaddch(scr, conf->y-3-i, conf->x, ' '|A_REVERSE);
        mvwaddch(scr, conf->y-3-i, conf->x+1, ' ' |A_REVERSE);
    }


    return 0;
}
