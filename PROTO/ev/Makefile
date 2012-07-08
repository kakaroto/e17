PREFIX ?= /usr/local
DOCDIR ?= $(PREFIX)/share/doc

CC     ?= gcc
CFLAGS ?= -O0 -pipe -Wall -Wextra -g
SHELL   = /bin/sh
LIBS    = $(shell pkg-config --cflags --libs elementary)

ev: ev.c
	$(CC) $(CFLAGS) $(LIBS) -o $@ $<

install: ev
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m755 $< $(DESTDIR)$(PREFIX)/bin/
	mkdir -p $(DESTDIR)$(DOCDIR)
	install -m644 AUTHORS COPYING README $(DESTDIR)$(DOCDIR)/

.PHONY: clean

clean:
	rm -f ev *.o
