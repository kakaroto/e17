/* alb_document_xml.h

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef ALB_DOCUMENT_XML_H
#define ALB_DOCUMENT_XML_H

#include "alb.h"
#include "alb_object.h"
#include "alb_image.h"
#include "alb_rect.h"
#include "alb_text.h"
#include "alb_line.h"
#include "alb_poly.h"

#include <libxml/parser.h>
#include <libxml/xmlmemory.h>

enum xml_load_err { ERR_NO_FILE, ERR_EMPTY_FILE, ERR_WRONG_TYPE };

int alb_document_save_xml(alb_document * doc,
                          char *filename);
alb_document *alb_document_load_xml(char *xml,
                                    gint * err_return);

#endif
