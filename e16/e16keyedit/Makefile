# files in source that are needed
PACKAGE  = e16-key-edit
VERSION  = 0.0.0
SRCS     = \
    viewer.c menus.c ipc.c
HEADERS  = \
    viewer.h menus.h
DATA     =
EXTRAS   =
BUILD    = \

LIB_DIRS = \
    -L/usr/local/lib -L/usr/X11R6/lib -L/usr/X11/lib -L/usr/dt/lib
LIBS     = \
    `gtk-config --libs`
CFLAGS   = \
    -O -g -D_GNU_SOURCE $(DEVFLAGS)
#CFLAGS   = \
#    -O -g -W -Wall -Wno-unused  -pedantic -pedantic-errors -ansi -D_GNU_SOURCE
OPTFLAGS   = \
    -O2 -D_GNU_SOURCE
INCLUDES = \
    `gtk-config --cflags` -I..

BIN      = e16keyedit

# configurable stuff
BINARY_DESTINATION = /usr/local/enlightenment/bin
DATA_DESTINATION   = /usr/local/enlightenment/share

# commands to do things
INSTALL  = install
MKDIR    = mkdir
RM       = rm -f
RMRF     = rm -rf
TAR      = tar
GZIP     = gzip
CD       = cd
MAKE     = make
STRIP    = strip
SED      = sed
MAKEDEP  = gcc -MM $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDES) $(EXTRA_INCLUDES)
#MAKEDEP = makedepend -f- -s'\# CUT' -- -Y $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDES) $(EXTRA_INCLUDES) --

# piece together variables
OBJS     = $(SRCS:.c=.o)
DIST     = $(PACKAGE)-$(VERSION)
BLD      = Makefile $(BUILD)
FILES    = $(SRCS) $(HEADERS) $(DATA) $(BLD) $(EXTRAS)

all: e16keyedit

# show help
help:
	@echo
	@echo
#	@echo "To build and install type:"
#	@echo "	make [options] install"
	@echo "To only build the binaries:"
	@echo "	make [options] e16keyedit"
	@echo
	@echo "Options for building (zero or more of the following):"
	@echo "	BINARY_DESTINATION=/path/to/binary/dir [default="$(BINARY_DESTINATION)"]"
	@echo "	DATA_DESTINATION=/path/to/data/dir [default="$(DATA_DESTINATION)"]"
	@echo "	CFLAGS=\"flags for compiler\" [default="$(CFLAGS)"]"
	@echo "	INCLUDES=\"include directiors compiler\" [default="$(INCLUDES)"]"
	@echo "	EXTRA_CFLAGS=\"extra flags for compiler\" [default="$(EXTRA_CFLAGS)"]"
	@echo "	EXTRA_INCLUDES=\"extra include directiors compiler\" [default="$(EXTRA_INCLUDES)"]"
	@echo "	LIBS=\"libraries to link to\" [default="$(LIBS)"]"
	@echo "	LIB_DIRS=\"directories to find libraries\" [default="$(LIB_DIRS)"]"
	@echo "	EXTRA_LIBS=\"extra libraries to link to\" [default="$(EXTRA_LIBS)"]"
	@echo "	EXTRA_LIB_DIRS=\"extra directories to find libraries\" [default="$(EXTRA_LIB_DIRS)"]"
	@echo
	@echo

bigcompile:
	$(CC) $(SRCS) -o e16keyedit $(OPTFLAGS) $(EXTRA_CFLAGS) $(INCLUDES) $(EXTRA_INCLUDES) $(LIBS) $(EXTRA_LIBS) $(LIB_DIRS) $(EXTRA_LIB_DIRS)
	$(STRIP) ./e16keyedit

optimized: bigcompile

# rules for source
e16keyedit:    $(OBJS)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LIBS) $(EXTRA_LIBS) $(LIB_DIRS) $(EXTRA_LIB_DIRS)

