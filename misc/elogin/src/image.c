/***************************************************************************
                          image.c  -  description
                             -------------------
    begin                : Sat Apr 8 2000
    copyright            : (C) 2000 by Chris Thomas
    email                : x5rings@fsck.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "main.h"
#include "x.h"
#include "mem.h"
#include <Imlib2.h>
#include "image.h"

//Elogin_View		*main_view = NULL;

void
Elogin_ImageSetName(Elogin_Image *im, char *name)
{
	IF_FREE(im->name);
	im->name = name;
}

void
Elogin_ImageSetImage(Elogin_Image *im, Imlib_Image *imlib)
{
	IF_FREE(im->im);
	im->im = imlib;
}

Elogin_Image	*
Elogin_ImageNew	 ()
{
	Elogin_Image	*im;
	im = NEW(Elogin_Image, 1);
	im->name = NULL;
	im->im = NULL;
	im->w = 0;
	im->h = 0;
	im->x = 0;
	im->y = 0;
	return im;
}

Elogin_Image	*
Elogin_LoadImage(char *file)
{
	Elogin_Image	*image;
	image = Elogin_ImageNew();
	printf("Attempting to open file %s\n", file);	
	Elogin_ImageSetName(image, file);
	Elogin_ImageSetImage(image, imlib_load_image(file));
	imlib_context_set_image(image->im);
	image->w = imlib_image_get_width();
	image->h = imlib_image_get_height();
	
	return image;
}

/* Temporary function just so I can figure out how to load images. */
void
Elogin_LoadAll ()
{
	Elogin_Image *lbox, *llogo, *lbg, *lpbox, *lp1, *lp2;
 	lbox = Elogin_LoadImage("images/box2.png");
	llogo = Elogin_LoadImage("images/elogin.png");
	lbg = Elogin_LoadImage("images/bg2.png");
	lpbox = Elogin_LoadImage("images/pbox.png");
	lp1 = Elogin_LoadImage("images/input.png");
	lp2 = Elogin_LoadImage("images/input.png");
 	Elogin_DisplayAll(lbox, llogo, lbg, lpbox, lp1, lp2);
}

void
Elogin_DisplayAll (Elogin_Image *lbox, Elogin_Image *llogo, Elogin_Image *lbg, 
		Elogin_Image *lpbox, Elogin_Image *lp1, Elogin_Image *lp2)
{
	Pixmap pm;
	static Atom	a = 0;
	/* BG */
	pm = e_pixmap_new(default_root, lbg->w, lbg->h, default_depth);
	imlib_context_set_drawable(pm);
	imlib_context_set_image(lbg->im);
	imlib_image_tile();
	imlib_render_image_on_drawable(0, 0);
	//a = e_atom_get("_XROOTPMAP_ID");
	E_ATOM(a, "_XROOTPMAP_ID");
	e_window_property_set(default_root, a, XA_PIXMAP, 32,
		&pm, 1);
	//	(unsigned char *)&pm, 1);

	e_window_set_background_pixmap(default_root, pm);
	e_window_resize(default_root, lbg->w, lbg->h);
	e_window_clear(default_root);
	e_window_show(default_root);
	e_sync();
	imlib_free_image();
	
	/* Logo */
	pm = e_pixmap_new(main_view->win, lbox->w, lbox->h, default_depth);
	imlib_context_set_drawable(pm);
	imlib_context_set_image(lbox->im);
	imlib_blend_image_onto_image(llogo->im, 0,
		0, 0, llogo->w, llogo->h,
		15, 15, llogo->w, llogo->h);
	
	/* lp1 & lp2 */
	imlib_context_set_image(lpbox->im);
	imlib_blend_image_onto_image(lp1->im, 0,
		0, 0, lp1->w, lp1->h,
		50, 25, lp1->w, lp1->h);
	imlib_blend_image_onto_image(lp2->im, 0,
		0, 0, lp2->w, lp2->h,
		50, 55, lp2->w, lp2->h);
		
	/* pbox */
	imlib_context_set_image(lbox->im);
	imlib_blend_image_onto_image(lpbox->im, 0,
		0, 0, lpbox->w, lpbox->h,
		250, 200, lpbox->w, lpbox->h);
	
	imlib_render_image_on_drawable(0, 0);
	e_window_set_background_pixmap(main_view->win, pm);
	e_window_resize(main_view->win, lbox->w, lbox->h);
	e_window_show(main_view->win);
	e_sync();
	
	/* Free me! */
	imlib_context_set_image(lbox->im);imlib_free_image();
	imlib_context_set_image(llogo->im);imlib_free_image();
	imlib_context_set_image(lpbox->im);imlib_free_image();
	imlib_context_set_image(lp1->im);imlib_free_image();
	imlib_context_set_image(lp2->im);imlib_free_image();
}

void
Elogin_SetRootBG (void)
{
	Pixmap pm;
	static Atom	a = 0;
	
	/* BG */
	pm = e_pixmap_new(default_root, main_view->bg->w, 
			main_view->bg->h, default_depth);
	imlib_context_set_drawable(pm);
	imlib_context_set_image(main_view->bg);
	imlib_image_tile();
	imlib_render_image_on_drawable(0, 0);
	E_ATOM(a, "_XROOTPMAP_ID");
	e_window_property_set(default_root, a, XA_PIXMAP, 32,
		&pm, 1);

	e_window_set_background_pixmap(default_root, pm);
	e_window_resize(default_root, main_view->bg->w, 
			main_view->bg->h);
	e_window_clear(default_root);
	e_window_show(default_root);
	e_sync();
	imlib_free_image();
}

Elogin_View		*
Elogin_ViewNew (void)
{
	Elogin_View		*view;

	view = NEW(Elogin_View, 1);
	view->frame = e_window_new(default_root);
	view->win = e_window_new(view->frame);
	view->w = 500;
	view->h = 250;
	view->x = 0;
	view->y = 0;
	view->bg = Elogin_LoadImage("images/bg2.png");
//	view->logo = Elogin_BitLoad("logo");
	view->login_box = Elogin_WidgetLoad("login_box");
//	view->user_box = Elogin_BitLoad("user_box");
//	view->pass_box = Elogin_BitLoad("pass_box");
	view->widgets = NULL;
	view->next = main_view;
	if (view->next)
		view->next->prev = view;
	main_view = view;
	return view;
}

void
Elogin_ViewFree (Elogin_View *view)
{
	e_window_destroy(view->frame);
	/* Free our widgets */
	FREE(view);
}

void
Elogin_Display (void)
{
	Pixmap pm;
	
	pm = e_pixmap_new(main_view->win, main_view->login_box->im->w, 
			main_view->login_box->h, default_depth);
	imlib_context_set_drawable(pm);
	imlib_context_set_image(main_view->login_box->im->im);
	imlib_render_image_on_drawable(0, 0);
	e_window_set_background_pixmap(main_view->win, pm);
	e_window_resize(main_view->win, main_view->login_box->im->w, 
			main_view->login_box->im->h);
	e_window_resize(main_view->win, 100, 100);
	e_window_show(main_view->win);
	e_sync();
	
	imlib_context_set_image(main_view->login_box->im->im);imlib_free_image();
}

