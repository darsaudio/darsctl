#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "darsaudio.h"
#include "darsdbus.h"
#include "bass.h" 
#include "utils.h"

#define BASS_MAX_CTL_NUM 4

struct bass {
    WINDOW  *scr;
    int     active; /* index 1 */
    int     mode;   /* index 2 */
    float   freq;   /* index 3 */
    float   factor; /* index 4 */
    int active_index;
};

extern darsdbus_t *g_dbus;
extern int attr_active;

static inline void toggle_enable(bass_t *bs)
{
    if (!bs)
        return;
    int enable = bs->active ? 0 : 1;
    if (darsdbus_set_int(g_dbus, DARS_PARAM_BASS_ENABLE_STR, enable) == 0) {
        bs->active = enable;
    }
}

bass_t *
bass_new(WINDOW *scr)
{
    bass_t *bs;
    if (!scr)
        return NULL;

    bs = malloc(sizeof(bass_t));
    if (!bs)
        return NULL;
    memset(bs, 0, sizeof(bass_t));

    bs->scr = scr;
    bs->active_index = 1;
    darsdbus_get_int(g_dbus, DARS_PARAM_BASS_ENABLE_STR, &bs->active);
    darsdbus_get_int(g_dbus, DARS_PARAM_BASS_MODE_STR, &bs->mode);
    darsdbus_get_float(g_dbus, DARS_PARAM_BASS_FREQ_STR, &bs->freq);
    darsdbus_get_float(g_dbus, DARS_PARAM_BASS_FACTOR_STR, &bs->factor);

    return bs;
}

int 
bass_del(bass_t *bs)
{
    if (!bs)
        return -1;

    free(bs);
    return 0;
}


