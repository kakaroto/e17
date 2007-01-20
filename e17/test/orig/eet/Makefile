FLAGS = `eet-config --cflags`
LIBS  = `eet-config --libs`

BINS  = eet eet_bench
all: $(BINS)

eet_SRCS = eet_main.c
eet_OBJS = $(eet_SRCS:.c=.o)
eet: $(eet_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(eet_OBJS) -o $@

eet_bench_SRCS = eet_bench_main.c
eet_bench_OBJS = $(eet_bench_SRCS:.c=.o)
eet_bench: $(eet_bench_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(eet_bench_OBJS) -o $@

############################################################################
#### boilerplate
.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
