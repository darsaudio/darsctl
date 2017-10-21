#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

#include "darsdbus.h"


struct darsdbus {
    char *addr;
    DBusConnection *conn;

};

darsdbus_t *
darsdbus_new()
{
    DBusError err;
    int ret = 0;
    DBusConnection *session = NULL;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    char *addr = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    const char *intf_STRING = "org.PulseAudio.ServerLookup1";
    const char *arg_STRING = "Address";

    darsdbus_t *bus = malloc(sizeof darsdbus_t);
    if (!bus)
        return NULL;

    memset(bus, 0, sizeof darsdbus_t);
    dbus_error_init(&err);

    /*
     * check the ENV
     */
    addr = getenv("PULSE_DBUS_SERVER");
    if (addr) {
        bus->addr = strdup(addr);
    }
    else {
        // get addr from session dbus
        session = dbus_bus_get(DBUS_BUS_SESSION, &err);
        if (!session) {
            ret = -1;
            goto out;
        }

        msg = dbus_message_new_method_call(
                "org.PulseAudio1", // destination
                "/org/pulseaudio/server_lookup1", // path
                "org.freedesktop.DBus.Properties", // interface
                "Get"
                );
        if (!msg) {
            ret = -1;
            goto out;
        }
        if (!dbus_message_append_args(msg,  DBUS_TYPE_STRING, &intf_STRING, 
                    DBUS_TYPE_STRING, &arg_STRING, DBUS_TYPE_INVALID)) {
            ret = -1;
            goto out;
        }
        reply = dbus_connection_send_with_reply_and_block(session, msg, DBUS_TIMEOUT_INFINITE, &err );
        if (!reply) {
            ret = -1;
            goto out;
        }

        dbus_message_iter_init(reply, &iter);
        int type;
        while ((type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID) {
            if (type == DBUS_TYPE_VARIANT) {
                dbus_message_iter_recurse(&iter, &sub);
                type = dbus_message_iter_get_arg_type(&sub);
                if (type == DBUS_TYPE_STRING) {
                    dbus_message_iter_get_basic(&sub, &addr);
                    if (addr) {
                        bus->addr = strdup(addr);
                        break;
                    }
                }
            }
            dbus_message_iter_next(&iter);
        }
    }
    if (!bus->addr) {
        ret = -1;
        goto out;
    }

    bus->conn = dbus_connection_open_private(bus->addr, &err);
    if (!bus->conn) {
        ret = -1;
        goto out;
    }

    // SUCCESS
    ret = 0;

out:
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);
    if (session)
        dbus_connection_unref(session);
    if (ret) {
        // Error
        if (bus) {
            if (bus->addr) {
                free(bus->addr);
            }
            if (bus->conn) {
                dbus_connection_close(bus->conn);
                dbus_connection_unref(bus->conn);
            }
            free(bus);
        }
        return NULL;
    }

    return bus;
}

int 
darsdbus_del(darsdbus_t *bus)
{
    if (!bus)
        return -1;

    if (bus->addr) {
        free(bus->addr);
        bus->addr = NULL;
    }

    if (bus->conn) {
        dbus_connection_close(bus->conn);
        dbus_connection_unref(bus->conn);
        bus->conn = NULL;
    }

    free(bus);
    return 0;
}

char *
darsdbus_get_property(darsdbus_t *bus, const char *name)
{
    char *value = NULL;
    DBusError err;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    const char *intf_STRING = "org.PulseAudio.Ext.Dars.Controller";

    if (!bus || !name || !bus->conn)
        return NULL;

    dbus_error_init(&err);

    msg = dbus_message_new_method_call(
            "org.PulseAudio.Ext.Dars.Controller", // destination
            "/org/pulseaudio/dars", // path
            "org.freedesktop.DBus.Properties", // interface
            "Get"
            );
    if (!msg) {
        value = NULL;
        goto out;
    }

    if (!dbus_message_append_args(msg,  DBUS_TYPE_STRING, &intf_STRING, 
                                        DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID)) {
        value = NULL;
        goto out;
    }

    reply = dbus_connection_send_with_reply_and_block(bus->conn, msg, DBUS_TIMEOUT_INFINITE, &err );
    if (!reply) {
        value = NULL;
        goto out;
    }

    dbus_message_iter_init(reply, &iter);
    int type;
    while ((type = dbus_message_iter_get_arg_type(&iter)) != DBUS_TYPE_INVALID) {
        if (type == DBUS_TYPE_VARIANT) {
            dbus_message_iter_recurse(&iter, &sub);
            type = dbus_message_iter_get_arg_type(&sub);
            if (type == DBUS_TYPE_STRING) {
                dbus_message_iter_get_basic(&sub, &value);
                if (value) {
                    value = strdup(value);
                    break;
                }
            }
        }
        dbus_message_iter_next(&iter);
    }

out:
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);

    return value;
}

char *
darsdbus_get_param(darsdbus_t *bus, const char *name)
{
    char *value = NULL;
    DBusError err;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    const char *intf_STRING = "org.PulseAudio.Ext.Dars.Controller";

    if (!bus || !name || !bus->conn)
        return NULL;

    dbus_error_init(&err);
    msg = dbus_message_new_method_call(
            "org.PulseAudio.Ext.Dars.Controller", // destination
            "/org/pulseaudio/dars", // path
            "org.PulseAudio.Ext.Dars.Controller", // interface
            "GetParam"
            );
    if (!msg) {
        value = NULL;
        goto out;
    }

    if (!dbus_message_append_args(msg, DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID)) {
        value = NULL;
        goto out;
    }

    reply = dbus_connection_send_with_reply_and_block(bus->conn, msg, DBUS_TIMEOUT_INFINITE, &err );
    if (!reply) {
        value = NULL;
        goto out;
    }
    if (!dbus_message_get_args(reply, &err, DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID)) {
        value = NULL;
        goto out;
    }
    value = strdup(value);

out:
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);
    return value;
}

int 
darsdbus_set_param(darsdbus_t *bus, const char *param, const char *value)
{
    DBusError err;
    DBusMessage *msg = NULL;
    DBusMessage *reply = NULL;
    DBusMessageIter iter;
    DBusMessageIter sub;
    int retcode = -1;

    if (!bus || !name || !value || !bus->conn)
        return NULL;

    dbus_error_init(&err);
    msg = dbus_message_new_method_call(
            "org.PulseAudio.Ext.Dars.Controller", // destination
            "/org/pulseaudio/dars", // path
            "org.PulseAudio.Ext.Dars.Controller", // interface
            "SetParam"
            );
    if (!msg) {
        retcode = -1;
        goto out;
    }

    if (!dbus_message_append_args(msg, 
                DBUS_TYPE_STRING, &param, 
                DBUS_TYPE_STRING, &value, DBUS_TYPE_INVALID)) {
        retcode = -1;
        goto out;
    }

    reply = dbus_connection_send_with_reply_and_block(bus->conn, msg, DBUS_TIMEOUT_INFINITE, &err );
    if (!reply) {
        retcode = -1;
        goto out;
    }
    retcode = 0;

out:
    if (msg)
        dbus_message_unref(msg);
    if (reply)
        dbus_message_unref(reply);

    return retcode;
}