int 
bass_key_handler(bass_t *bs, int key)
{
    WINDOW *scr = NULL;

    if (!bs || !bs->scr)
        return -1;

    scr = bs->scr;

    switch(key) {
        case KEY_LEFT:
            bs->active_index--;
            if (bs->active_index < 1)
                bs->active_index = BASS_MAX_CTL_NUM;

            break;

        case KEY_RIGHT:
            bs->active_index++;
            if (bs->active_index > BASS_MAX_CTL_NUM)
                bs->active_index = 1;

            break;

        case KEY_UP:
            {
                switch(bs->active_index) {
                    case 1:
                        toggle_enable(bs);
                        break;

                    case 2:

                        break;

                    case 3:
                        {
                            float freq = bs->freq;
                            freq += 1.0f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_BASS_FREQ_STR, freq)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_BASS_FREQ_STR, &freq)) {
                                    bs->freq = freq;
                                }
                            }
                        }
                        break;

                    case 4:
                        {
                            float factor = bs->factor;
                            factor += 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_BASS_FACTOR_STR, factor)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_BASS_FACTOR_STR, &factor)) {
                                    bs->factor = factor;
                                }
                            }
                        }
                        break;

                }

            }
            break;
        case KEY_DOWN:
            {
                switch(bs->active_index) {
                    case 1:
                        toggle_enable(bs);
                        break;

                    case 2:
                        break;

                    case 3:
                        {
                            float freq = bs->freq;
                            freq -= 1.0f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_BASS_FREQ_STR, freq)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_BASS_FREQ_STR, &freq)) {
                                    bs->freq = freq;
                                }
                            }
                        }
                        break;

                    case 4:
                        {
                            float factor = bs->factor;
                            factor -= 0.05f;
                            if (0 == darsdbus_set_float(g_dbus, DARS_PARAM_BASS_FACTOR_STR, factor)) {
                                if (0 == darsdbus_get_float(g_dbus, DARS_PARAM_BASS_FACTOR_STR, &factor)) {
                                    bs->factor = factor;
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
bass_draw_refresh(bass_t *bs)
{
    int ctl_w = COLS / BASS_MAX_CTL_NUM;

    if (!bs || !bs->scr)
        return -1;

    WINDOW *scr = bs->scr;
    if (COLS >= 100) {
        int len = strlen("  _____          _____   _____            ____           _____ _____ ");
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 3, pos, "  _____          _____   _____            ____           _____ _____ ");
        mvwprintw(scr, 4, pos, " |  __ \\   /\\   |  __ \\ / ____|          |  _ \\   /\\    / ____/ ____|");
        mvwprintw(scr, 5, pos, " | |  | | /  \\  | |__) | (___    ______  | |_) | /  \\  | (___| (___  ");
        mvwprintw(scr, 6, pos, " | |  | |/ /\\ \\ |  _  / \\___ \\  |______| |  _ < / /\\ \\  \\___ \\\\___ \\ ");
        mvwprintw(scr, 7, pos, " | |__| / ____ \\| | \\ \\ ____) |          | |_) / ____ \\ ____) |___) |");
        mvwprintw(scr, 8, pos, " |_____/_/    \\_\\_|  \\_\\_____/           |____/_/    \\_\\_____/_____/ ");
    }
    else {
        char *logo = "DARS-BASS";
        int len = strlen(logo);
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 2, pos, logo);
    }

    if (bs->active_index == 1) { // Enable
        attron(attr_active);
        mvwprintw(scr, LINES-5, ctl_w/2, "BASS-STATUS");
        if (bs->active) {
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
        mvwprintw(scr, LINES-5, ctl_w/2, "BASS-STATUS");
        if (bs->active) {
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

    if (bs->active_index == 2) { // mode
        attron(attr_active);
        mvwprintw(scr, LINES-5, ctl_w/2+ctl_w, "BASS-MODE");
        if (bs->mode == 0) {
            attron(attr_active | A_BOLD);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(attr_active | A_BOLD);
        }
        else {
            attron(attr_active | A_DIM);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(attr_active | A_DIM);
        }

        /*
        if (bs->mode == 1) {
            attron(attr_active | A_BOLD);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(attr_active | A_BOLD);
        }
        else {
            attron(attr_active | A_DIM);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(attr_active | A_DIM);
        }

        if (bs->mode == 2) {
            attron(attr_active | A_BOLD);
            mvwprintw(scr, LINES-5-6, ctl_w/2+ctl_w, "SUBWOOFER");
            attroff(attr_active | A_BOLD);
        }
        else {
            attron(attr_active | A_DIM);
            mvwprintw(scr, LINES-5-6, ctl_w/2+ctl_w, "SUBWOOFER");
            attroff(attr_active | A_DIM);
        }
        */

        attroff(attr_active);
    }
    else {
        mvwprintw(scr, LINES-5, ctl_w/2+ctl_w, "BASS-MODE");
        if (bs->mode == 0) {
            attron(A_BOLD);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(A_BOLD);
        }
        else {
            attron(A_DIM);
            mvwprintw(scr, LINES-5-2, ctl_w/2+ctl_w, "NATURE");
            attroff(A_DIM);
        }

        /*
        if (bs->mode == 1) {
            attron(A_BOLD);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(A_BOLD);
        }
        else {
            attron(A_DIM);
            mvwprintw(scr, LINES-5-4, ctl_w/2+ctl_w, "OXYGEN");
            attroff(A_DIM);
        }

        if (bs->mode == 2) {
            attron(A_BOLD);
            mvwprintw(scr, LINES-5-6, ctl_w/2+ctl_w, "SUBWOOFER");
            attroff(A_BOLD);
        }
        else {
            attron(A_DIM);
            mvwprintw(scr, LINES-5-6, ctl_w/2+ctl_w, "SUBWOOFER");
            attroff(A_DIM);
        }
        */
    }


    char buf[64] = {0,};
    snprintf(buf, 64, "BASS-FREQ:%.2f", bs->freq);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2+2*ctl_w;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 280.0f;
    conf.value = bs->freq;
    if (bs->active_index == 3) { // freq
        attron(attr_active);
        draw_sliderbar(scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(scr, &conf);
    }

    snprintf(buf, 64, "BASS-FACTOR:%.2f", bs->factor);
    conf.x = ctl_w/2+3*ctl_w;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 20.0f;
    conf.value = bs->factor;
    if (bs->active_index == 4) { // factor
        attron(attr_active);
        draw_sliderbar(scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(scr, &conf);
    }



    return 0;
}
