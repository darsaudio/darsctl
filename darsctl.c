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

#if 0
static char* get_pulse_dbus_addr()
{

    DBusError err;
    DBusConnection *session = NULL;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    char *addr = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    const char *intf_STRING = "org.PulseAudio.ServerLookup1";
    const char *arg_STRING = "Address";


    if (addr = getenv("PULSE_DBUS_SERVER")) {
        return addr;
    }
   
    dbus_error_init(&err);
    session = dbus_bus_get(DBUS_BUS_SESSION, &err);

    if (!session) {
        fprintf(stderr, "%s : %s\n", err.name, err.message);
        return NULL;
    }

    msg = dbus_message_new_method_call(
            "org.PulseAudio1", // destination
            "/org/pulseaudio/server_lookup1", // path
            "org.freedesktop.DBus.Properties", // interface
            "Get"
            );
    if (!msg) {
        fprintf(stderr, "can't new method call\n");
        addr = NULL;
        goto ret;
    }
    if (!dbus_message_append_args(msg,  DBUS_TYPE_STRING, &intf_STRING, 
                                        DBUS_TYPE_STRING, &arg_STRING, DBUS_TYPE_INVALID)) {
        fprintf(stderr, "can't add arg\n");
        addr = NULL;
        goto ret;
    }
    reply = dbus_connection_send_with_reply_and_block(session, msg, DBUS_TIMEOUT_INFINITE, &err );
    if (!reply) {
        fprintf(stderr, "send ERROR: %s : %s\n", err.name, err.message);
        addr = NULL;
        goto ret;
    }

    dbus_message_iter_init(reply, &iter);
    int type;
    while ((type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID) {
        if (type == DBUS_TYPE_VARIANT) {
            dbus_message_iter_recurse(&iter, &sub);
            type = dbus_message_iter_get_arg_type(&sub);
            if (type == DBUS_TYPE_STRING) {
                dbus_message_iter_get_basic(&sub, &addr);
                if (addr)
                    addr = strdup(addr);
            }
        }
        dbus_message_iter_next(&iter);
    }

ret :
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);
    if (session)
        dbus_connection_unref(session);
    return addr;
}

static char *get_dars_property(char *addr, const char *name) 
{
    char *value = NULL;
    DBusConnection *conn = NULL;
    DBusError err;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    const char *intf_STRING = "org.PulseAudio.Ext.Dars.Controller";
    const char *arg_STRING = name;

    dbus_error_init(&err);
    conn = dbus_connection_open_private(addr, &err);
    if (!conn) {
        fprintf(stderr, "connection error: %s : %s\n", err.name, err.message);
        value = NULL;
        goto ret;
    }

    msg = dbus_message_new_method_call(
            "org.PulseAudio.Ext.Dars.Controller", // destination
            "/org/pulseaudio/dars", // path
            "org.freedesktop.DBus.Properties", // interface
            "Get"
            );
    if (!msg) {
        fprintf(stderr, "can't new method call\n");
        value = NULL;
        goto ret;
    }

    if (!dbus_message_append_args(msg,  DBUS_TYPE_STRING, &intf_STRING, 
                                        DBUS_TYPE_STRING, &arg_STRING, DBUS_TYPE_INVALID)) {
        fprintf(stderr, "can't add arg\n");
        value = NULL;
        goto ret;
    }

    reply = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_TIMEOUT_INFINITE, &err );
    if (!reply) {
        fprintf(stderr, "send ERROR: %s : %s\n", err.name, err.message);
        value = NULL;
        goto ret;
    }

    dbus_message_iter_init(reply, &iter);
    int type;
    while ((type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID) {
        if (type == DBUS_TYPE_VARIANT) {
            dbus_message_iter_recurse(&iter, &sub);
            type = dbus_message_iter_get_arg_type(&sub);
            if (type == DBUS_TYPE_STRING) {
                dbus_message_iter_get_basic(&sub, &value);
                if (value)
                    value = strdup(value);
            }
        }
        dbus_message_iter_next(&iter);
    }

ret:
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);
    if (conn) {
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
    }
    return value;
}

static char *get_dars_param(char *addr, const char *name)
{
    char *value = NULL;
    DBusConnection *conn = NULL;
    DBusError err;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    const char *intf_STRING = "org.PulseAudio.Ext.Dars.Controller";
    const char *arg_STRING = name;

    dbus_error_init(&err);
    conn = dbus_connection_open_private(addr, &err);
    if (!conn) {
        fprintf(stderr, "connection error: %s : %s\n", err.name, err.message);
        value = NULL;
        goto ret;
    }

    msg = dbus_message_new_method_call(
            "org.PulseAudio.Ext.Dars.Controller", // destination
            "/org/pulseaudio/dars", // path
            "org.PulseAudio.Ext.Dars.Controller", // interface
            "GetParam"
            );
    if (!msg) {
        fprintf(stderr, "can't new method call\n");
        value = NULL;
        goto ret;
    }

    if (!dbus_message_append_args(msg, DBUS_TYPE_STRING, &arg_STRING, DBUS_TYPE_INVALID)) {
        fprintf(stderr, "can't add arg\n");
        value = NULL;
        goto ret;
    }

    reply = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_TIMEOUT_INFINITE, &err );
    if (!reply) {
        fprintf(stderr, "send ERROR: %s : %s\n", err.name, err.message);
        value = NULL;
        goto ret;
    }
    if (!dbus_message_get_args(reply, &err, DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID)) {
        fprintf(stderr, "get args ERROR: %s : %s\n", err.name, err.message);
        value = NULL;
        goto ret;
    }
    value = strdup(value);

