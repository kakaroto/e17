INSTALL  = install

SRCS     = lvs-gui.c

OBJS     = $(SRCS:.c=.o)
BINDIR   = /usr/local/bin
ETCDIR   = /etc
BIN      = lvs-gui
LIBS     = -L/usr/lcoal/lib `gtk-config --libs` `imlib-config --libs-gdk`

lvs-gui: $(OBJS)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LIBS)

clean::
	$(RM) $(BIN) *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut  "#"*

.c.o:
	$(CC) -I/usr/local/include `gtk-config --cflags` `imlib-config --cflags-gdk` -O2 -c $< -o $@

install:: 
	$(INSTALL) $(BIN)      $(BINDIR)

