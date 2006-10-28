FLAGS = `eet-config --cflags`
LIBS  = `eet-config --libs`

BINS  = eet eet_bench
all: $(BINS)

eet_SRCS = eet_main.c
eet_OBJS = $(eet_SRCS:.c=.o)
eet_bench_SRCS = eet_bench_main.c
eet_bench_OBJS = $(eet_bench_SRCS:.c=.o)

### link
eet: $(eet_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(CFLAGS) $(LDFLAGS) $< -o $@

eet_bench: $(eet_bench_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(CFLAGS) $(LDFLAGS) $< -o $@

############################################################################
#### boilerplate
.c.o:
	$(CC) $(FALGS) $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
	
