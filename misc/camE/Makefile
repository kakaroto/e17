srcdir=.

ROOT=
prefix=/usr/local
exec_prefix=${prefix}
bindir=$(ROOT)${exec_prefix}/bin
mandir=$(ROOT)${prefix}/man

CC=gcc
CFLAGS=-O2 -Wall -I$(srcdir)/.. -I.. -D_GNU_SOURCE -I/usr/local/include -I/usr/X11R6/include

INSTALL=/usr/bin/install -c
INSTALL_PROGRAM=${INSTALL}
INSTALL_DATA=${INSTALL} -m 644
INSTALL_DIR=mkdir -p

PROGS=camE

OBJS=webcam.o ftp.o parseconfig.o

##########################################################################

all build: $(PROGS)

camE: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -L/usr/local/lib -lImlib2 -lttf -lm -ldl -lXext -lXext -L/usr/X11R6/lib

install: all
	$(INSTALL_DIR) $(bindir)
	$(INSTALL_PROGRAM) -s camE $(bindir)

clean:
	-rm -f $(OBJS) *.moc *~ core* *.bak TAGS camE

realclean distclean: clean
	-rm -f $(PROGS) Makefile

dep depend install:
