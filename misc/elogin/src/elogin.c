#include "main.h"
#include "main.h"
#include "ewl.h"
#include "elogin.h"

void	elogin_draw_window (void);
void	elogin_set_window_hints (EwlWindow *win);

void
elogin_init()
{
	elogin_draw_window();
}

void
elogin_draw_window()
{
	EwlWidget	*win;
	EwlWidget	*box1, *box2, *box3;
	EwlWidget	*logo;
	EwlWidget	*lbox;
	EwlWidget	*lprompt;
	EwlWidget	*pprompt;
	EwlImage	*logo_img;
	EwlImage	*lbox_img;
	EwlImage	*prompt_img;
	EwlImage	*trans_img;

	int			l = 15,  t = 15,
				r = 304, b = 203;
	/* Window */
	win = ewl_window_new_with_values("eLogin", 500, 350, "elogin", "elogin");
	elogin_set_window_hints((EwlWindow*) win);
	trans_img = ewl_image_new_with_values("trans", "images/bg.png", "1", "1", "false");
	
	/* Big box */
	box1 = ewl_hbox_new(FALSE);
	ewl_widget_set_background(box1, trans_img->im);
	
	/* left box */
	box2 = ewl_hbox_new(FALSE);
	ewl_widget_set_background(box2, trans_img->im);
	ewl_container_set_child_padding(box2, &l, &t, &r, &b);
	
	/* right box */
	box3 = ewl_hbox_new(FALSE);
	ewl_widget_set_background(box3, trans_img->im);
	l = 250, t = 200, r = 50, b = 50;
	ewl_container_set_child_padding(box3, &l, &t, &r, &b);
	
	/* logo (top left) */
	logo = ewl_hbox_new(FALSE);
	logo_img = ewl_image_new_with_values("logo", "images/elogin.png", "181", "132", "true");
	ewl_widget_set_background(logo, logo_img->im);
	ewl_box_pack_end(box2, logo);
	ewl_widget_show(logo);
	ewl_box_pack_end(box1, box2);
	ewl_widget_show(box2);
	
	/* login box (bottom right) */
	lbox = ewl_hbox_new(FALSE);
	lbox_img = ewl_image_new_with_values("lbox", "images/pbox.png", "200", "100", "true");
	ewl_widget_set_background(lbox, lbox_img->im);
	ewl_box_pack_end(box3, lbox);
	ewl_widget_show(lbox);
	ewl_box_pack_end(box1, box3);
	ewl_widget_show(box3);
	
	
	
	
	
	/* finally pack the window and show everything */
	ewl_window_pack(win, box1);
	ewl_widget_show(box1);
	ewl_widget_show(win);
}


void
elogin_set_window_hints (EwlWindow *win)
{
	Atom		WM_HINTS;
	EwlState	*s = ewl_state_get();
//	XSizeHints	*xsh;
	int			x, y;

	x = (DisplayWidth(s->disp, DefaultScreen(s->disp)) - (500)) / 2;
	y = (DisplayHeight(s->disp, DefaultScreen(s->disp)) - (350)) / 2;

	WM_HINTS = XInternAtom(s->disp, "_MOTIF_WM_HINTS", True);
    if ( WM_HINTS != None ) { 
		MWMHints decorhints = { MWM_HINTS_DECORATIONS, 0, 0, 0, 0 };
		MWMHints sizehints = { MWM_HINTS_FUNCTIONS, 2, 0, 0, 0 };

        XChangeProperty(s->disp, win->xwin, WM_HINTS, WM_HINTS, 32,
		    PropModeReplace, (unsigned char *)&decorhints,
			sizeof(MWMHints)/4);
		
/*        XChangeProperty(s->disp, win->xwin, WM_HINTS, WM_HINTS, 32,
		    PropModeReplace, (unsigned char *)&sizehints,
			sizeof(MWMHints)/4);
*/
	}


//	XMoveWindow(s->disp, win->xwin, x, y);
//	xsh = XAllocSizeHints();
//	xsh->min_aspect = (0, 0);
//	xsh->max_aspect.x = x;
//	xsh->max_aspect.y = y;
//	XSetWMSizeHints(s->disp, win->xwin, xsh, WM_HINTS);
//	XFree(xsh);
}

/*
void
elogin_set_window_location (EwlWindow *win)
{
	EwlState	*s = ewl_state_get();

	int x = (DisplayWidth(s->disp, DefaultScreen(s->disp)) - (500)) / 2;
	int y = (DisplayHeight(s->disp, DefaultScreen(s->disp)) - (350)) / 2;
}
*/

char cb_test_option(int argc, char *argv[])
{
    fprintf(stderr,"wahoo!\n");
    return 1;
}

char cb_mouse(EwlWidget *w, EwlEvent *ev, EwlData *d)
{
    char evtype[8] = "";

    if (ev->type==EWL_EVENT_MOUSEDOWN)
            sprintf(evtype,"down");
    else if (ev->type==EWL_EVENT_MOUSEUP)
            sprintf(evtype,"up");
    else if (ev->type==EWL_EVENT_MOUSEMOVE)
            sprintf(evtype,"move");

    fprintf(stderr,"mouse%s in widget 0x%08x\n", evtype, (unsigned int) w);
    return TRUE;
}

char cb_keydown(EwlWidget *w, EwlEvent *ev, EwlData *d)
{
    fprintf(stderr,"keydown in widget 0x%08x\n", (unsigned int) w);
    ewl_quit();
    return TRUE;
}
