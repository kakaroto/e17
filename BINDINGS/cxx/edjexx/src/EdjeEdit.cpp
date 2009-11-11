#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/EdjePart.h"
#include "../include/edjexx/EdjeEdit.h"
#include "../include/edjexx/EdjeBase.h"
#include "../include/edjexx/EdjeExceptions.h"

/* EFL */
#include <Edje.h>
#include <Edje_Edit.h>

using namespace std;

namespace efl {
  
EdjeEdit::EdjeEdit (EvasCanvas &canvas) :
  EdjeObject (canvas)
{
}
  
EdjeEdit::EdjeEdit (EvasCanvas &canvas, const std::string &filename, const std::string &groupname) :
  EdjeObject (canvas, filename, groupname)
{
}

const string EdjeEdit::getCompiler ()
{
  return edje_edit_compiler_get (o);
}

bool EdjeEdit::save ()
{
  return edje_edit_save (o);
}
  
void EdjeEdit::printInternalStatus ()
{
  edje_edit_print_internal_status (o);
}


} // end namespace efl
