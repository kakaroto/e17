FLAGS = `pkg-config --cflags eet`
LIBS  = `pkg-config --libs eet`

BINS  = eet eet_bench eet_test
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

eet_test_SRCS = eet_test_main.c
eet_test_OBJS = $(eet_test_SRCS:.c=.o)
eet_test: $(eet_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) `pkg-config --libs evas` $(LDFLAGS) $(eet_test_OBJS) -o $@

############################################################################
#### boilerplate
.c.o:
	$(CC) $(FLAGS) `pkg-config --cflags evas` $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
