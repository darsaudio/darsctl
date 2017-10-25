#include <stdio.h>
#include "volume.h"
#include "darsdbus.h"
#include "utils.h"

struct volume {
    WINDOW *scr;
    float   pre_volume;
    float   post_volume;
    float   pan;
    int     active_index;   /* 1 pre_volume */
                            /* 2 pan  */
                            /* 3 post_volume  */
};

extern darsdbus_t *g_dbus;
extern int attr_active;

volume_t *
volume_new(WINDOW *scr)
{
    volume_t *vm = NULL;
    char *v;

    if (!scr)
        return NULL;

    vm = malloc(sizeof(volume_t));
    if (!vm)
        return NULL;
    memset(vm, 0, sizeof(volume_t));

    vm->scr = scr;
    vm->active_index = 1;

    v = darsdbus_get_param(g_dbus, "volume-before-dars");
    if (!v) {
        vm->pre_volume = 0.0f;
    }
    else {
        if (sscanf(v, "%f", &vm->pre_volume) < 0) {
            vm->pre_volume = 0.0f;
        }
        free(v);
    }

    v = darsdbus_get_param(g_dbus, "volume-after-dars");
    if (!v) {
        vm->post_volume = 0.0f;
    }
    else {
        if (sscanf(v, "%f", &vm->post_volume) < 0) {
            vm->post_volume = 0.0f;
        }
        free(v);
    }

    v = darsdbus_get_param(g_dbus, "channels-pan");
    if (!v) {
        vm->pan = 0.0f;
    }
    else {
        if (sscanf(v, "%f", &vm->pan) < 0) {
            vm->pan = 0.0f;
        }
        free(v);
    }

    return vm;
}



int volume_del(volume_t *vm)
{
    if (!vm)
        return -1;

    vm->scr = NULL;
    free(vm);
    return 0;
}

