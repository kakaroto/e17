/* Project */
#include "TestWindow.h"
#include "searchFile.h"

/* STD */
#include <iostream>

using namespace std;

TestWindow::TestWindow (const Glib::ustring &title, const std::string &helpImage) :
  mImageHelp (searchPixmapFile (helpImage))
{
  set_border_width (5);
  set_title (title);

  add (mVBox);
  mVBox.pack_start (mImageHelp);

  mVBox.show_all ();
}

TestWindow::~TestWindow ()
{
}

int TestWindow::getImageWidth () const
{
  const Glib::RefPtr <const Gdk::Pixbuf> pixbuf = mImageHelp.get_pixbuf ();
  return pixbuf->get_width ();
}

int TestWindow::getImageHeight () const
{
  const Glib::RefPtr <const Gdk::Pixbuf> pixbuf = mImageHelp.get_pixbuf ();
  return pixbuf->get_height ();
}
