#ifndef TEST_WINDOW_H
#define TEST_WINDOW_H

#include <gtkmm.h>

class TestWindow : public Gtk::Window
{
public:
  TestWindow (const Glib::ustring &title, const std::string &helpImage);
  virtual ~TestWindow ();

  int getImageWidth () const;
  int getImageHeight () const;

  void add (Widget& widget);
  
protected:
  Gtk::VBox mVBox;
  Gtk::Image mImageHelp;
};

#endif // TEST_WINDOW_H
