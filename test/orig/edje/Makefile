FLAGS = `pkg-config --cflags edje ecore-evas evas eet`
LIBS = `pkg-config --libs edje ecore-evas evas eet`

BINS  = edje edje_ls edje_test edje_thumb
all: $(BINS)

edje_SRCS = edje_main.c
edje_OBJS = $(edje_SRCS:.c=.o)
edje: $(edje_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(edje_OBJS) -o $@

edje_ls_SRCS = edje_ls.c
edje_ls_OBJS = $(edje_ls_SRCS:.c=.o)
edje_ls: $(edje_ls_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(edje_ls_OBJS) -o $@

edje_test_SRCS = edje_test_main.c
edje_test_OBJS = $(edje_test_SRCS:.c=.o)
edje_test: $(edje_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(edje_test_OBJS) -o $@

edje_thumb_SRCS = edje_thumb.c
edje_thumb_OBJS = $(edje_thumb_SRCS:.c=.o)
edje_thumb: $(edje_thumb_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(edje_thumb_OBJS) -o $@

############################################################################
#### boilerplate
.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
	
