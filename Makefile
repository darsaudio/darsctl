DBUS_CFLAGS=$(shell pkg-config --cflags dbus-1)
DBUS_LDFLAGS=$(shell pkg-config --libs dbus-1)
NCURSES_CFLAGS=$(shell pkg-config --cflags ncurses)
NCURSES_LDFLAGS=$(shell pkg-config --libs ncurses)

all :
	$(CC) -g $(DBUS_CFLAGS) $(NCURSES_CFLAGS) -o darsctl topbar.c darsdbus.c darsctl.c $(DBUS_LDFLAGS) $(NCURSES_LDFLAGS)

clean :
	rm -f *.o darsctl
