#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "darsaudio.h"
#include "darsdbus.h"
#include "agc.h" 
#include "utils.h"

#define AGC_MAX_CTL_NUM 4

struct agc {
    WINDOW  *scr;
    int     active; /* index 1 */
    float   ratio;   /* index 2 */
    float   volume; /* index 3 */
    float   factor;  /*  index 4 */

    int active_index;
};

extern darsdbus_t *g_dbus;
extern int attr_active;

static void draw_active_ctl(agc_t *a, int act);
static void draw_ratio_ctl(agc_t *a, int act);
static void draw_volume_ctl(agc_t *a, int act);
static void draw_factor_ctl(agc_t *a, int act);

static inline void toggle_enable(agc_t *a)
{
    if (!a) {
        return;
    }
    int enable = a->active ? 0 : 1;
    if (darsdbus_set_int(g_dbus, DARS_PARAM_AGC_ENABLE_STR, enable) == 0) {
        if (0 == darsdbus_get_int(g_dbus, DARS_PARAM_AGC_ENABLE_STR, &enable)) {
            a->active = enable;
        }
    }
}

agc_t *
agc_new(WINDOW *scr)
{
    agc_t *pagc;
    if (!scr) {
        return NULL;
    }

    pagc = malloc(sizeof(agc_t));
    if (!pagc) {
        return NULL;
    }
    memset(pagc, 0, sizeof(agc_t));

    pagc->scr = scr;
    pagc->active_index = 1;
    darsdbus_get_int(g_dbus, DARS_PARAM_AGC_ENABLE_STR, &pagc->active);
    darsdbus_get_float(g_dbus, DARS_PARAM_AGC_RATIO_STR, &pagc->ratio);
    darsdbus_get_float(g_dbus, DARS_PARAM_AGC_VOLUME_STR, &pagc->volume);
    darsdbus_get_float(g_dbus, DARS_PARAM_AGC_MAX_GAIN_STR, &pagc->factor);

    return pagc;
}

int 
agc_del(agc_t *a)
{
    if (!a) {
        return -1;
    }

    free(a);
    return 0;
}

int 
agc_key_handler(agc_t *a, int key)
{
    if (!a) {
        return -1;
    }

    WINDOW *scr = a->scr;

    switch(key) {
        case KEY_LEFT:
            a->active_index--;
            if (a->active_index < 1)
                a->active_index = AGC_MAX_CTL_NUM;

            break;

        case KEY_RIGHT:
            a->active_index++;
            if (a->active_index > AGC_MAX_CTL_NUM)
                a->active_index = 1;

            break;

        case KEY_UP:
            {
                switch(a->active_index) {
                    case 1:
                        toggle_enable(a);
                        break;

                    case 2:
                        {
                            float ratio = a->ratio;
                            ratio += 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_AGC_RATIO_STR, ratio)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_AGC_RATIO_STR, &ratio)) {
                                    a->ratio = ratio;
                                }
                            }
                        }
                        break;

                    case 3:
                        {
                            float volume = a->volume;
                            volume += 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_AGC_VOLUME_STR, volume)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_AGC_VOLUME_STR, &volume)) {
                                    a->volume = volume;
                                }
                            }
                        }
                        break;

                    case 4:
                        {
                            float factor = a->factor;
                            factor += 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_AGC_MAX_GAIN_STR, factor)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_AGC_MAX_GAIN_STR, &factor)) {
                                    a->factor = factor;
                                }
                            }
                        }
                        break;

                }

            }
            break;
        case KEY_DOWN:
            {
                switch(a->active_index) {
                    case 1:
                        toggle_enable(a);
                        break;

                    case 2:
                        {
                            float ratio = a->ratio;
                            ratio -= 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_AGC_RATIO_STR, ratio)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_AGC_RATIO_STR, &ratio)) {
                                    a->ratio = ratio;
                                }
                            }
                        }
                        break;

                    case 3:
                        {
                            float volume = a->volume;
                            volume -= 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_AGC_VOLUME_STR, volume)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_AGC_VOLUME_STR, &volume)) {
                                    a->volume = volume;
                                }
                            }
                        }
                        break;

                    case 4:
                        {
                            float factor = a->factor;
                            factor -= 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_AGC_MAX_GAIN_STR, factor)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_AGC_MAX_GAIN_STR, &factor)) {
                                    a->factor = factor;
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
agc_draw_refresh(agc_t *a)
{
    int ctl_w = COLS / AGC_MAX_CTL_NUM;

    if (!a || !a->scr)
        return -1;

    WINDOW *scr = a->scr;
    if (COLS >= 100) {
        int len = strlen("  _____          _____   _____                     _____  _____ ");
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 3, pos, "  _____          _____   _____                     _____  _____ ");
        mvwprintw(scr, 4, pos, " |  __ \\   /\\   |  __ \\ / ____|              /\\   / ____|/ ____|");
        mvwprintw(scr, 5, pos, " | |  | | /  \\  | |__) | (___    ______     /  \\ | |  __| |     ");
        mvwprintw(scr, 6, pos, " | |  | |/ /\\ \\ |  _  / \\___ \\  |______|   / /\\ \\| | |_ | |     ");
        mvwprintw(scr, 7, pos, " | |__| / ____ \\| | \\ \\ ____) |           / ____ \\ |__| | |____ ");
        mvwprintw(scr, 8, pos, " |_____/_/    \\_\\_|  \\_\\_____/           /_/    \\_\\_____|\\_____|");
    }
    else {
        char *logo = "DARS-AGC";
        int len = strlen(logo);
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 2, pos, logo);
    }

    if (1 == a->active_index) {
        attron(attr_active);
        draw_active_ctl(a, 1);
        attroff(attr_active);
    }
    else {
        draw_active_ctl(a, 0);
    }

    if (2 == a->active_index) {
        attron(attr_active);
        draw_ratio_ctl(a, 1);
        attroff(attr_active);
    }
    else {
        draw_ratio_ctl(a, 0);
    }

    if (3 == a->active_index) {
        attron(attr_active);
        draw_volume_ctl(a, 1);
        attroff(attr_active);
    }
    else {
        draw_volume_ctl(a, 0);
    }

    if (4 == a->active_index) {
        attron(attr_active);
        draw_factor_ctl(a, 1);
        attroff(attr_active);
    }
    else {
        draw_factor_ctl(a, 0);
    }

    return 0;
}


