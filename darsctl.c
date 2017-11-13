#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <dbus/dbus.h>
#include <ncurses.h>

#include "darsdbus.h"
#include "topbar.h"
#include "volume.h"
#include "bass.h"
#include "clarity.h"
#include "surround.h"
#include "agc.h"
#include "eq.h"

#define TOPBAR_CTL_NUM 10

int g_index = 0;
topbar_t *g_topbar = NULL;
darsdbus_t *g_dbus = NULL;
volume_t *g_volume = NULL;
bass_t *g_bass = NULL;
clarity_t *g_clarity = NULL;
surround_t *g_surround = NULL;
agc_t *g_agc = NULL;
eq_t *g_eq = NULL;

int attr_active;
int darsaudio_global_enable;

static int main_draw_refresh(WINDOW *);
static int global_enable_refresh(WINDOW *);

int
main(int argc, char *argv[])
{
    char *name = NULL;
    char *ver = NULL;
    char ch;
    int ret;

    /*
     * check the is the driver loaded?
     */

    ret = system("pacmd list-modules | grep module-dbus-protocol 1>/dev/null 2>&1 ");
    if (ret) {
        system("pacmd load-module module-dbus-protocol 1>/dev/null 2>&1");
    }

    ret = system("pacmd list-modules | grep module-dars-sink 1>/dev/null 2>&1");
    if (ret) {
        system("pacmd load-module module-dars-sink 1>/dev/null 2>&1");
    }

    g_dbus = darsdbus_new();
    if (!g_dbus) {
        fprintf(stderr, "can't init darsaudio dbus : module-dbus-protocol loaded ?\n");
        exit(EXIT_FAILURE);
    }

    name = darsdbus_get_property(g_dbus, "Name");
    ver = darsdbus_get_property(g_dbus, "Version");
    if (!name || ! ver) {
        fprintf(stderr, "can't darsaudio's info : module-dars-sink loaded ?\n");
        exit(EXIT_FAILURE);
    }


    while((ch = getopt(argc, argv, "h")) != -1) {
        switch (ch) {
            case 'h':
                printf("help!\n");
                break;
            default:
                printf("Error arg\n");
                exit(EXIT_FAILURE);
        }
    }
    if (argc > optind) {
        int i = optind;
        while (i < argc) {
            char *key;
            char *value;
            if(strchr(argv[i], '=') == NULL) {
                i++;
                continue;
            }

            key = strtok(argv[i], "=");
            value = strtok(NULL, "=");

            darsdbus_set_param(g_dbus, key, value);
            printf("%s -> %s\n", key, darsdbus_get_param(g_dbus, key));
            i++;
        }
        exit(EXIT_SUCCESS);
    }

    initscr();
    cbreak();
    noecho();
    ESCDELAY=500;
    keypad(stdscr, TRUE);
    curs_set(0); // hide cursor
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_RED);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        init_pair(3, COLOR_WHITE, COLOR_BLUE); // window background
        attr_active = COLOR_PAIR(1);

        wbkgd(stdscr, COLOR_PAIR(3));
    }

    int active_frame = 0;

    g_topbar = topbar_new(stdscr);
    g_volume = volume_new(stdscr);
    g_bass = bass_new(stdscr);
    g_clarity = clarity_new(stdscr);
    g_surround = surround_new(stdscr);
    g_agc = agc_new(stdscr);
    g_eq = eq_new(stdscr);

    { // check darsaudio global enabel?
        char * ge = darsdbus_get_param(g_dbus, "global-enable");
        if (!ge) {
            darsaudio_global_enable = 0;
        }
        else {
            int e = atoi(ge);
            darsaudio_global_enable = e ? 1 : 0;
            free(ge);
        }

        if (darsaudio_global_enable) {
            attr_active = COLOR_PAIR(1);
        }
        else {
            attr_active = COLOR_PAIR(2);
        }
    }
    
    for(;;) {
        erase();
        topbar_draw_refresh(g_topbar);
        global_enable_refresh(stdscr);

        // draw controls
        switch(g_index) {
            case 0:
                main_draw_refresh(stdscr);
                break;
            case 1:
                volume_draw_refresh(g_volume);
                break;

            case 2:
                eq_draw_refresh(g_eq);
                break;

            case 3:
                bass_draw_refresh(g_bass);
                break;

            case 4:
                clarity_draw_refresh(g_clarity);
                break;

            case 5:
                surround_draw_refresh(g_surround);
                break;

            case 6:
                agc_draw_refresh(g_agc);
                break;

            default :
                break;

        }


        int ch = getch();
        switch(ch) {
            case 27: // ESC code
                endwin();
                exit(EXIT_SUCCESS);

            case ' ': // toggle global enable
                {
                    char * ge = darsdbus_get_param(g_dbus, "global-enable");
                    char buf[8] = {0,};
                    if (!ge) {
                        darsaudio_global_enable = 0;
                    }
                    else {
                        int e = atoi(ge);
                        darsaudio_global_enable = e ? 0 : 1;
                        free(ge);

                        snprintf(buf, 8, "%d", darsaudio_global_enable);
                        darsdbus_set_param(g_dbus, "global-enable", buf);
                    }

                    if (darsaudio_global_enable) {
                        attr_active = COLOR_PAIR(1);
                    }
                    else {
                        attr_active = COLOR_PAIR(2);
                    }
                }
                break;

            case KEY_HOME:
                topbar_set_active_index(g_topbar, 0);
                g_index = 0;
                break;

            case KEY_NPAGE:
                g_index++;
                if (g_index > TOPBAR_CTL_NUM) {
                    g_index = TOPBAR_CTL_NUM;
                }
                topbar_set_active_index(g_topbar, g_index);
                break;

            case KEY_PPAGE:
                g_index--;
                if (g_index < 0) {
                    g_index = 0;
                }
                topbar_set_active_index(g_topbar, g_index);
                break;

            case KEY_F(1):
            case 'V':
                {
                    topbar_set_active_index(g_topbar, 1);
                    g_index = 1;
                }
                continue;
            case KEY_F(2):
            case 'E':
                {
                    topbar_set_active_index(g_topbar, 2);
                    g_index = 2;
                }
                continue;
            case KEY_F(3):
            case 'B':
                {
                    topbar_set_active_index(g_topbar, 3);
                    g_index = 3;
                }
                continue;
            case KEY_F(4):
            case 'T':
                {
                    topbar_set_active_index(g_topbar, 4);
                    g_index = 4;
                }
                continue;
            case KEY_F(5):
            case 'S':
                {
                    topbar_set_active_index(g_topbar, 5);
                    g_index = 5;
                }
                continue;
            case KEY_F(6):
            case 'A':
                {
                    topbar_set_active_index(g_topbar, 6);
                    g_index = 6;
                }
                continue;
            case KEY_F(7):
            case 'C':
                {
                    topbar_set_active_index(g_topbar, 7);
                    g_index = 7;
                }
                continue;
            case KEY_F(8):
            case 'R':
                {
                    topbar_set_active_index(g_topbar, 8);
                    g_index = 8;
                }
                continue;
            case KEY_F(9):
            case 'I':
                {
                    topbar_set_active_index(g_topbar, 9);
                    g_index = 9;
                }
                continue;
            case KEY_F(10):
            case 'M':
                {
                    topbar_set_active_index(g_topbar, 10);
                    g_index = 10;
                }
                continue;

            default:
                break;
        }

        // key handler
        switch (g_index) {
            case 1:
                volume_key_handler(g_volume, ch);
                volume_draw_refresh(g_volume);
                break;
            case 2:
                eq_key_handler(g_eq, ch);
                eq_draw_refresh(g_eq);
                break;

            case 3:
                bass_key_handler(g_bass, ch);
                bass_draw_refresh(g_bass);
                break;

            case 4:
                clarity_key_handler(g_clarity, ch);
                clarity_draw_refresh(g_clarity);
                break;

            case 5:
                surround_key_handler(g_surround, ch);
                surround_draw_refresh(g_surround);
                break;

            case 6:
                agc_key_handler(g_agc, ch);
                agc_draw_refresh(g_agc);
                break;

            default :
                break;
        }

    }

    topbar_del(g_topbar);
    volume_del(g_volume);
    darsdbus_del(g_dbus);
    endwin();

    return EXIT_SUCCESS;
}



