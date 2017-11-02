#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "darsaudio.h"
#include "darsdbus.h"
#include "clarity.h" 
#include "utils.h"

#define CLARITY_MAX_CTL_NUM 3

struct clarity {
    WINDOW  *scr;
    int     active; /* index 1 */
    int     mode;   /* index 2 */
    float   factor; /* index 3 */
    int active_index;
};

extern darsdbus_t *g_dbus;
extern int attr_active;

static void draw_active_ctl(clarity_t *a, int act);
static void draw_mode_ctl(clarity_t *a, int act);
static void draw_factor_ctl(clarity_t *a, int act);

static inline void toggle_enable(clarity_t *c)
{
    if (!c)
        return;
    int enable = c->active ? 0 : 1;
    if (darsdbus_set_int(g_dbus, DARS_PARAM_CLARITY_ENABLE_STR, enable) == 0) {
        c->active = enable;
    }
}

clarity_t *
clarity_new(WINDOW *scr)
{
    clarity_t *c;
    if (!scr)
        return NULL;

    c = malloc(sizeof(clarity_t));
    if (!c)
        return NULL;
    memset(c, 0, sizeof(clarity_t));

    c->scr = scr;
    c->active_index = 1;
    darsdbus_get_int(g_dbus, DARS_PARAM_CLARITY_ENABLE_STR, &c->active);
    darsdbus_get_int(g_dbus, DARS_PARAM_CLARITY_MODE_STR, &c->mode);
    darsdbus_get_float(g_dbus, DARS_PARAM_CLARITY_FACTOR_STR, &c->factor);

    return c;
}

int 
clarity_del(clarity_t *c)
{
    if (!c) {
        return -1;
    }

    free(c);
    return 0;
}

int 
clarity_key_handler(clarity_t *c, int key)
{
    WINDOW *scr = NULL;

    if (!c || !c->scr)
        return -1;

    scr = c->scr;

    switch(key) {
        case KEY_LEFT:
            c->active_index--;
            if (c->active_index < 1)
                c->active_index = CLARITY_MAX_CTL_NUM;

            break;

        case KEY_RIGHT:
            c->active_index++;
            if (c->active_index > CLARITY_MAX_CTL_NUM)
                c->active_index = 1;

            break;

        case KEY_UP:
            {
                switch(c->active_index) {
                    case 1:
                        toggle_enable(c);
                        break;

                    case 2:
                        {
                            int m = c->mode;
                            m++;
                            if (m > 1) {
                                m = 1;
                            }
                            if (0 == darsdbus_set_int(g_dbus, DARS_PARAM_CLARITY_MODE_STR, m)) {
                                if (0 == darsdbus_get_int(g_dbus, DARS_PARAM_CLARITY_MODE_STR, &m)) {
                                    c->mode = m;
                                }
                            }
                        }
                        break;

                    case 3:
                        {
                            float factor = c->factor;
                            factor += 0.1f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_CLARITY_FACTOR_STR, factor)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_CLARITY_FACTOR_STR, &factor)) {
                                    c->factor = factor;
                                }
                            }
                        }
                        break;

                }

            }
            break;
        case KEY_DOWN:
            {
                switch(c->active_index) {
                    case 1:
                        toggle_enable(c);
                        break;

                    case 2:
                        {
                            int m = c->mode;
                            m--;
                            if (m < 0) {
                                m = 0;
                            }
                            if (0 == darsdbus_set_int(g_dbus, DARS_PARAM_CLARITY_MODE_STR, m)) {
                                if (0 == darsdbus_get_int(g_dbus, DARS_PARAM_CLARITY_MODE_STR, &m)) {
                                    c->mode = m;
                                }
                            }
                        }
                        break;

                    case 3:
                        {
                            float factor = c->factor;
                            factor -= 0.1f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_CLARITY_FACTOR_STR, factor)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_CLARITY_FACTOR_STR, &factor)) {
                                    c->factor = factor;
                                }
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
clarity_draw_refresh(clarity_t *c)
{
    int ctl_w = COLS / CLARITY_MAX_CTL_NUM;

    if (!c || !c->scr)
        return -1;

    WINDOW *scr = c->scr;
    if (COLS >= 100) {
        int len = strlen("  _____          _____   _____             _____ _               _____  _____ _________     __");
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 3, pos, "  _____          _____   _____             _____ _               _____  _____ _________     __");
        mvwprintw(scr, 4, pos, " |  __ \\   /\\   |  __ \\ / ____|           / ____| |        /\\   |  __ \\|_   _|__   __\\ \\   / /");
        mvwprintw(scr, 5, pos, " | |  | | /  \\  | |__) | (___    ______  | |    | |       /  \\  | |__) | | |    | |   \\ \\_/ / ");
        mvwprintw(scr, 6, pos, " | |  | |/ /\\ \\ |  _  / \\___ \\  |______| | |    | |      / /\\ \\ |  _  /  | |    | |    \\   /  ");
        mvwprintw(scr, 7, pos, " | |__| / ____ \\| | \\ \\ ____) |          | |____| |____ / ____ \\| | \\ \\ _| |_   | |     | |   ");
        mvwprintw(scr, 8, pos, " |_____/_/    \\_\\_|  \\_\\_____/            \\_____|______/_/    \\_\\_|  \\_\\_____|  |_|     |_|   ");
    }
    else {
        char *logo = "DARS-CLARITY";
        int len = strlen(logo);
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 2, pos, logo);
    }

    if (1 == c->active_index) {
        attron(attr_active);
        draw_active_ctl(c, 1);
        attroff(attr_active);
    }
    else {
        draw_active_ctl(c, 0);
    }

    if (c->active_index == 2) { // mode
        attron(attr_active);
        draw_mode_ctl(c, 1);
        attroff(attr_active);
    }
    else {
        draw_mode_ctl(c, 0);
    }

    if (3 == c->active_index) {
        attron(attr_active);
        draw_factor_ctl(c, 1);
        attroff(attr_active);
    }
    else {
        draw_factor_ctl(c, 0);
    }

    return 0;
}

