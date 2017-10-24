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

int g_index = 0;
topbar_t *g_topbar = NULL;
darsdbus_t *g_dbus = NULL;
volume_t *g_volume = NULL;

int attr_active;

static int main_draw_refresh(WINDOW *);

int
main(int argc, char *argv[])
{
    char *name = NULL;
    char *ver = NULL;
    char ch;

    g_dbus = darsdbus_new();
    if (!g_dbus) {
        fprintf(stderr, "can't init darsaudio dbus : module-dbus-protocol loaded ?\n");
        exit(EXIT_FAILURE);
    }

    name = darsdbus_get_property(g_dbus, "Name");
    ver = darsdbus_get_param(g_dbus, "Version");
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
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        attr_active = COLOR_PAIR(1);
    }

    int active_frame = 0;

    g_topbar = topbar_new(stdscr);
    g_volume = volume_new(stdscr);
    
    for(;;) {
        erase();
        topbar_draw_refresh(g_topbar);

        // draw controls
        switch(g_index) {
            case 0:
                main_draw_refresh(stdscr);
                break;
            case 1:
                volume_draw_refresh(g_volume);
                break;

            default :
                break;

        }


        int ch = getch();
        switch(ch) {
            case 27: // ESC code
                endwin();
                exit(EXIT_SUCCESS);

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
                }
                continue;
            case KEY_F(3):
            case 'B':
                {
                    topbar_set_active_index(g_topbar, 3);
                }
                continue;
            case KEY_F(4):
            case 'T':
                {
                    topbar_set_active_index(g_topbar, 4);
                }
                continue;
            case KEY_F(5):
            case 'S':
                {
                    topbar_set_active_index(g_topbar, 5);
                }
                continue;
            case KEY_F(6):
            case 'A':
                {
                    topbar_set_active_index(g_topbar, 6);
                }
                continue;
            case KEY_F(7):
            case 'C':
                {
                    topbar_set_active_index(g_topbar, 7);
                }
                continue;
            case KEY_F(8):
            case 'R':
                {
                    topbar_set_active_index(g_topbar, 8);
                }
                continue;
            case KEY_F(9):
            case 'I':
                {
                    topbar_set_active_index(g_topbar, 9);
                }
                continue;
            case KEY_F(10):
            case 'M':
                {
                    topbar_set_active_index(g_topbar, 10);
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

}
