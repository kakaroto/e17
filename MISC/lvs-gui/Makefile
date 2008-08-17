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
PATCH    = 2
RELEASE  = 1

SRCS         = lvs-gui.c options.c config_file.c dynamic_array.c util.c \
               callback.c
HDRS         = options.h config_file.h dynamic_array.h lvs-gui.h \
               lvs-gui_types.h util.h callback.h
INIT_SCRIPTS = etc/rc.d/init.d/transparent_proxy etc/rc.d/init.d/ipvs
EXTRA_DIST   = Makefile COPYING TODO ChangeLog AUTHORS README va.xpm
SPEC_FILE_IN = lvs-gui.spec.in

######################################################################
# No user servicable parts below here
######################################################################

DISTNAME     = $(PACKAGE)-$(MAJOR).$(MINOR).$(PATCH)
RPMDISTNAME  = $(PACKAGE)-$(MAJOR).$(MINOR).$(PATCH)-$(RELEASE)
SPEC_FILE    = $(SPEC_FILE_IN:.in=)
DISTFILES    = $(SRCS) $(HDRS) $(EXTRA_DIST) $(INIT_SCRIPTS) \
$(SPEC_FILE) $(SPEC_FILE_IN)
DEPEND_FILE  = .dep


CC       = gcc
CP       = cp
ECHO     = echo
GZIP     = gzip --best
INSTALL  = install
MKDIR    = mkdir
RM       = rm
RPM      = rpm
SED      = sed
STRIP    = strip
TAR      = tar


OBJS     = $(SRCS:.c=.o)
PREFIX   = /usr
BINDIR   = $(BUILD_ROOT)/$(PREFIX)/bin
ETCDIR   = $(BUILD_ROOT)/etc
INITDIR_SUFFIX=rc.d/init.d/
INITDIR  = $(ETCDIR)/$(INITDIR_SUFFIX)
BIN      = lvs-gui
LIBS     = \
-L/usr/local/lib \
`gtk-config --libs` \
`imlib-config --libs-gdk`
HEADERS = \
-I/usr/local/include \
`gtk-config --cflags` \
`imlib-config --cflags-gdk`

.PHONY = \
clean \
install \
install-client \
install-server \
install-strip \
install-client-strip \
install-server-strip \
all \
dep \
depend \
rpm

all: readme.h lvs-gui  $(SPEC_FILE)

readme.h: README
	$(SED) -e 's/"/\\"/g' -e 's/\n/\\\\n/g' -e 's/^.*$$/"&\\n"/' < $< > $@
	
lvs-gui: $(DEPEND_FILE) $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS) $(CFLAGS) $(HEADERS)

clean:
	$(RM) -f $(BIN) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a \
        .emacs_* tags TAGS make.log MakeOut  "#"* \
        $(SPEC_FILE) $(DEPEND_FILE) readme.h

install: install-client install-server
	if [ ! -e $(BINDIR) ]; then $(MKDIR) -p $(BINDIR); fi
	$(INSTALL) $(BIN)      $(BINDIR)

install-client: lvs-gui
	if [ ! -e $(BINDIR) ]; then $(MKDIR) -p $(BINDIR); fi
	$(INSTALL) $(BIN)      $(BINDIR)

install-server:
	if [ ! -e $(INITDIR) ]; then $(MKDIR) -p $(INITDIR); fi
	@for i in $(INIT_SCRIPTS); do \
		echo $(INSTALL) $$i $(INITDIR); \
		$(INSTALL) $$i $(INITDIR); \
	done

install-strip: install-server-strip install-client-strip

install-server-strip: install-server

install-client-strip: install-client
	$(STRIP) $(BINDIR)/$(BIN)

dep: depend

depend: $(DEPEND_FILE)

$(DEPEND_FILE): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -M $(SRCS) $(HEADERS) > $(DEPEND_FILE)

dist: $(SPEC_FILE)
	$(RM) -rf $(DISTNAME)/
	$(MKDIR) $(DISTNAME)/
	@for i in $(DISTFILES); do \
		dir=`dirname $$i`; \
                file=`basename $$i`; \
		if [ ! -e $(DISTNAME)/$$dir ]; then \
			echo $(MKDIR) -p $(DISTNAME)/$$dir; \
			$(MKDIR) -p $(DISTNAME)/$$dir; \
		fi; \
		echo $(CP) $$i $(DISTNAME)/$$dir; \
		$(CP) $$i $(DISTNAME)/$$dir; \
	done
	$(TAR) -cf $(DISTNAME).tar $(DISTNAME)/
	$(RM) -f $(DISTNAME).tar.gz
	$(GZIP) $(DISTNAME).tar
	$(RM) -rf $(DISTNAME)/
	@$(ECHO) "############################################################"
	@$(ECHO) "# $(DISTNAME).tar.gz is ready for distribution"
	@$(ECHO) "############################################################"

rpm: dist
	$(RPM) -ta $(DISTNAME).tar.gz
	$(RM) $(DISTNAME).tar.gz

%: %.in Makefile
	sed \
		-e "s/@PACKAGE@/$(PACKAGE)/" \
		-e "s/@MAJOR@/$(MAJOR)/" \
		-e "s/@MINOR@/$(MINOR)/" \
		-e "s/@PATCH@/$(PATCH)/" \
		-e "s/@RELEASE@/$(RELEASE)/" < $< > $@
.c.o:
	$(CC) -I/usr/local/include $(CFLAGS) $(HEADERS) -c $< -o $@

ifeq (.dep,$(wildcard $(DEPEND_FILE)))
include $(DEPEND_FILE)
endif

