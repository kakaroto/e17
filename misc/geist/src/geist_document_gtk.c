#include "geist_document_gtk.h"

void
geist_document_render_to_window(geist_document * doc)
{
   Window xwin;

   D_ENTER(3);

   xwin = GDK_WINDOW_XWINDOW(doc->darea->window);

   XSetWindowBackgroundPixmap(disp, xwin, doc->pmap);

   XClearWindow(disp, xwin);

   D_RETURN_(3);
}

void
geist_document_render_to_window_partial(geist_document * doc,
                                            int x, int y,
                                            int w, int h)
{
   Window xwin;

   D_ENTER(3);

   xwin = GDK_WINDOW_XWINDOW(doc->darea->window);

   XSetWindowBackgroundPixmap(disp, xwin, doc->pmap);

   XClearArea(disp, xwin, x, y, w, h, False);

   D_RETURN_(3);
}
