# Copyright (C) 1999 Carsten Haitzler and Simon Horman
#  
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#   
# The above copyright notice and this permission notice shall be included in
# all copies of the Software, its documentation and marketing & publicity 
# materials, and acknowledgment shall be given in the documentation, materials
# and software packages that this Software was used.
#    
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

######################################################################
# Change these at will
######################################################################

PACKAGE  = lvs-gui
MAJOR    = 0
MINOR    = 0
PATCH    = 0
RELEASE  = 1

SRCS     = lvs-gui.c
EXTRA_DIST = \
  Makefile \
  COPYING \
  etc/rc.d/init.d/transparent_proxy \
  etc/rc.d/init.d/ipvs

######################################################################
# No user servicable parts below here
######################################################################

DISTNAME     = $(PACKAGE)-$(MAJOR).$(MINOR).$(PATCH)
RPMDISTNAME  = $(PACKAGE)-$(MAJOR).$(MINOR).$(PATCH)-$(RELEASE)
DISTFILES    = $(SRCS) $(EXTRA_DIST)


CP       = cp
ECHO     = echo
GZIP     = gzip --best
INSTALL  = install
MKDIR    = mkdir
RM       = rm
TAR      = tar


OBJS     = $(SRCS:.c=.o)
PREFIX   = /usr
BINDIR   = $(BUILD_ROOT)/$(PREFIX)/bin
ETCDIR   = /etc
BIN      = lvs-gui
LIBS     = -L/usr/lcoal/lib `gtk-config --libs` `imlib-config --libs-gdk`

.PHONY = clean install dist all

all: lvs-gui

lvs-gui: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	$(RM) -f $(BIN) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut  "#"*

.c.o:
	$(CC) -I/usr/local/include `gtk-config --cflags` `imlib-config --cflags-gdk` -O2 -c $< -o $@

install: lvs-gui
	if [ ! -e $(BINDIR) ]; then $(MKDIR) -p $(BINDIR); fi
	$(INSTALL) $(BIN)      $(BINDIR)

dist:
	$(RM) -rf $(DISTNAME)/
	$(MKDIR) $(DISTNAME)/
	@for i in $(DISTFILES); do \
		dir=`dirname $$i`; \
                file=`basename $$i`; \
		echo $$i $$dir $$file; \
		if [ ! -e $(DISTNAME)/$$dir ]; then \
			$(MKDIR) -p $(DISTNAME)/$$dir; \
		fi; \
		$(CP) $$i $(DISTNAME)/$$dir; \
	done
	$(TAR) -cf $(DISTNAME).tar $(DISTNAME)/
	$(RM) -f $(DISTNAME).tar.gz
	$(GZIP) $(DISTNAME).tar
	$(RM) -rf $(DISTNAME)/
	@$(ECHO) "############################################################"
	@$(ECHO) "# $(DISTNAME).tar.gz is ready for distribution"
	@$(ECHO) "############################################################"
