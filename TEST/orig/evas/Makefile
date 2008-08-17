FLAGS = `pkg-config --cflags evas` -I/usr/X11R6/include -I/usr/X11R7/include -I/usr/xorg/include
LIBS  = `pkg-config --libs evas`

BINS  = evas_software_x11_test \
evas_software_x11_perf_test \
evas_software_x11_perf_load \
evas_software_xcb_test \
evas_software_xcb_perf_test \
evas_software_xcb_perf_load \
evas_directfb_test \
evas_directfb_window \
evas_fb_test \
evas_buffer_test \
evas_software_qtopia_test \
evas_gl_x11_test \
evas_cairo_x11_test \
evas_xrender_x11_test \
evas_xrender_xcb_test \
evas_software_ddraw_test

all: evas_buffer_test

evas_buffer_test_SRCS = evas_test_main.c evas_buffer_main.c
evas_buffer_test_OBJS = $(evas_buffer_test_SRCS:.c=.o)
evas_buffer_test: $(evas_buffer_test_OBJS)
	$(RM) $@
	$(CC) $(evas_buffer_test_OBJS) $(LIBS) $(LDFLAGS) -lm -o $@

evas_software_x11_test_SRCS = evas_test_main.c evas_software_x11_main.c
evas_software_x11_test_OBJS = $(evas_software_x11_test_SRCS:.c=.o)
evas_software_x11_test: $(evas_software_x11_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_software_x11_test_OBJS) -lm  -L/usr/X11R6/lib -L/usr/X11R7/lib -L/usr/xorg/lib -lX11 -lXext -o $@

evas_xrender_x11_test_SRCS = evas_test_main.c evas_xrender_x11_main.c
evas_xrender_x11_test_OBJS = $(evas_xrender_x11_test_SRCS:.c=.o)
evas_xrender_x11_test: $(evas_xrender_x11_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_xrender_x11_test_OBJS) -lm  -L/usr/X11R6/lib -L/usr/X11R7/lib -L/usr/xorg/lib -lX11 -lXext -lXrender -o $@

# disabled by default
evas_fb_test_SRCS = evas_test_main.c evas_fb_main.c
evas_fb_test_OBJS = $(evas_fb_test_SRCS:.c=.o)
evas_fb_test: $(evas_fb_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_fb_test_OBJS) -lm  -o $@

evas_gl_x11_test_SRCS = evas_test_main.c evas_gl_x11_main.c
evas_gl_x11_test_OBJS = $(evas_gl_x11_test_SRCS:.c=.o)
evas_gl_x11_test: $(evas_gl_x11_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_gl_x11_test_OBJS) -lm  -L/usr/X11R6/lib -L/usr/X11R7/lib -L/usr/xorg/lib -lX11 -lXext -lGL -lGLU -lpthread -o $@

# test engines - not production
evas_software_xcb_test_SRCS = evas_test_main.c evas_software_xcb_main.c
evas_software_xcb_test_OBJS = $(evas_software_xcb_test_SRCS:.c=.o)
evas_software_xcb_test: $(evas_software_xcb_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_software_xcb_test_OBJS) -lm  -o $@

evas_xrender_xcb_test_SRCS = evas_test_main.c evas_xrender_xcb_main.c
evas_xrender_xcb_test_OBJS = $(evas_xrender_xcb_test_SRCS:.c=.o)
evas_xrender_xcb_test: $(evas_xrender_xcb_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_xrender_xcb_test_OBJS) -lm  -o $@

# almost never used
evas_directfb_test_SRCS = evas_test_main.c evas_directfb_main.c
evas_directfb_test_OBJS = $(evas_directfb_test_SRCS:.c=.o)
evas_directfb_test: $(evas_directfb_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_directfb_test_OBJS) -lm  -o $@

evas_directfb_window_test_SRCS = evas_test_main.c evas_directfb_window.c
evas_directfb_window_test_OBJS = $(evas_directfb_window_test_SRCS:.c=.o)
evas_directfb_window_test: $(evas_directfb_window_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_directfb_window_test_OBJS) -lm  -o $@

## dead engine - useless
evas_cairo_x11_test_SRCS = evas_test_main.c evas_cairo_x11_main.c
evas_cairo_x11_test_OBJS = $(evas_cairo_x11_test_SRCS:.c=.o)
evas_cairo_x11_test: $(evas_cairo_x11_test_OBJS)
	$(RM) $@
	$(CC) $(LIBS) $(LDFLAGS) $(evas_cairo_x11_test_OBJS) -lm -L/usr/X11R6/lib -L/usr/X11R7/lib -L/usr/xorg/lib -lX11 -lXext -o $@

evas_software_ddraw_test_SRCS = evas_test_main.c evas_software_ddraw_main.c
evas_software_ddraw_test_OBJS = $(evas_software_ddraw_test_SRCS:.c=.o)
evas_software_ddraw_test: $(evas_software_ddraw_test_OBJS)
	$(RM) $@
	$(CC) $(evas_software_ddraw_test_OBJS) -lddraw -ldxguid $(LIBS) $(LDFLAGS) -lm  -o $@



############################################################################
#### boilerplate
.c.o:
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
clean::
	rm -rf $(BINS) *.o *~
