/**********************************************************************
 * dither.h                                               November 2001
 * Horms                                             horms@vergenet.net
 *
 * Clone of ImageMagick's convert programme
 *
 * imlib2_tools
 * Clone of ImageMagick command line programmes using Imlib2.
 * Copyright (C) 2001  Horms
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *   
 * The above copyright notice and this permission notice shall be
 * included in all copies of the Software, its documentation and
 * marketing & publicity materials, and acknowledgment shall be given
 * in the documentation, materials and software packages that this
 * Software was used.
 *      
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************/

#ifndef _IMLIB2_TOOLS_DITHER_H
#define _IMLIB2_TOOLS_DITHER_H

#ifdef HAVE_CONFIG
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#define DITHER_FLOYD_ALTERNATE 0x01

Imlib_Image *dither_ordered_image(Imlib_Image image, int dimension);

Imlib_Image *dither_floyd_image(Imlib_Image image, int flag);

#endif /* _IMLIB2_TOOLS_DITHER_H */
