#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "darsaudio.h"
#include "darsdbus.h"
#include "eq.h" 
#include "utils.h"

#define EQ_MAX_CTL_NUM 11

struct eq {
    WINDOW  *scr;
    int     active; /* index 1 */
    float   bands[10];   /* index 2 */

    int active_index;
};

extern darsdbus_t *g_dbus;
extern int attr_active;

static inline void toggle_enable(eq_t *p)
{
    if (!p) {
        return;
    }
    int enable = p->active ? 0 : 1;
    if (darsdbus_set_int(g_dbus, DARS_PARAM_FIREQ_ENABLE_STR, enable) == 0) {
        p->active = enable;
    }
}

static void draw_band(eq_t *p, int index, int act)
{
    char buf[64] = {0, };
    int ctl_w = COLS / EQ_MAX_CTL_NUM;

    if (!p || !p->scr) {
        return;
    }

    switch (index) {
        case 0:
            snprintf(buf, 64, "31");
            break;
        case 1:
            snprintf(buf, 64, "63");
            break;
        case 2:
            snprintf(buf, 64, "125");
            break;
        case 3:
            snprintf(buf, 64, "250");
            break;
        case 4:
            snprintf(buf, 64, "500");
            break;
        case 5:
            snprintf(buf, 64, "1K");
            break;
        case 6:
            snprintf(buf, 64, "2K");
            break;
        case 7:
            snprintf(buf, 64, "4K");
            break;
        case 8:
            snprintf(buf, 64, "8K");
            break;
        case 9:
            snprintf(buf, 64, "16K");
            break;
    }

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+(index+1)*ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = -10.0f;
    conf.max = 10.0f;
    conf.value = p->bands[index];
    if (act) {
        attron(attr_active);
        draw_sliderbar(p->scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(p->scr, &conf);
    }

}

static void 
set_band(eq_t *p)
{
    char sbuf[256] = {0, };

    if (!p) {
        return;
    }

    snprintf(sbuf, 256, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", p->bands[0], p->bands[1], p->bands[2], p->bands[3],
            p->bands[4], p->bands[5], p->bands[6], p->bands[7], p->bands[8], p->bands[9]);

    darsdbus_set_param(g_dbus, DARS_PARAM_FIREQ_BANDS_STR, sbuf);
}

static void
get_band(eq_t *p) 
{
    char *bands;
    if (!p) {
        return;
    }

    bands = darsdbus_get_param(g_dbus, DARS_PARAM_FIREQ_BANDS_STR);
    if (bands) {
        sscanf(bands, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &p->bands[0], &p->bands[1], &p->bands[2],
                &p->bands[3], &p->bands[4], &p->bands[5], &p->bands[6], &p->bands[7], &p->bands[8], &p->bands[9]);
        free(bands);
    }
}

eq_t *
eq_new(WINDOW *scr)
{
    eq_t *p;
    char *bands;

    if (!scr) {
        return NULL;
    }
    p = malloc(sizeof(eq_t));
    if (!p) {
        return NULL;
    }
    memset(p, 0, sizeof(eq_t));

    p->scr = scr;
    p->active_index = 1;

    get_band(p);
    return p;
}

int 
eq_del(eq_t *p)
{
    if (!p) {
        return -1;
    }
    free(p);

    return 0;
}


int 
eq_key_handler(eq_t *p, int key)
{
    WINDOW *scr = NULL;

    if (!p || !p->scr)
        return -1;

    scr = p->scr;

    switch(key) {
        case KEY_LEFT:
            p->active_index--;
            if (p->active_index < 1)
                p->active_index = EQ_MAX_CTL_NUM;

            break;

        case KEY_RIGHT:
            p->active_index++;
            if (p->active_index > EQ_MAX_CTL_NUM)
                p->active_index = 1;

            break;

        case KEY_UP:
            {
                switch(p->active_index) {
                    case 1:
                        toggle_enable(p);
                        break;

                    default:
                        {
                            int i = p->active_index - 2;
                            if (i >= 0 && i < 10) {
                                p->bands[i] += 0.1f;
                                set_band(p);
                                get_band(p);
                            }
                        }
                        break;
                }

            }
            break;
        case KEY_DOWN:
            {
                switch(p->active_index) {
                    case 1:
                        toggle_enable(p);
                        break;

                    default:
                        {
                            int i = p->active_index - 2;
                            if (i >= 0 && i < 10) {
                                p->bands[i] -= 0.1f;
                                set_band(p);
                                get_band(p);
                            }
                        }
                        break;
                }
            }
            break;

        default:
            break;

    }

}

int 
eq_draw_refresh(eq_t *p)
{
    int ctl_w = COLS / EQ_MAX_CTL_NUM;

    if (!p || !p->scr)
        return -1;

    WINDOW *scr = p->scr;
    if (COLS >= 100) {
        int len = strlen("  _____          _____   _____            ______ ____  ");
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 3, pos, "  _____          _____   _____            ______ ____  ");
        mvwprintw(scr, 4, pos, " |  __ \\   /\\   |  __ \\ / ____|          |  ____/ __ \\ ");
        mvwprintw(scr, 5, pos, " | |  | | /  \\  | |__) | (___    ______  | |__ | |  | |");
        mvwprintw(scr, 6, pos, " | |  | |/ /\\ \\ |  _  / \\___ \\  |______| |  __|| |  | |");
        mvwprintw(scr, 7, pos, " | |__| / ____ \\| | \\ \\ ____) |          | |___| |__| |");
        mvwprintw(scr, 8, pos, " |_____/_/    \\_\\_|  \\_\\_____/           |______\\___\\_\\");
    }
    else {
        char *logo = "DARS-EQ";
        int len = strlen(logo);
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 2, pos, logo);
    }

    if (p->active_index == 1) { // Enable
        attron(attr_active);
        mvwprintw(scr, LINES-5, ctl_w/2, "EQ-STATUS");
        if (p->active) {
            attron(attr_active | A_BOLD);
            mvwprintw(scr, LINES-5-2, ctl_w/2, "ENABLED");
            attroff(attr_active | A_BOLD);
        }
        else {
            attron(attr_active | A_DIM);
            mvwprintw(scr, LINES-5-2, ctl_w/2, "DISABLED");
            attroff(attr_active | A_DIM);
        }
        attroff(attr_active);

    }
    else {
        mvwprintw(scr, LINES-5, ctl_w/2, "EQ-STATUS");
        if (p->active) {
            attron(A_BOLD);
            mvwprintw(scr, LINES-5-2, ctl_w/2, "ENABLED");
            attroff(A_BOLD);
        }
        else {
            attron(A_DIM);
            mvwprintw(scr, LINES-5-2, ctl_w/2, "DISABLED");
            attroff(A_DIM);
        }
    }

    for (int i = 0; i < 10; i++) {
        if (p->active_index-2 == i) {
            attron(attr_active);
            draw_band(p, i, 1);
            attroff(attr_active);
        }
        else {
            draw_band(p, i, 0);
        }
    }

    return 0;

}






