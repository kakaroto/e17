/*
  Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to
  deal in the Software without restriction, including without limitation the
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies of the Software, its documentation and marketing & publicity
  materials, and acknowledgment shall be given in the documentation, materials
  and software packages that this Software was used.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  */

#include "dox.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xosdefs.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XShm.h>

#ifdef HAS_XINERAMA
#include <X11/extensions/Xinerama.h>
#endif

#include <Imlib.h>
#if USE_FNLIB
#include <Fnlib.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#if defined(__alpha__) && defined(__GNUC__) && ((__GNUC__ == 2) && (__GNUC_MINOR__ < 96)) /* gets rid of some misalignment in GCC */
#pragma 2
#endif

/* Motif window hints */
#define MWM_HINTS_FUNCTIONS           (1L << 0)
#define MWM_HINTS_DECORATIONS         (1L << 1)

/* bit definitions for MwmHints.functions */
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL                 (1L << 0)
#define MWM_DECOR_BORDER              (1L << 1)
#define MWM_DECOR_RESIZEH             (1L << 2)
#define MWM_DECOR_TITLE               (1L << 3)
#define MWM_DECOR_MENU                (1L << 4)
#define MWM_DECOR_MINIMIZE            (1L << 5)
#define MWM_DECOR_MAXIMIZE            (1L << 6)

#define PROP_MWM_HINTS_ELEMENTS       4

#define STARTPOS 4				/* to bypass next/prev/exit buttons for key binding positions */

typedef struct _mwmhints {
	CARD32 flags;
	CARD32 functions;
	CARD32 decorations;
	INT32 inputMode;
} MWMHints;

#include "title.xpm"
#include "prev1.xpm"
#include "prev2.xpm"
#include "next1.xpm"
#include "next2.xpm"
#include "exit1.xpm"
#include "exit2.xpm"

Display *disp;
ImlibData *pImlibData;
#if USE_FNLIB
FnlibData *pFnlibData;
#endif
Window win_main, win_title, win_exit, win_next, win_prev, win_text, win_cover;
int w, h, t;
ImlibImage *im_text;
ImlibImage *im_title;
ImlibImage *im_prev1, *im_prev2;
ImlibImage *im_next1, *im_next2;
ImlibImage *im_exit1, *im_exit2;
char *docdir;

Window CreateWindow(Window parent, int x, int y, int ww, int hh);
int ReadHeader(FILE * f);
int ReadPages(FILE * f);

Window CreateWindow(Window parent, int x, int y, int ww, int hh)
{
	Window win;
	XSetWindowAttributes attr;
	MWMHints mwm;

/*   Atom                a; */
	XSizeHints hnt;

	attr.backing_store = NotUseful;
	attr.override_redirect = False;
	attr.colormap = Imlib_get_colormap(pImlibData);
	attr.border_pixel = 0;
	attr.background_pixel = 0;
	attr.save_under = False;
	mwm.flags = MWM_HINTS_DECORATIONS;
	mwm.functions = 0;
	mwm.decorations = 0;
	mwm.inputMode = 0;
/*   a = XInternAtom(disp, "_MOTIF_WM_HINTS", False); */
	win = XCreateWindow(disp, parent, x, y, ww, hh, 0, pImlibData->x.depth,
						InputOutput, Imlib_get_visual(pImlibData),
						CWOverrideRedirect | CWSaveUnder | CWBackingStore |
						CWColormap | CWBackPixel | CWBorderPixel, &attr);
	XSetWindowBackground(disp, win, 0);
/*   XChangeProperty(disp, win, a, a, 32, PropModeReplace,
 * (unsigned char *)&mwm, sizeof(MWMHints) / 4); */
	XStoreName(disp, win, "DOX: Enlightenment Document Viewer");
	hnt.flags = USPosition | USSize | PPosition | PSize | PMinSize | PMaxSize;
	hnt.x = x;
	hnt.y = y;
	hnt.width = ww;
	hnt.height = hh;
	hnt.min_width = ww;
	hnt.max_width = ww;
	hnt.min_height = hh;
	hnt.max_height = hh;
	XSetWMNormalHints(disp, win, &hnt);
	return win;
}

#define FREE_LINKS \
ll = l; \
while (ll) \
{ \
  l = ll; \
  ll = ll->next; \
  free(l->name); \
  free(l); \
}

#define UPDATE_NOW \
{ \
XSetWindowBackgroundPixmap(disp, win_text, draw); \
XClearWindow(disp, win_text); \
}

