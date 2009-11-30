#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/eetxx/Document.h"

namespace Eetxx {

void Document::init ()
{
  eet_init ();
}
  
void Document::close ()
{
  eet_close (_ef);
}

Eet_File_Mode Document::mode ()
{
  return eet_mode_get (_ef);
}

Eet_File *Document::get ()
{
  return _ef;
}

} // end namespace Eetxx