static void 
draw_active_ctl(agc_t *a, int act)
{
    int ctl_w = COLS / AGC_MAX_CTL_NUM;

    if (!a || !a->scr)
        return;

    mvwprintw(a->scr, LINES-5, ctl_w/2, "AGC-STATUS");
    if (a->active) {
        if (act) {
            attron(attr_active | A_BOLD);
        }
        else {
            attron(A_BOLD);
        }
        mvwprintw(a->scr, LINES-5-2, ctl_w/2, "ENABLED");
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
        mvwprintw(a->scr, LINES-5-2, ctl_w/2, "DISABLED");
        if (act) {
            attroff(attr_active | A_DIM);
        }
        else {
            attroff(A_DIM);
        }
    }
}

static void 
draw_ratio_ctl(agc_t *a, int act)
{
    char buf[64] = {0,};
    int ctl_w = COLS / AGC_MAX_CTL_NUM;

    if (!a || !a->scr) {
        return;
    }

    snprintf(buf, 64, "AGC-RATIO:%.2f", a->ratio);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 10.0f;
    conf.value = a->ratio;
    if (act) {
        attron(attr_active);
        draw_sliderbar(a->scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(a->scr, &conf);
    }
}

static void 
draw_volume_ctl(agc_t *a, int act)
{
    char buf[64] = {0,};
    int ctl_w = COLS / AGC_MAX_CTL_NUM;

    if (!a || !a->scr) {
        return;
    }

    snprintf(buf, 64, "AGC-VOLUME:%.2f", a->volume);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+2*ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 10.0f;
    conf.value = a->volume;
    if (act) {
        attron(attr_active);
        draw_sliderbar(a->scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(a->scr, &conf);
    }

}

static void 
draw_factor_ctl(agc_t *a, int act)
{
    char buf[64] = {0,};
    int ctl_w = COLS / AGC_MAX_CTL_NUM;

    if (!a || !a->scr) {
        return;
    }

    snprintf(buf, 64, "AGC-FACTOR:%.2f", a->factor);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+3*ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 10.0f;
    conf.value = a->factor;
    if (act) {
        attron(attr_active);
        draw_sliderbar(a->scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(a->scr, &conf);
    }

}
