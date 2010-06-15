/* Gtk */
#include <libglademm/xml.h>
#include <gtkmm.h>

/* STD */
#include <iostream>

/* Project */
#include "searchFile.h"
#include "Application.h"

   
int main (int argc, char *argv[])
{
  Gtk::Main kit(argc, argv);
  
  Application window;
  Gtk::Main::run(window);

  return 0;
}
