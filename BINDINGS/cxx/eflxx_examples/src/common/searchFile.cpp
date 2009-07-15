#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include "searchFile.h"

using namespace std;

const std::string searchFontFile (std::string font_file)
{
  vector <string> name_vector;

  name_vector.push_back (string (PACKAGE_SOURCE_DIR) + "/data/fonts/" + font_file);
  name_vector.push_back (string (PACKAGE_DATA_DIR) + "/fonts/" + font_file);

  return searchFile (name_vector);
}

const std::string searchEdjeFile (std::string edje_file)
{
  vector <string> name_vector;

  name_vector.push_back (string (PACKAGE_SOURCE_DIR) + "/data/edjes/" + edje_file);
  name_vector.push_back (string (PACKAGE_DATA_DIR) + "/edjes/" + edje_file);

  return searchFile (name_vector);
}

const std::string searchDataDir ()
{
  vector <string> name_vector;

  name_vector.push_back ("data");
  name_vector.push_back ("../data");
  name_vector.push_back (string (PACKAGE_SOURCE_DIR) + "/data");
  name_vector.push_back (string (PACKAGE_DATA_DIR) + "/");

  return searchFile (name_vector);
}

const std::string searchPixmapFile (std::string pixmap_file)
{
  vector <string> name_vector;

  name_vector.push_back (string (PACKAGE_SOURCE_DIR) + "/data/images/" + pixmap_file);
  name_vector.push_back (string (PACKAGE_PIXMAPS_DIR) + "/images/" + pixmap_file);

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
      return try_name;
    }
  }

  return "";
}
