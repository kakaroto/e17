/* DOX for Enlightenment - by The Rasterman (C) 1998 */

#include "econfig.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xosdefs.h>
#include <X11/Xproto.h>
#include <X11/keysym.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XShm.h>
#include <Imlib.h>
#include <Fnlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "econfig.h"

#include "dox.h"

#ifdef __alpha__		/* gets rid of some misalignment in GCC */
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

#define STARTPOS 4		/* to bypass next/prev/exit buttons for key binding positions */

typedef struct _mwmhints
  {
     CARD32              flags;
     CARD32              functions;
     CARD32              decorations;
     INT32               inputMode;
  }
MWMHints;

#include "title.xpm"
#include "prev1.xpm"
#include "prev2.xpm"
#include "next1.xpm"
#include "next2.xpm"
#include "exit1.xpm"
#include "exit2.xpm"

Display            *disp;
ImlibData          *id;
FnlibData          *fd;
Window              win_main, win_title, win_exit, win_next, win_prev, win_text,
                    win_cover;
int                 w, h, t;
ImlibImage         *im_text;
ImlibImage         *im_title;
ImlibImage         *im_prev1, *im_prev2;
ImlibImage         *im_next1, *im_next2;
ImlibImage         *im_exit1, *im_exit2;
char               *docdir;

Window              CreateWindow(Window parent, int x, int y, int ww, int hh);
int                 ReadHeader(FILE * f);
int                 ReadPages(FILE * f);

