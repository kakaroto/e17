SRCS = \
pixel_op.c \
blend.c

FLGS = \
$(CFLAGS) \
-I. \
`$(CFBIN)imlib2-config --cflags`

LIBS = \
$(CFLAGS) \
`$(CFBIN)imlib2-config --libs`

####################
OBJS = $(SRCS:.c=.o)

blend: $(OBJS)
	$(RM) $@
	$(CC) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) $(FLGS) $(CFLAGS) -c $< -o $@

clean::
	rm -rf blend out_*.png *.CKP *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags TAGS make.log MakeOut "#"*