.c.o:
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDES) $(EXTRA_INCLUDES) -c $< -o $@

# clean out build crap
clean::
	$(RMRF) $(BIN) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut $(DIST) $(DIST).tar $(DIST).tar.gz ./=build ./=install "#"*

# how to install
install:: e
	@echo 
	@echo 
	@echo "You don't really want to do this. it doesn't work yet."
	@echo 
	@echo 
#	for I in $(BIN); do $(INSTALL) -D -m 0775 $$I $(prefix)$(BINARY_DESTINATION)/$$I; done
#	for I in $(DATA); do $(INSTALL) -D -m 0664 $$I $(prefix)$(DATA_DESTINATION)/$$I; done

# how to make a dist tarball
dist::
	$(RMRF) $(DIST) $(DIST).tar $(DIST).tar.gz
	$(MKDIR) $(DIST)
	for I in $(FILES); do \
	if [ -x $$I ]; then $(INSTALL) -D -m 0775 $$I $(DIST)/$$I; else \
	if [ -f $$I ]; then $(INSTALL) -D -m 0664 $$I $(DIST)/$$I; else \
	if [ -d $$I ]; then $(INSTALL) -D -m 0555 -d $$I $(DIST)/$$I; fi; fi; fi; \
	done 
	$(TAR) -cf $(DIST).tar $(DIST)
	$(GZIP) $(DIST).tar
	$(RMRF) $(DIST)

# how to check the dist
distcheck: dist
	$(RMRF) ./=build ./=install
	$(MKDIR) ./=build
	$(MKDIR) ./=install
	($(CD) ./=build; $(GZIP) -d -c ../$(DIST).tar.gz | $(TAR) xvf -;)
	($(CD) ./=build/$(PACKAGE)-$(VERSION); \
	 $(MAKE) prefix=../../=install install)
	$(RMRF) ./=build ./=install $(DIST).tar.gz

# Update dependency info
dep depend:
	$(SED) '/^# CUT/,$$d' Makefile > Makefile.tmp
	echo "# CUT" >> Makefile.tmp
	$(MAKEDEP) $(SRCS) >> Makefile.tmp && mv Makefile.tmp Makefile

# File dependencies.  Do not remove the next line.  sed needs it.
# CUT
batch.o: batch.c batch.h events.h x.h ev_filter.h str.h mem.h util.h \
 file.h
db.o: db.c db.h mem.h util.h str.h
efm_view.o: efm_view.c x.h str.h mem.h util.h file.h events.h ev_x.h \
 ev_filter.h efm_view.h db.h im_engine.h batch.h glob.h
ev_dispatch.o: ev_dispatch.c ev_dispatch.h events.h x.h str.h mem.h \
 util.h
ev_filter.o: ev_filter.c ev_filter.h events.h x.h str.h mem.h util.h
ev_ipc.o: ev_ipc.c events.h x.h ev_ipc.h str.h mem.h util.h file.h
ev_session.o: ev_session.c str.h mem.h util.h
ev_signal.o: ev_signal.c events.h x.h ev_signal.h str.h mem.h util.h
ev_x.o: ev_x.c events.h x.h ev_x.h str.h mem.h util.h
events.o: events.c events.h x.h ev_filter.h str.h mem.h util.h
file.o: file.c file.h mem.h str.h events.h x.h db.h glob.h ev_ipc.h \
 jobs.h util.h
glob.o: glob.c str.h glob.h
im_engine.o: im_engine.c util.h mem.h str.h x.h events.h ev_x.h \
 im_engine.h db.h
jobs.o: jobs.c jobs.h
main.o: main.c events.h x.h ev_signal.h ev_x.h ev_ipc.h ev_filter.h \
 batch.h efm_view.h file.h db.h im_engine.h mem.h util.h str.h
mem.o: mem.c mem.h
str.o: str.c str.h mem.h
util.o: util.c util.h mem.h str.h
x.o: x.c x.h mem.h str.h