ret:
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);
    if (conn) {
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
    }
    return value;
}

static int set_dars_param(char *addr, const char *param, const char *value)
{
    DBusConnection *conn = NULL;
    DBusError err;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    int retcode = -1;

    dbus_error_init(&err);
    conn = dbus_connection_open_private(addr, &err);
    if (!conn) {
        fprintf(stderr, "connection error: %s : %s\n", err.name, err.message);
        retcode = -1;
        goto ret;
    }

    msg = dbus_message_new_method_call(
            "org.PulseAudio.Ext.Dars.Controller", // destination
            "/org/pulseaudio/dars", // path
            "org.PulseAudio.Ext.Dars.Controller", // interface
            "SetParam"
            );
    if (!msg) {
        fprintf(stderr, "can't new method call\n");
        retcode = -1;
        goto ret;
    }

    if (!dbus_message_append_args(msg, 
                DBUS_TYPE_STRING, &param, 
                DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID)) {
        fprintf(stderr, "can't add arg\n");
        retcode = -1;
        goto ret;
    }

    reply = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_TIMEOUT_INFINITE, &err );
    if (!reply) {
        fprintf(stderr, "send ERROR: %s : %s\n", err.name, err.message);
        retcode = -1;
        goto ret;
    }
    retcode = 0;

ret:
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);
    if (conn) {
        dbus_connection_close(conn);
        dbus_connection_unref(conn);
    }
    return retcode;
}
#endif

int
main(int argc, char *argv[])
{
    char *name = NULL;
    char *ver = NULL;
    char ch;
    darsdbus_t *dars_dbus;

    dars_dbus = darsdbus_new();
    if (!dars_dbus) {
        fprintf(stderr, "can't init darsaudio dbus : module-dbus-protocol loaded ?\n");
        exit(EXIT_FAILURE);
    }

    name = darsdbus_get_property(dars_dbus, "Name");
    ver = darsdbus_get_param(dars_dbus, "Version");
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

            darsdbus_set_param(dars_dbus, key, value);
            printf("%s -> %s\n", key, darsdbus_get_param(dars_dbus, key));
            i++;
        }
        exit(EXIT_SUCCESS);
    }

    initscr();
    cbreak();
    noecho();
    ESCDELAY=500;
    keypad(stdscr, TRUE);
    curs_set(0);

    int active_frame = 0;
    
    char title_enable[] = "  DARS AUDIO (Enable) F12 ";
    char title_disable[] = "  DARS AUDIO (Disable) F12 ";
    char title_unknown[] = "  DARS AUDIO (unknown) F12 ";
    char *title;
    char *ge = darsdbus_get_param(dars_dbus, "global-enable");
    if (ge) {
        if (ge[0] == '0') {
            title = title_disable;
        }
        else if (ge[0] == '1') {
            title = title_enable;
        }
        else {
            title = title_unknown;
        }
        free(ge);
    }
    else {
        title = title_unknown;
    }

    for(;;) {
        erase();
        box(stdscr, 0, 0);
        mvprintw(0, COLS/2 - strlen(title)/2, title);
        refresh();
        int ch = getch();
        switch(ch) {
            case 27: // ESC code
                endwin();
                exit(EXIT_SUCCESS);
                break;

            case KEY_F(12): {

                                char *ge = darsdbus_get_param(dars_dbus, "global-enable");
                                if (ge) {
                                    if (ge[0] == '0') {
                                        darsdbus_set_param(dars_dbus, "global-enable", "1");
                                    }
                                    else if (ge[0] == '1') {
                                        darsdbus_set_param(dars_dbus, "global-enable", "0");
                                    }
                                    free(ge);
                                }

                                ge = darsdbus_get_param(dars_dbus, "global-enable");
                                if (ge) {
                                    if (ge[0] == '0') {
                                        title = title_disable;
                                    }
                                    else if (ge[0] == '1') {
                                        title = title_enable;
                                    }
                                    else {
                                        title = title_unknown;
                                    }
                                    free(ge);
                                }
                                else {
                                    title = title_unknown;
                                }
                            }
                break;
        }


    }

    endwin();

    return EXIT_SUCCESS;
}


