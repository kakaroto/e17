/***************************************************************************
                          image.h  -  description
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
#ifndef E_IMAGE_H
#define E_IMAGE_H 1

#include "x.h"
#include "im_engine.h"
#include <Imlib2.h>

//typedef struct        _elogin_image   Elogin_Image;
typedef struct _elogin_view Elogin_View;

struct _elogin_image
{
   char               *name;
   Imlib_Image        *im;
   int                 x, y, w, h;
};

struct _elogin_view
{
   Window              win, frame;
   int                 x, y, w, h;
   char                visible;
   Elogin_Image       *bg;
   char                focused;
   int                 button_down;
   int                 x_down, y_down;
   int                 start_x, start_y;
   int                 psel_x, psel_y, psel_w, psel_h;
   int                 sel_x, sel_y, sel_w, sel_h;
   char                psel, sel;
   Ewidget            *box, *logo, *login_box;
   Ewidget            *user_box, *pass_box;
//      Elogin_ImageObject              *user_box, *pass_box;
   Ewidget            *widgets;
   Elogin_View        *prev, *next;
};

void                Elogin_ImageSetName(Elogin_Image * im, char *name);
void                Elogin_ImageSetImage(Elogin_Image * im,

					 Imlib_Image * imlib);
void                Elogin_SetImage(Ewidget * bit, char *file);
Elogin_Image       *Elogin_LoadImage(char *file);
Elogin_Image       *Elogin_ImageNew();
Elogin_View        *Elogin_ViewNew();
void                Elogin_LoadAll();
void                Elogin_DisplayAll(Elogin_Image * lbox, Elogin_Image * llogo,
				      Elogin_Image * lbg, Elogin_Image * lpbox,
				      Elogin_Image * lp1, Elogin_Image * lp2);
void                Elogin_Display(void);
#endif