static void 
draw_active_ctl(clarity_t *c, int act)
{
    int ctl_w = COLS / CLARITY_MAX_CTL_NUM;

    if (!c || !c->scr)
        return;

    mvwprintw(c->scr, LINES-5, ctl_w/2, "CLARITY-STATUS");
    if (c->active) {
        if (act) {
            attron(attr_active | A_BOLD);
        }
        else {
            attron(A_BOLD);
        }
        mvwprintw(c->scr, LINES-5-2, ctl_w/2, "ENABLED");
        if (act) {
            attroff(attr_active | A_BOLD);
        }
        else {
            attroff(A_BOLD);
        }
    }
    else {
        if (act) {
            attron(attr_active | A_DIM);
        }
        else {
            attron(A_DIM);
        }
        mvwprintw(c->scr, LINES-5-2, ctl_w/2, "DISABLED");
        if (act) {
            attroff(attr_active | A_DIM);
        }
        else {
            attroff(A_DIM);
        }
    }

}

static void 
draw_factor_ctl(clarity_t *c, int act)
{
    char buf[64] = {0,};
    int ctl_w = COLS / CLARITY_MAX_CTL_NUM;

    if (!c || !c->scr) {
        return;
    }

    snprintf(buf, 64, "CLARITY-FACTOR:%.2f", c->factor);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+2*ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 15.0f;
    conf.value = c->factor;
    if (act) {
        attron(attr_active);
        draw_sliderbar(c->scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(c->scr, &conf);
    }

}

static void 
draw_mode_ctl(clarity_t *c, int act)
{
    int ctl_w = COLS / CLARITY_MAX_CTL_NUM;
    WINDOW *scr = c->scr;

    mvwprintw(scr, LINES-5, ctl_w/2+ctl_w, "CLARITY-MODE");
    if (c->mode == 0) {
        if (act) {
            attron(attr_active | A_BOLD);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(attr_active | A_BOLD);
        }
        else {
            attron(A_BOLD);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(A_BOLD);
        }
    }
    else {
        if (act) {
            attron(attr_active | A_DIM);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(attr_active | A_DIM);
        }
        else {
            attron(A_DIM);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(A_DIM);
        }
    }

    if (c->mode == 1) {
        if (act) {
            attron(attr_active | A_BOLD);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(attr_active | A_BOLD);
        }
        else {
            attron(A_BOLD);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(A_BOLD);
        }
    }
    else {
        if (act) {
            attron(attr_active | A_DIM);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(attr_active | A_DIM);
        }
        else {
            attron(A_DIM);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(A_DIM);
        }
    }
}