static int 
main_draw_refresh(WINDOW *scr)
{
    if (!scr)
        return -1;


    int len = strlen("  _____          _____   _____           _    _ _____ _____ ____  ");
    if (COLS >= len) {

        mvwprintw(scr, 4, COLS/2 - len/2, "  _____          _____   _____           _    _ _____ _____ ____  ");
        mvwprintw(scr, 5, COLS/2 - len/2, " |  __ \\   /\\   |  __ \\ / ____|     /\\  | |  | |  __ \\_   _/ __ \\ ");
        mvwprintw(scr, 6, COLS/2 - len/2, " | |  | | /  \\  | |__) | (___      /  \\ | |  | | |  | || || |  | |");
        mvwprintw(scr, 7, COLS/2 - len/2, " | |  | |/ /\\ \\ |  _  / \\___ \\    / /\\ \\| |  | | |  | || || |  | |");
        mvwprintw(scr, 8, COLS/2 - len/2, " | |__| / ____ \\| | \\ \\ ____) |  / ____ \\ |__| | |__| || || |__| |");
        mvwprintw(scr, 9, COLS/2 - len/2, " |_____/_/    \\_\\_|  \\_\\_____/  /_/    \\_\\____/|_____/_____\\____/ ");

    }
    else {
        char *str = "DarsAudio";
        int l = strlen(str);
        mvwprintw(scr, 4, COLS/2 - l/2, str);
    }

    return 0;

}

static int 
global_enable_refresh(WINDOW *scr)
{
    char *str1 = "DarsAudio GlobalEnabled [SPACE BAR]";
    char *str2 = "DarsAudio GlobalDisabled [SPACE BAR]";
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    if (!scr)
        return -1;


    attron(attr_active);
    if (darsaudio_global_enable)
        mvwprintw(scr, LINES-1, COLS/2 - len1/2, str1);
    else
        mvwprintw(scr, LINES-1, COLS/2 - len2/2, str2);
    attroff(attr_active);


    wrefresh(scr);
    return 0;
}