#define UPDATE \
{ \
  int up_i, up_j; \
  int up_lut[16] = { 0, 8, 4, 12, 2, 6, 10, 14, \
                     3, 11, 1, 9, 7, 13, 5, 15}; \
  XSetWindowBackgroundPixmap(disp, win_text, draw); \
  for (up_j = 0; up_j < 16; up_j++) \
    { \
      for (up_i = 0; up_i < h; up_i += 16) \
        { \
          XClearArea(disp, win_text, 0, up_i + up_lut[up_j], w, 1, False); \
        } \
      XSync(disp, False); \
    } \
}

int main(int argc, char **argv)
{
	int pagenum;
	int i, w, h, x, y;
	int wx, wy;
	FILE *f;
	char *s, *docfile = NULL;
	Pixmap draw = 0;
	Link *l = NULL, *ll = NULL;
	ImlibBorder ibd;
	ImlibInitParams params;
	int *page_hist = NULL;
	int page_hist_len = 1;
	int page_hist_pos = 0;

	w = 512;
	h = 400;
	x = 0;
	y = 0;
	pagenum = 0;

        if((int)(disp = XOpenDisplay(NULL)) < 1) {
                printf("%s: Cannot open X display.\n",argv[0]);
                exit(0);
        }

	/* now we'll set the locale */
	setlocale(LC_ALL, "");
	if (!XSupportsLocale())
		setlocale(LC_ALL, "C");
	XSetLocaleModifiers("");
	setlocale(LC_ALL, NULL);

	/* I dont want any internationalisation of my numeric input & output */
	setlocale(LC_NUMERIC, "C");

	params.flags = PARAMS_IMAGECACHESIZE | PARAMS_PIXMAPCACHESIZE;
	params.imagecachesize = (w * h * 3 * 2);
	params.pixmapcachesize = (w * h * 3 * 2 * 8);
	pImlibData = Imlib_init_with_params(disp, &params);
	Imlib_set_render_type(pImlibData, RT_DITHER_TRUECOL);
#if USE_FNLIB
	pFnlibData = Fnlib_init(pImlibData);
#endif

	im_title = Imlib_create_image_from_xpm_data(pImlibData, title_xpm);
	ibd.left = 50;
	ibd.right = 2;
	ibd.top = 2;
	ibd.bottom = 2;
	Imlib_set_image_border(pImlibData, im_title, &ibd);
	im_prev1 = Imlib_create_image_from_xpm_data(pImlibData, prev1_xpm);
	im_prev2 = Imlib_create_image_from_xpm_data(pImlibData, prev2_xpm);
	im_next1 = Imlib_create_image_from_xpm_data(pImlibData, next1_xpm);
	im_next2 = Imlib_create_image_from_xpm_data(pImlibData, next2_xpm);
	im_exit1 = Imlib_create_image_from_xpm_data(pImlibData, exit1_xpm);
	im_exit2 = Imlib_create_image_from_xpm_data(pImlibData, exit2_xpm);

	if (argc < 2) {
		printf("usage:\n"
			   "%s [-page page_number] [-file Edoc_fname] [-size width height] Edoc_dir\n",
			   argv[0]);
		exit(1);
	}
	docdir = ".";
	docfile = "MAIN";
	for (i = 1; i < argc; i++) {
		if ((!strcmp(argv[i], "-page")) && (i < (argc - 1)))
			pagenum = atoi(argv[++i]);
		else if ((!strcmp(argv[i], "-file")) && (i < (argc - 1)))
			docfile = argv[++i];
		else if ((!strcmp(argv[i], "-size")) && (i < (argc - 2))) {
			w = atoi(argv[++i]);
			h = atoi(argv[++i]);
		} else
			docdir = strdup(argv[i]);
	}
	s = malloc(strlen(docdir) + strlen(docfile) + 2 + 20);
	sprintf(s, "%s/%s", docdir, docfile);
	findLocalizedFile(s);

#ifndef __EMX__
	f = fopen(s, "r");
#else
	f = fopen(s, "rt");
#endif
	if (!f) {
		strcpy(s, docdir);
		strcat(s, "/");
		strcat(s, docfile);
#ifndef __EMX__
		f = fopen(s, "r");
#else
		f = fopen(s, "rt");
#endif
		if (!f) {
			printf("Edoc_dir %s does not contain a %s file\n", docdir, docfile);
			exit(1);
		}
	}
	Esetenv("DISPLAY", DisplayString(disp), 1);
	Esetenv("E_DATADIR", ENLIGHTENMENT_ROOT, 1);
	Esetenv("E_BINDIR", ENLIGHTENMENT_BIN, 1);

	t = 16;
	GetObjects(f);
	fclose(f);
#if USE_FNLIB
	Fnlib_add_dir(pFnlibData, docdir);
#endif
	wx = (DisplayWidth(disp, DefaultScreen(disp)) - w) / 2;
	wy = (DisplayHeight(disp, DefaultScreen(disp)) - (h + t)) / 2;
#ifdef HAS_XINERAMA
	{
		if (XineramaIsActive(disp)) {
			Window rt, ch;
			int d;
			unsigned int ud;
			int pointer_x, pointer_y;
			int num;
			XineramaScreenInfo *screens;

			XQueryPointer(disp, pImlibData->x.root, &rt, &ch, &pointer_x, &pointer_y,
						  &d, &d, &ud);

			screens = XineramaQueryScreens(disp, &num);
			for (i = 0; i < num; i++) {
				if (pointer_x >= screens[i].x_org) {
					if (pointer_x <= (screens[i].width + screens[i].x_org)) {
						if (pointer_y >= screens[i].y_org) {
							if (pointer_y <= (screens[i].height +
											  screens[i].y_org)) {
								wx = ((screens[i].width - w) / 2) 
									+ screens[i].x_org;
								wy = ((screens[i].height - (h + t)) / 2) 
									+ screens[i].y_org;
							}
						}
					}
				}
			}

			XFree(screens);
		}

	}
#endif
	win_main = CreateWindow(pImlibData->x.root, wx, wy, w, h + t);
	win_title = CreateWindow(win_main, 0, 0, (w - 64 - 64 - t), t);
	win_prev = CreateWindow(win_main, (w - 64 - 64 - t), 0, 64, t);
	XSelectInput(disp, win_prev, ButtonPressMask | ButtonReleaseMask);
	win_next = CreateWindow(win_main, (w - 64 - 64 - t) + 64, 0, 64, t);
	XSelectInput(disp, win_next, ButtonPressMask | ButtonReleaseMask);
	win_exit = CreateWindow(win_main, (w - 64 - 64 - t) + 64 + 64, 0, t, t);
	XSelectInput(disp, win_exit, ButtonPressMask | ButtonReleaseMask);
	win_text = CreateWindow(win_main, 0, t, w, h);
	XSelectInput(disp, win_text, ButtonPressMask | ButtonReleaseMask |
				 KeyPressMask | KeyReleaseMask | PointerMotionMask);
	draw = XCreatePixmap(disp, win_text, w, h, pImlibData->x.depth);
	XSetWindowBackgroundPixmap(disp, win_text, draw);
	Imlib_apply_image(pImlibData, im_title, win_title);
	Imlib_apply_image(pImlibData, im_exit1, win_exit);
	Imlib_apply_image(pImlibData, im_next1, win_next);
	Imlib_apply_image(pImlibData, im_prev1, win_prev);

	l = RenderPage(draw, pagenum, w, h);

	XMapWindow(disp, win_text);
	XMapWindow(disp, win_exit);
	XMapWindow(disp, win_next);
	XMapWindow(disp, win_prev);
	XMapWindow(disp, win_title);
	XMapWindow(disp, win_main);
	XSync(disp, False);
	page_hist = malloc(sizeof(int));

	page_hist[0] = 0;

	for (;;) {
		KeySym key;
		XEvent ev;

		XNextEvent(disp, &ev);
		switch (ev.type) {
			case KeyPress:
			key = XLookupKeysym(&ev.xkey, 0);
			switch (key) {
				case XK_Escape:
				exit(0);
				break;
				case XK_Down:
				case XK_Right:
				break;
				case XK_Up:
				case XK_Left:
				break;
				case XK_Return:
				break;
				case XK_Home:
				FREE_LINKS;
				pagenum = 0;
				pagenum = FixPage(pagenum);
				l = RenderPage(draw, pagenum, w, h);
				UPDATE;
				break;
				case XK_End:
				FREE_LINKS;
				pagenum = 99999;
				pagenum = FixPage(pagenum);
				l = RenderPage(draw, pagenum, w, h);
				UPDATE;
				break;
				case XK_Prior:
				FREE_LINKS;
				pagenum--;
				pagenum = FixPage(pagenum);
				l = RenderPage(draw, pagenum, w, h);
				UPDATE;
				break;
				case XK_Next:
				FREE_LINKS;
				pagenum++;
				pagenum = FixPage(pagenum);
				l = RenderPage(draw, pagenum, w, h);
				UPDATE;
				break;
			}
			break;
			case ButtonPress:
			if (ev.xbutton.window == win_prev)
				Imlib_apply_image(pImlibData, im_prev2, win_prev);
			else if (ev.xbutton.window == win_next)
				Imlib_apply_image(pImlibData, im_next2, win_next);
			else if (ev.xbutton.window == win_exit)
				Imlib_apply_image(pImlibData, im_exit2, win_exit);
			else {
				int x, y;

				x = ev.xbutton.x;
				y = ev.xbutton.y;
				ll = l;
				while (ll) {
					if ((x >= ll->x) && (y >= ll->y) &&
						(x < (ll->x + ll->w)) && (y < (ll->y + ll->h))) {
						int pg;

						if (!strncmp("EXEC.", ll->name, 5)) {
							if (!fork()) {
								char *exe;

								exe = &(ll->name[5]);
								execl("/bin/sh", "/bin/sh", "-c", exe, NULL);
								exit(0);
							}
						} else if (!strncmp("INPUT.", ll->name, 6)) {
							FILE *p;
							char *exe, tmp[1024];

							exe = &(ll->name[6]);
							if (exe[0] != '/') {
								sprintf(tmp, "%s/%s", docdir, exe);
								findLocalizedFile(tmp);
								exe = tmp;
							}
							p = popen(exe, "r");
							if (p) {
								int dirlen = 0;
								char *sp;

								sp = exe;
								while ((*sp) && (*sp != ' '))
									sp++;
								while ((*sp != '/') && (sp != exe))
									sp--;
								dirlen = sp - exe;
								if (dirlen > 1) {
									free(docdir);
									docdir = malloc(dirlen + 1);
									memcpy(docdir, exe, dirlen);
									docdir[dirlen] = 0;
								}
								GetObjects(p);
								pclose(p);
								if (page_hist)
									free(page_hist);
								page_hist = malloc(sizeof(int));

								page_hist[0] = 0;
								page_hist_len = 1;
								pagenum = 0;
								page_hist_pos = 0;
								FREE_LINKS;
								l = RenderPage(draw, pagenum, w, h);
								UPDATE;
							}
						} else {
							pg = GetPage(ll->name);
							if (pg >= 0) {
								FREE_LINKS;
								pagenum = pg;
								page_hist_pos++;
								if (page_hist_pos >= page_hist_len) {
									page_hist_len++;
									page_hist =
										realloc(page_hist,
												sizeof(int) * page_hist_len);
								}
								page_hist[page_hist_pos] = pagenum;
								l = RenderPage(draw, pagenum, w, h);
								UPDATE;
							}
						}
						break;
					}
					ll = ll->next;
				}
			}
			break;
			case ButtonRelease:
			if (ev.xbutton.window == win_prev) {
				Imlib_apply_image(pImlibData, im_prev1, win_prev);
				FREE_LINKS;
				page_hist_pos--;
				if (page_hist_pos < 0)
					page_hist_pos = 0;
				pagenum = page_hist[page_hist_pos];
				l = RenderPage(draw, pagenum, w, h);
				UPDATE;
			} else if (ev.xbutton.window == win_next) {
				int prev_pagenum;

				Imlib_apply_image(pImlibData, im_next1, win_next);
				prev_pagenum = pagenum;
				pagenum++;
				pagenum = FixPage(pagenum);
				if (pagenum != prev_pagenum) {
					FREE_LINKS;
					page_hist_pos++;
					if (page_hist_pos >= page_hist_len) {
						page_hist_len++;
						page_hist = realloc(page_hist,
											sizeof(int) * page_hist_len);

						page_hist[page_hist_pos] = pagenum;
					} else
						page_hist[page_hist_pos] = pagenum;
					l = RenderPage(draw, pagenum, w, h);
					UPDATE;
				}
			} else if (ev.xbutton.window == win_exit) {
				Imlib_apply_image(pImlibData, im_exit1, win_exit);
				exit(0);
			}
			break;
			case EnterNotify:
			break;
			case LeaveNotify:
			break;
			case MotionNotify:
			while (XCheckTypedEvent(disp, ev.type, &ev));
			{
				int x, y;
				static Link *pl = NULL;
				char found = 0;

				x = ev.xmotion.x;
				y = ev.xmotion.y;
				ll = l;
				while (ll) {
					if ((x >= ll->x) && (y >= ll->y) &&
						(x < (ll->x + ll->w)) && (y < (ll->y + ll->h))) {
						GC gc;
						XGCValues gcv;
						int r, g, b;

						if (pl != ll) {
							if (pl) {
								UPDATE_NOW;
							}
							GetLinkColors(pagenum, &r, &g, &b);
							gc = XCreateGC(disp, win_text, 0, &gcv);
							XSetForeground(disp, gc, Imlib_best_color_match(pImlibData, &r, &g, &b));
							XDrawRectangle(disp, win_text, gc, ll->x, ll->y,
										   ll->w, ll->h);
							XFreeGC(disp, gc);
							pl = ll;
						}
						found = 1;
						ll = NULL;
					}
					if (ll)
						ll = ll->next;
				}
				if (!found) {
					UPDATE_NOW;
					pl = NULL;
				}
			}
			break;
			default:
			break;
		}
	}
}
