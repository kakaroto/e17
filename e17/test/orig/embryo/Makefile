FLAGS = `pkg-config --cflags embryo`
LIBS  = `pkg-config --libs embryo`

BINS  = embryo
all: $(BINS)

embryo_SRCS = embryo_main.c
embryo_OBJS = $(embryo_SRCS:.c=.o)
embryo: $(embryo_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(embryo_OBJS) -o $@

############################################################################
#### boilerplate
.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
	