Window
CreateWindow(Window parent, int x, int y, int ww, int hh)
{
   Window              win;
   XSetWindowAttributes attr;
   MWMHints            mwm;

/*   Atom                a; */
   XSizeHints          hnt;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.colormap = Imlib_get_colormap(id);
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   mwm.flags = MWM_HINTS_DECORATIONS;
   mwm.functions = 0;
   mwm.decorations = 0;
   mwm.inputMode = 0;
/*   a = XInternAtom(disp, "_MOTIF_WM_HINTS", False); */
   win = XCreateWindow(disp, parent, x, y, ww, hh, 0, id->x.depth,
		       InputOutput, Imlib_get_visual(id),
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


int
main(int argc, char **argv)
{
   int                 pagenum;
   int                 x, y;
   int                 wx, wy;
   FILE               *f;
   char               *s;
   Pixmap              draw = 0;
   Link               *l = NULL, *ll = NULL;
   ImlibBorder         ibd;
   ImlibInitParams     params;
   int                *page_hist = NULL;
   int                 page_hist_len = 1;
   int                 page_hist_pos = 0;

   w = 512;
   h = 400;
   x = 0;
   y = 0;
   pagenum = 0;

   disp = XOpenDisplay(NULL);
   params.flags = PARAMS_IMAGECACHESIZE | PARAMS_PIXMAPCACHESIZE;
   params.imagecachesize = (w * h * 3 * 2);
   params.pixmapcachesize = (w * h * 3 * 2 * 8);
   id = Imlib_init_with_params(disp, &params);
   Imlib_set_render_type(id, RT_DITHER_TRUECOL);
   fd = Fnlib_init(id);

   im_title = Imlib_create_image_from_xpm_data(id, title_xpm);
   ibd.left = 50;
   ibd.right = 2;
   ibd.top = 2;
   ibd.bottom = 2;
   Imlib_set_image_border(id, im_title, &ibd);
   im_prev1 = Imlib_create_image_from_xpm_data(id, prev1_xpm);
   im_prev2 = Imlib_create_image_from_xpm_data(id, prev2_xpm);
   im_next1 = Imlib_create_image_from_xpm_data(id, next1_xpm);
   im_next2 = Imlib_create_image_from_xpm_data(id, next2_xpm);
   im_exit1 = Imlib_create_image_from_xpm_data(id, exit1_xpm);
   im_exit2 = Imlib_create_image_from_xpm_data(id, exit2_xpm);

   if (argc < 2)
     {
	printf("usage:\n%s [-page num] Edoc_dir\n", argv[0]);
	exit(1);
     }
   if (!strcmp(argv[1], "-page"))
     {
	pagenum = atoi(argv[2]);
	docdir = strdup(argv[3]);
     }
   else
      docdir = strdup(argv[1]);
   s = malloc(strlen(docdir) + 1 + 5);
   strcpy(s, docdir);
   strcat(s, "/MAIN");
   f = fopen(s, "r");
   if (!f)
     {
	printf("Edoc_dir does not contain a MAIN file\n");
	exit(1);
     }
   t = 16;
   GetObjects(f);
   fclose(f);
   Fnlib_add_dir(fd, docdir);
   wx = (DisplayWidth(disp, DefaultScreen(disp)) - w) / 2;
   wy = (DisplayHeight(disp, DefaultScreen(disp)) - (h + t)) / 2;
   win_main = CreateWindow(id->x.root, wx, wy, w, h + t);
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
   draw = XCreatePixmap(disp, win_text, w, h, id->x.depth);
   XSetWindowBackgroundPixmap(disp, win_text, draw);
   Imlib_apply_image(id, im_title, win_title);
   Imlib_apply_image(id, im_exit1, win_exit);
   Imlib_apply_image(id, im_next1, win_next);
   Imlib_apply_image(id, im_prev1, win_prev);

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
   
   for (;;)
     {
	KeySym              key;
	XEvent              ev;

	XNextEvent(disp, &ev);
	switch (ev.type)
	  {
	  case KeyPress:
	     key = XLookupKeysym(&ev.xkey, 0);
	     switch (key)
	       {
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
		Imlib_apply_image(id, im_prev2, win_prev);
	     else if (ev.xbutton.window == win_next)
		Imlib_apply_image(id, im_next2, win_next);
	     else if (ev.xbutton.window == win_exit)
		Imlib_apply_image(id, im_exit2, win_exit);
	     else
	       {
		  int                 x, y;

		  x = ev.xbutton.x;
		  y = ev.xbutton.y;
		  ll = l;
		  while (ll)
		    {
		       if ((x >= ll->x) && (y >= ll->y) &&
			   (x < (ll->x + ll->w)) &&
			   (y < (ll->y + ll->h)))
			 {
			    int                 pg;

			    if (!strncmp("EXEC.", ll->name, 5))
			      {
				 if (!fork())
				   {
				      char *exe;
				      
				      exe = &(ll->name[5]);
				      execl("/bin/sh", "/bin/sh", "-c", exe, NULL);
				      exit(0);
				   }
			      }
			    else if (!strncmp("INPUT.", ll->name, 6))
			      {
				 FILE *p;
				 char *exe, tmp[1024];
				 
				 exe = &(ll->name[6]);
				 if (exe[0] != '/')
				   {
				      sprintf(tmp, "%s/%s", docdir, exe);
				      exe = tmp;
				   }
				 p = popen(exe, "r");
				 if (p)
				   {
				      int dirlen = 0;
				      char *sp;
				      
				      sp = exe;
				      while ((*sp) && (*sp != ' '))
					 sp++;
				      while ((*sp != '/') && (sp != exe))
					 sp--;
				      dirlen = sp - exe;
				      if (dirlen > 1)
					{
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
			      }
			    else
			      {
				 pg = GetPage(ll->name);
				 if (pg >= 0)
				   {
				      FREE_LINKS;
				      pagenum = pg;
				      if (page_hist_pos >= page_hist_len)
					{
					   page_hist_len++;
					   page_hist = 
					      realloc(page_hist,
						      sizeof(int) * page_hist_len);
					}
				      page_hist_pos++;
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
	     if (ev.xbutton.window == win_prev)
	       {
		  Imlib_apply_image(id, im_prev1, win_prev);
		  FREE_LINKS;
		  page_hist_pos--;
		  if (page_hist_pos < 0)
		     page_hist_pos = 0;
		  pagenum = page_hist[page_hist_pos];
		  l = RenderPage(draw, pagenum, w, h);
		  UPDATE;
	       }
	     else if (ev.xbutton.window == win_next)
	       {
		  int prev_pagenum;
		  Imlib_apply_image(id, im_next1, win_next);
		  FREE_LINKS;
		  prev_pagenum = pagenum;
		  pagenum++;
		  pagenum = FixPage(pagenum);
		  if (pagenum != prev_pagenum)
		    {
		       if (page_hist_pos >= page_hist_len)
			 {
			    page_hist_len++;
			    page_hist = realloc(page_hist,
						sizeof(int) * page_hist_len);
			    page_hist[page_hist_len - 1] = pagenum;
			    page_hist_pos = page_hist_len - 1;
			 }
		       else
			  page_hist_pos++;
		       l = RenderPage(draw, pagenum, w, h);
		       UPDATE;
		    }
	       }
	     else if (ev.xbutton.window == win_exit)
	       {
		  Imlib_apply_image(id, im_exit1, win_exit);
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
		int                 x, y;
		static Link        *pl = NULL;
		char                found = 0;

		x = ev.xmotion.x;
		y = ev.xmotion.y;
		ll = l;
		while (ll)
		  {
		     if ((x >= ll->x) && (y >= ll->y) &&
			 (x < (ll->x + ll->w)) &&
			 (y < (ll->y + ll->h)))
		       {
			  GC                  gc;
			  XGCValues           gcv;
			  int                 r, g, b;

			  if (pl != ll)
			    {
			       if (pl)
				 {
				    UPDATE_NOW;
				 }
			       GetLinkColors(pagenum, &r, &g, &b);
			       gc = XCreateGC(disp, win_text, 0, &gcv);
			       XSetForeground(disp, gc,
				      Imlib_best_color_match(id, &r, &g, &b));
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
		if (!found)
		  {
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
