PREFIX ?= /usr/local
DOCDIR ?= $(PREFIX)/share/doc/ev

CC     ?= gcc
CFLAGS ?= -O0 -pipe -Wall -Wextra -g
SHELL   = /bin/sh
LIBS    = $(shell pkg-config --cflags --libs elementary eio)

ev: ev.c
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)

install: ev
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m755 $< $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(DOCDIR)
	install -m644 AUTHORS COPYING README $(DESTDIR)$(DOCDIR)/
	mkdir -p $(DESTDIR)$(PREFIX)/share/applications
	install -m644 ev.desktop $(DESTDIR)$(PREFIX)/share/applications/
	mkdir -p $(DESTDIR)$(PREFIX)/share/icons
	install -m644 ev.png $(DESTDIR)$(PREFIX)/share/icons/

.PHONY: clean uninstall

clean:
	rm -f ev *.o

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/ev
	rm -f $(DESTDIR)$(DOCDIR)/*
	rmdir $(DESTDIR)$(DOCDIR)
	rm -f $(DESTDIR)$(PREFIX)/share/applications/ev.desktop
	rm -f $(DESTDIR)$(PREFIX)/share/icons/ev.png
