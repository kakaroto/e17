INSTALL  = install

SRCS     = rend.c ximage.c scale.c main.c rgba.c image.c color.c grab.c \
           blend.c file.c draw.c rgbadraw.c api.c
OBJS     = $(SRCS:.c=.o)
BINDIR   = /usr/local/bin
BIN      = imlib2
LIBS     = -L/usr/X11R6/lib/ -lX11 -lXext -ldl

CFLAGS   = -O -g -mpentium -mcpu=pentium -march=pentium

imlib2: $(OBJS)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LIBS)
	$(CC) loader_png.c -o png.so -Wl,-soname -Wl,png.so -shared -lpng -lz -lm

clean::
	$(RM) $(BIN) *.CKP *.ln *.BAK *.bak *.o *.so core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut  "#"*

c.o:
	$(CC) $(CFLAGS) -I/usr/X11R6/include -c $< -o $@
load.o:
	$(CC) $(CFLAGS) -I/usr/X11R6/include -c $< -o $@

install:: 
	$(INSTALL) $(BIN)      $(BINDIR)

