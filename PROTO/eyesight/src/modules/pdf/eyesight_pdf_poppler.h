/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010 Vincent Torri <vtorri at univ-evry dot fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __EYESIGHT_PDF_PRIVATE_H__
#define __EYESIGHT_PDF_PRIVATE_H__


typedef struct _Eyesight_Backend_Pdf  Eyesight_Backend_Pdf;

struct _Eyesight_Backend_Pdf
{
  char *filename;
  Eyesight_Document_Pdf *document;
  Evas_Object *obj;

  /* Document */
  struct {
    PDFDoc          *pdfdoc;
    Dict            *dict;
    FontInfoScanner *scanner;
    Eina_List       *toc;
  } doc;

  /* Current page */
  struct {
    ::Page              *page;
    Eyesight_Orientation orientation;
    double               hscale;
    double               vscale;
    Eina_List           *links;
  } page;
};


#endif /* __EYESIGHT_PDF_PRIVATE_H__ */
