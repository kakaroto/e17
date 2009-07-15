#ifndef SEARCHFILE_H
#define SEARCHFILE_H

#include <string>
#include <vector>

// public search functions
const std::string searchEdjeFile (std::string edje_file);
const std::string searchFontFile (std::string font_file);
const std::string searchPixmapFile (std::string pixmap_file);
const std::string searchDataDir ();

// private
const std::string searchFile (std::vector <std::string> &name_vector);

#endif // SEARCHFILE_H
