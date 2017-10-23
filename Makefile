DBUS_CFLAGS=$(shell pkg-config --cflags dbus-1)
DBUS_LDFLAGS=$(shell pkg-config --libs dbus-1)
NCURSES_CFLAGS=$(shell pkg-config --cflags ncurses)
NCURSES_LDFLAGS=$(shell pkg-config --libs ncurses)

SRCS=topbar.c darsdbus.c darsctl.c

all :
	$(CC) -g $(DBUS_CFLAGS) $(NCURSES_CFLAGS) -o darsctl $(SRCS)  $(DBUS_LDFLAGS) $(NCURSES_LDFLAGS)

clean :
	rm -f *.o darsctl
