#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/eetxx/EetDocument.h"

namespace efl {

void EetDocument::init ()
{
  eet_init ();
}
  
void EetDocument::close ()
{
  eet_close (_ef);
}

Eet_File_Mode EetDocument::mode ()
{
  return eet_mode_get (_ef);
}

Eet_File *EetDocument::get ()
{
  return _ef;
}

} // end namespace efl
