/* Project */
#include "../include/edjexx/PartEdit.h"
#include "../include/edjexx/Edit.h"

namespace Edjexx {

PartEdit::PartEdit (Edit &edit, const std::string &part) :
  mEdit (&edit),
  mPart (part)
{  
}

bool PartEdit::setName (const std::string &newName)
{
  return edje_edit_part_name_set (mEdit->obj (), mPart.c_str (), newName.c_str ());
}

Edje_Part_Type PartEdit::getType () const
{
  return edje_edit_part_type_get (mEdit->obj (), mPart.c_str ());
}

void PartEdit::addState (const std::string &name, double value)
{
  edje_edit_state_add (mEdit->obj (), mPart.c_str (), name.c_str (), value);
}

void PartEdit::delState (const std::string &state, double value)
{
  edje_edit_state_del (mEdit->obj (), mPart.c_str (), state.c_str (), value);
}

bool PartEdit::hasState (const std::string &state, double value)
{
  return edje_edit_state_exist (mEdit->obj (), mPart.c_str (), state.c_str (), value);
}

} // end namespace Edjexx
