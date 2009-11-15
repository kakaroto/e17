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

bool EdjeEdit::addGroup (const std::string &name)
{
  return edje_edit_group_add (o, name.c_str ());
}
 

bool EdjeEdit::delGroup ()
{
  return edje_edit_group_del (o);
}

bool EdjeEdit::hasGroup (const std::string &group)
{
  return edje_edit_group_exist (o, group.c_str ());
}

bool EdjeEdit::setGroupName (const std::string &newName)
{
  return edje_edit_group_name_set (o, newName.c_str ());
}

int EdjeEdit::getGroupMinWidth ()
{
  return edje_edit_group_min_w_get (o);
}

void EdjeEdit::setGroupMinWidth (int width)
{
  edje_edit_group_min_w_set (o, width);
}

int EdjeEdit::getGroupMinHeight ()
{
  return edje_edit_group_min_h_get (o);
}

void EdjeEdit::setGroupMinHeight (int height)
{
  edje_edit_group_min_h_set (o, height);
}

int EdjeEdit::getGroupMaxWidth ()
{
  return edje_edit_group_max_w_get (o);
}

void EdjeEdit::setGroupMaxWidth (int width)
{
  edje_edit_group_max_w_set(o, width);
}

int EdjeEdit::getGroupMaxHeight ()
{
  return edje_edit_group_max_h_get (o);
}

void EdjeEdit::setGroupMaxHeight (int height)
{
  edje_edit_group_max_h_set(o, height);
}

} // end namespace efl
