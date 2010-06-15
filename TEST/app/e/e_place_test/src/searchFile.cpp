#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/* STD */
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Project */
#include "searchFile.h"

using namespace std;

Glib::RefPtr<Gtk::Builder> createGlade (const std::string &gladeFile, const Glib::ustring &object_id)
{
  return Gtk::Builder::create_from_file (searchGladeFile (gladeFile), object_id);
}

Glib::RefPtr<Gtk::Builder> createGlade (const std::string &gladeFile)
{
  return Gtk::Builder::create_from_file (searchGladeFile (gladeFile));
}

const std::string searchGladeFile (const std::string &glade_file)
{
  vector <string> name_vector;
  
  name_vector.push_back (string (PACKAGE_SOURCE_DIR) + "/src/" + glade_file);
  name_vector.push_back (string (PACKAGE_DATA_DIR) + "/glade/" + glade_file);

  return searchFile (name_vector);
} 

const std::string searchDataDir ()
{
  vector <string> name_vector;

  name_vector.push_back (string (PACKAGE_SOURCE_DIR) + "/data/");
  name_vector.push_back (string (PACKAGE_DATA_DIR) + "/data/");

  return searchFile (name_vector);
}

const std::string searchPixmapFile (std::string pixmap_file)
{
  vector <string> name_vector;

  name_vector.push_back (string (PACKAGE_SOURCE_DIR) + "/pixmaps/" + pixmap_file);
  name_vector.push_back (string (PACKAGE_PIXMAPS_DIR) + "/" + pixmap_file);

  return searchFile (name_vector);
}

const std::string searchFile (std::vector <std::string> &name_vector)
{
  struct stat buf;

  for (unsigned int i = 0; i < name_vector.size (); i++)
  {
    string &try_name = name_vector[i];

    bool found = !(stat (try_name.c_str (), &buf));

    if (found)
    {
      //cout << try_name << endl;
      return try_name;
    }
  }

  return "";
}