int 
volume_key_handler(volume_t *vm, int key)
{
    WINDOW *scr = NULL;

    if (!vm || !vm->scr)
        return -1;

    scr = vm->scr;

    switch (key) {
        case KEY_LEFT:
            vm->active_index--;
            if (vm->active_index < 1) {
                vm->active_index = 3;
            }
            break;
        case KEY_RIGHT:
            vm->active_index++;
            if (vm->active_index > 3) {
                vm->active_index = 1;
            }
            break;

        case KEY_UP:
            {
                switch(vm->active_index) {
                    case 1: // pre volume
                        {
                            float v = vm->pre_volume;
                            char vbuf[32] = {0,};
                            char *vstr = NULL;
                            v += 0.05f;
                            snprintf(vbuf, 32, "%f", v);
                            if (darsdbus_set_param(g_dbus, "volume-before-dars", vbuf) == 0) {
                                vstr = darsdbus_get_param(g_dbus, "volume-before-dars");
                                if (vstr) {
                                    vm->pre_volume = strtof(vstr, NULL);
                                    free(vstr);
                                }
                            }
                        }
                        break;
                    case 2: // pan
                        {
                            float v = vm->pan;
                            char vbuf[32] = {0,};
                            char *vstr = NULL;
                            v += 0.05f;
                            snprintf(vbuf, 32, "%f", v);
                            if (darsdbus_set_param(g_dbus, "channels-pan", vbuf) == 0) {
                                vstr = darsdbus_get_param(g_dbus, "channels-pan");
                                if (vstr) {
                                    vm->pan = strtof(vstr, NULL);
                                    free(vstr);
                                }
                            }
                        }
                        break;
                    case 3: // post volume
                        {
                            float v = vm->post_volume;
                            char vbuf[32] = {0,};
                            char *vstr = NULL;
                            v += 0.05f;
                            snprintf(vbuf, 32, "%f", v);
                            if (darsdbus_set_param(g_dbus, "volume-after-dars", vbuf) == 0) {
                                vstr = darsdbus_get_param(g_dbus, "volume-after-dars");
                                if (vstr) {
                                    vm->post_volume = strtof(vstr, NULL);
                                    free(vstr);
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            break;

        case KEY_DOWN:
            {
                switch(vm->active_index) {
                    case 1: // pre volume
                        {
                            float v = vm->pre_volume;
                            char vbuf[32] = {0,};
                            char *vstr = NULL;
                            v -= 0.05f;
                            snprintf(vbuf, 32, "%f", v);
                            if (darsdbus_set_param(g_dbus, "volume-before-dars", vbuf) == 0) {
                                vstr = darsdbus_get_param(g_dbus, "volume-before-dars");
                                if (vstr) {
                                    vm->pre_volume = strtof(vstr, NULL);
                                    free(vstr);
                                }
                            }
                        }
                        break;
                    case 2:
                        {
                            float v = vm->pan;
                            char vbuf[32] = {0,};
                            char *vstr = NULL;
                            v -= 0.05f;
                            snprintf(vbuf, 32, "%f", v);
                            if (darsdbus_set_param(g_dbus, "channels-pan", vbuf) == 0) {
                                vstr = darsdbus_get_param(g_dbus, "channels-pan");
                                if (vstr) {
                                    vm->pan = strtof(vstr, NULL);
                                    free(vstr);
                                }
                            }
                        }
                        break;
                    case 3: // post volume
                        {
                            float v = vm->post_volume;
                            char vbuf[32] = {0,};
                            char *vstr = NULL;
                            v -= 0.05f;
                            snprintf(vbuf, 32, "%f", v);
                            if (darsdbus_set_param(g_dbus, "volume-after-dars", vbuf) == 0) {
                                vstr = darsdbus_get_param(g_dbus, "volume-after-dars");
                                if (vstr) {
                                    vm->post_volume = strtof(vstr, NULL);
                                    free(vstr);
                                }
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            break;

        default:
            break;
    }


    return 0;
}

int 
volume_draw_refresh(volume_t *vm)
{
    int ctl_w = COLS / 3;

    if (!vm || !vm->scr)
        return -1;

    WINDOW *scr = vm->scr;
    // draw volume LOGO
    if (COLS >= 100) {
        int len = strlen("  _____          _____   _____           __      ______  _     _    _ __  __ ______ ");
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 3, pos, "  _____          _____   _____           __      ______  _     _    _ __  __ ______ ");
        mvwprintw(scr, 4, pos, " |  __ \\   /\\   |  __ \\ / ____|          \\ \\    / / __ \\| |   | |  | |  \\/  |  ____|");
        mvwprintw(scr, 5, pos, " | |  | | /  \\  | |__) | (___    ______   \\ \\  / / |  | | |   | |  | | \\  / | |__   ");
        mvwprintw(scr, 6, pos, " | |  | |/ /\\ \\ |  _  / \\___ \\  |______|   \\ \\/ /| |  | | |   | |  | | |\\/| |  __|  ");
        mvwprintw(scr, 7, pos, " | |__| / ____ \\| | \\ \\ ____) |             \\  / | |__| | |___| |__| | |  | | |____ ");
        mvwprintw(scr, 8, pos, " |_____/_/    \\_\\_|  \\_\\_____/               \\/   \\____/|______\\____/|_|  |_|______|");
    }
    else {
        char *logo = "DARS-VOLUME";
        int len = strlen(logo);
        int pos = COLS/2 - len/2;
        mvwprintw(scr, 2, pos, logo);
    }

    char buf[64] = {0,};
    snprintf(buf, 64, "PRE-VOLUME:%.2f", vm->pre_volume);

    sliderbar_conf_t conf;
    conf.y = LINES - 5;
    conf.x = ctl_w/2;
    conf.h = LINES/2;
    conf.label = buf;
    conf.min = 0.0f;
    conf.max = 10.0f;
    conf.value = vm->pre_volume;
    if (vm->active_index == 1) {
        attron(attr_active);
        draw_sliderbar(scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(scr, &conf);
    }

    snprintf(buf, 64, "CHANNELS-PAN:%.2f", vm->pan);
    conf.x = ctl_w/2 + ctl_w;
    conf.label = buf;
    conf.min = -1.0f;
    conf.max = 1.0f;
    conf.value = vm->pan;
    if (vm->active_index == 2) {
        attron(attr_active);
        draw_sliderbar(scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(scr, &conf);
    }

    conf.x = ctl_w/2 + 2*ctl_w;
    snprintf(buf, 64, "POST-VOLUME:%.2f", vm->post_volume);
    conf.max = 10;
    conf.min = 0;
    conf.label = buf;
    conf.value = vm->post_volume;
    if (vm->active_index == 3) {
        attron(attr_active);
        draw_sliderbar(scr, &conf);
        attroff(attr_active);
    }
    else {
        draw_sliderbar(scr, &conf);
    }

    wrefresh(scr);
    return 0;
}
