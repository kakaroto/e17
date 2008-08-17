/* main.cpp

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

#include "geist.h"
#include "GeistImage.h"
#include "GeistDocument.h"

int
main()
{
   GeistDebug::instance()->set_level(3);

   GeistObject *img = new GeistImage;

   try
   {
      img->add_child(new GeistImage);
   }
   catch (eNoChildren)
   {
      cout << "Good. Can't add children to a leaf class.\n";
   }

   GeistObject *doc = new GeistDocument;

   try
   {
      doc->add_child(new GeistImage);
   }
   catch (eNoChildren)
   {
      cout << "Ack! That should have worked!\n";
   }

   doc->render();
}
