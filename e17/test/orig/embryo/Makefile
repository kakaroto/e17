FLAGS = `embryo-config --cflags`
LIBS  = `embryo-config --libs`

BINS  = embryo
all: $(BINS)

embryo_SRCS = embryo_main.c
embryo_OBJS = $(embryo_SRCS:.c=.o)
embryo: $(embryo_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(CFLAGS) $(LDFLAGS) $(embryo_OBJS) -o $@

############################################################################
#### boilerplate
.c.o:
	$(CC) $(FALGS) $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
	
