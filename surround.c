#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "darsaudio.h"
#include "darsdbus.h"
#include "surround.h" 
#include "utils.h"

#define SURROUND_MAX_CTL_NUM 3

struct surround {
    WINDOW  *scr;
    int     active; /* index 1 */
    float   stereo;   /* index 2 */
    float   image; /* index 3 */

    int active_index;
};

extern darsdbus_t *g_dbus;
extern int attr_active;

static void draw_active_ctl(surround_t *a, int act);
static void draw_ratio_ctl(surround_t *a, int act);
static void draw_volume_ctl(surround_t *a, int act);
static void draw_factor_ctl(surround_t *a, int act);

static inline void toggle_enable(surround_t *a)
{
    if (!a) {
        return;
    }
    int enable = a->active ? 0 : 1;
    if (darsdbus_set_int(g_dbus, DARS_PARAM_SURROUND_ENABLE_STR, enable) == 0) {
        if (0 == darsdbus_get_int(g_dbus, DARS_PARAM_SURROUND_ENABLE_STR, &enable)) {
            a->active = enable;
        }
    }
}

surround_t *
surround_new(WINDOW *scr)
{
    surround_t *psurround;
    if (!scr) {
        return NULL;
    }

    psurround = malloc(sizeof(surround_t));
    if (!psurround) {
        return NULL;
    }
    memset(psurround, 0, sizeof(surround_t));

    psurround->scr = scr;
    psurround->active_index = 1;
    darsdbus_get_int(g_dbus, DARS_PARAM_SURROUND_ENABLE_STR, &psurround->active);
    darsdbus_get_float(g_dbus, DARS_PARAM_SURROUND_STEREO_STR, &psurround->stereo);
    darsdbus_get_float(g_dbus, DARS_PARAM_SURROUND_IMAGE_STR, &psurround->image);

    return psurround;
}

int 
surround_del(surround_t *a)
{
    if (!a) {
        return -1;
    }

    free(a);
    return 0;
}

int 
surround_key_handler(surround_t *a, int key)
{
    if (!a) {
        return -1;
    }

    WINDOW *scr = a->scr;

    switch(key) {
        case KEY_LEFT:
            a->active_index--;
            if (a->active_index < 1)
                a->active_index = SURROUND_MAX_CTL_NUM;

            break;

        case KEY_RIGHT:
            a->active_index++;
            if (a->active_index > SURROUND_MAX_CTL_NUM)
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
                            float stereo = a->stereo;
                            stereo += 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_SURROUND_STEREO_STR, stereo)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_SURROUND_STEREO_STR, &stereo)) {
                                    a->stereo = stereo;
                                }
                            }
                        }
                        break;

                    case 3:
                        {
                            float image = a->image;
                            image += 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_SURROUND_IMAGE_STR, image)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_SURROUND_IMAGE_STR, &image)) {
                                    a->image = image;
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
                            float stereo = a->stereo;
                            stereo -= 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_SURROUND_STEREO_STR, stereo)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_SURROUND_STEREO_STR, &stereo)) {
                                    a->stereo = stereo;
                                }
                            }
                        }
                        break;

                    case 3:
                        {
                            float image = a->image;
                            image -= 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_SURROUND_IMAGE_STR, image)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_SURROUND_IMAGE_STR, &image)) {
                                    a->image = image;
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
surround_draw_refresh(surround_t *a)
{
    int ctl_w = COLS / SURROUND_MAX_CTL_NUM;

    if (!a || !a->scr)
        return -1;

    WINDOW *scr = a->scr;
    if (COLS >= 100) {
        int len = strlen("  _____          _____   _____             _____ _    _ _____  _____   ____  _    _ _   _ _____  ");
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 3, pos, "  _____          _____   _____             _____ _    _ _____  _____   ____  _    _ _   _ _____  ");
        mvwprintw(scr, 4, pos, " |  __ \\   /\\   |  __ \\ / ____|           / ____| |  | |  __ \\|  __ \\ / __ \\| |  | | \\ | |  __ \\ ");
        mvwprintw(scr, 5, pos, " | |  | | /  \\  | |__) | (___    ______  | (___ | |  | | |__) | |__) | |  | | |  | |  \\| | |  | |");
        mvwprintw(scr, 6, pos, " | |  | |/ /\\ \\ |  _  / \\___ \\  |______|  \\___ \\| |  | |  _  /|  _  /| |  | | |  | | . ` | |  | |");
        mvwprintw(scr, 7, pos, " | |__| / ____ \\| | \\ \\ ____) |           ____) | |__| | | \\ \\| | \\ \\| |__| | |__| | |\\  | |__| |");
        mvwprintw(scr, 8, pos, " |_____/_/    \\_\\_|  \\_\\_____/           |_____/ \\____/|_|  \\_\\_|  \\_\\\\____/ \\____/|_| \\_|_____/ ");
    }
    else {
        char *logo = "DARS-SURROUND";
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

    return 0;
}


static void 
draw_active_ctl(surround_t *a, int act)
{
    int ctl_w = COLS / SURROUND_MAX_CTL_NUM;

    if (!a || !a->scr)
        return;

    mvwprintw(a->scr, LINES-5, ctl_w/2, "SURROUND-STATUS");
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
draw_ratio_ctl(surround_t *a, int act)
{
    char buf[64] = {0,};
    int ctl_w = COLS / SURROUND_MAX_CTL_NUM;

    if (!a || !a->scr) {
        return;
    }

    snprintf(buf, 64, "SURROUND-STEREO:%.2f", a->stereo);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 10.0f;
    conf.value = a->stereo;
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
draw_volume_ctl(surround_t *a, int act)
{
    char buf[64] = {0,};
    int ctl_w = COLS / SURROUND_MAX_CTL_NUM;

    if (!a || !a->scr) {
        return;
    }

    snprintf(buf, 64, "SURROUND-IMAGE:%.2f", a->image);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+2*ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 10.0f;
    conf.value = a->image;
    if (act) {
        attron(attr_active);
        draw_sliderbar(a->scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(a->scr, &conf);
    }

}

