/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Mon Apr  3 21:36:48 EST 2000
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "main.h"
#include "x.h"
#include "image.h"
#include "events.h"
#include <Imlib2.h>

int
main(int argc, char **argv)
{
	
//	wx =(DisplayWidth(disp, DefaultScreen(disp)) - 500) / 2;
//	wy = (DisplayHeight(disp, DefaultScreen(disp)) - (350)) / 2;

/*	root = XCreateWindow(disp, DefaultRootWindow(disp), 0, 0, 10, 10, 0, depth,
		InputOutput, vis, CWOverrideRedirect |
              CWSaveUnder | CWBackingStore | CWColormap |
              CWBackPixmap | CWBorderPixel, &attr);
*/
		
	/* Get this puppy centered! */

//	Elogin_XineramaCheck();
	
	

//	XSelectInput(disp, win, ButtonPressMask | ButtonReleaseMask |
//		ButtonMotionMask | PointerMotionMask | ExposureMask);
	
 	
//	Elogin_LoadConfigs ();
	e_display_init(NULL);
	Elogin_LoadAll();
	Elogin_EventLoop();

	 return 0;
}