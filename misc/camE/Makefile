srcdir=.

ROOT=
prefix=/usr/local
exec_prefix=${prefix}
bindir=$(ROOT)${exec_prefix}/bin
mandir=$(ROOT)${prefix}/man

CC=gcc
CFLAGS=-O3 -g -Wall -I$(srcdir)/.. -I.. -D_GNU_SOURCE `giblib-config --cflags` -lcurl

INSTALL=/usr/bin/install -c
INSTALL_PROGRAM=${INSTALL}
INSTALL_DATA=${INSTALL} -m 644
INSTALL_DIR=mkdir -p

PROGS=camE

OBJS=webcam.o parseconfig.o

##########################################################################

all build: $(PROGS)

camE: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) `giblib-config --libs`

install: all
	$(INSTALL_DIR) $(bindir)
	$(INSTALL_PROGRAM) camE $(bindir)

clean:
	-rm -f $(OBJS) *.moc *~ core* *.bak TAGS camE

realclean distclean: clean
	-rm -f $(PROGS) Makefile

dep depend install:
