#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/edjexx/Part.h"
#include "../include/edjexx/Edit.h"
#include "../include/edjexx/Base.h"
#include "../include/edjexx/Exceptions.h"

using namespace std;

namespace Edjexx {
  
Edit::Edit (Evasxx::Canvas &canvas) :
  Edjexx::Object (canvas)
{
}
  
Edit::Edit (Evasxx::Canvas &canvas, const std::string &filename, const std::string &groupname) :
  Edjexx::Object (canvas, filename, groupname)
{
}

const string Edit::getCompiler ()
{
  return edje_edit_compiler_get (o);
}

bool Edit::save ()
{
  return edje_edit_save (o);
}
  
void Edit::printInternalStatus ()
{
  edje_edit_print_internal_status (o);
}

bool Edit::addGroup (const std::string &name)
{
  return edje_edit_group_add (o, name.c_str ());
}
 

bool Edit::delGroup (const std::string &group)
{
  return edje_edit_group_del (o, group.c_str ());
}

bool Edit::hasGroup (const std::string &group)
{
  return edje_edit_group_exist (o, group.c_str ());
}

bool Edit::setGroupName (const std::string &newName)
{
  return edje_edit_group_name_set (o, newName.c_str ());
}

int Edit::getGroupMinWidth ()
{
  return edje_edit_group_min_w_get (o);
}

void Edit::setGroupMinWidth (int width)
{
  edje_edit_group_min_w_set (o, width);
}

int Edit::getGroupMinHeight ()
{
  return edje_edit_group_min_h_get (o);
}

void Edit::setGroupMinHeight (int height)
{
  edje_edit_group_min_h_set (o, height);
}

int Edit::getGroupMaxWidth ()
{
  return edje_edit_group_max_w_get (o);
}

void Edit::setGroupMaxWidth (int width)
{
  edje_edit_group_max_w_set(o, width);
}

int Edit::getGroupMaxHeight ()
{
  return edje_edit_group_max_h_get (o);
}

void Edit::setGroupMaxHeight (int height)
{
  edje_edit_group_max_h_set(o, height);
}

/******************************************************************************/
/**************************   PARTS API   *************************************/
/******************************************************************************/

Eflxx::CountedPtr <Einaxx::List <char*>::Iterator> Edit::getPartsNameList ()
{
  Einaxx::List <char*>::Iterator *lst = new Einaxx::List <char*>::Iterator (edje_edit_parts_list_get (o));
  
  return Eflxx::CountedPtr <Einaxx::List <char*>::Iterator> (lst);
}

Eflxx::CountedPtr <PartEdit> Edit::getPart (const std::string &part)
{
  if (hasPart (part))
  {
    PartEdit* ep = new PartEdit (*this, part);
    return Eflxx::CountedPtr <PartEdit> (ep);
  }
  throw PartNotExistingException (part);
}

bool Edit::addPart (const std::string &name, Edje_Part_Type type)
{
  return edje_edit_part_add (o, name.c_str (), type);
}

bool Edit::delPart (const std::string &part)
{
  return edje_edit_part_del (o, part.c_str ());
}

bool Edit::hasPart (const std::string &part)
{
  return edje_edit_part_exist (o, part.c_str ());
}

/******************************************************************************/
/**************************   STATES API   ************************************/
/******************************************************************************/

Eflxx::CountedPtr <Einaxx::List <char*>::Iterator>  Edit::getPartStatesList (const std::string &part)
{
  Einaxx::List <char*>::Iterator *lst = new Einaxx::List <char*>::Iterator (edje_edit_part_states_list_get (o, part.c_str ()));
  
  return Eflxx::CountedPtr <Einaxx::List <char*>::Iterator> (lst);
}

bool Edit::copyState (const std::string &part, const std::string &from, double valFrom, const std::string &to, double valTo)
{
  return edje_edit_state_copy (o, part.c_str (), from.c_str (), valFrom, to.c_str (), valTo);
}


/******************************************************************************/
/**************************   IMAGES API   ************************************/
/******************************************************************************/

Eflxx::CountedPtr <Einaxx::List <char*>::Iterator> Edit::getImagesList ()
{
  Einaxx::List <char*>::Iterator *lst = new Einaxx::List <char*>::Iterator (edje_edit_images_list_get (o));
  
  return Eflxx::CountedPtr <Einaxx::List <char*>::Iterator> (lst); 
}



} // end namespace Edjexx
