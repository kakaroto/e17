#ifndef PART_EDIT_H
#define PART_EDIT_H

/* STD */
#include <string>

/* EFL */
#include <Edje.h>
#include <Edje_Edit.h>

/* EFL++ */
#include <eflxx/Common.h>
#include <eflxx/CountedPtr.h>
#include <einaxx/Einaxx.h>

namespace Edjexx {
  
/* forward declarations */
class Edit;

class PartEdit
{
public:
  friend class Edit;  
  
  /*! 
   * Set a new name for part.
   * Note that the relative getter function don't exist as it don't make sense ;)
   *
   * @param part The name of the part to rename
   * @param newName The new name for the part
   *
   * @return true on success, false on failure
   */
  bool setName (const std::string &newName);
  
  /*!
   * Get the type of a part
   *
   * @param part The name of the part
   *
   * @return One of: EDJE_PART_TYPE_NONE, EDJE_PART_TYPE_RECTANGLE, 
   *         EDJE_PART_TYPE_TEXT,EDJE_PART_TYPE_IMAGE, EDJE_PART_TYPE_SWALLOW, 
   *         EDJE_PART_TYPE_TEXTBLOCK,EDJE_PART_TYPE_GRADIENT or EDJE_PART_TYPE_GROUP
   */
  Edje_Part_Type getType ();
  
  /*!
   * Create a new state to the give part
   *
   * @param part The name of the part
   * @param name The name for the new state (not including the state value)
   */
  void addState (const std::string &name);
  
  /*!
   * Delete the given part state from the edje
   *
   * @param part The name of the part that contain state
   * @param state The current name of the state (including the state value)
   */
  void delState (const std::string &state);

  /*!
   * Check if a part state with the given name exist.
   *
   * @param part The name of the partial
   * @param state The name of the state to check (including the state value)
   *
   * @return true if the part state exist, false otherwise
   */
  bool hasState (const std::string &state);

  
private:
  PartEdit ();
  PartEdit (Edit &edit, const std::string &part);
  
  Edit *mEdit;
  std::string mPart;
};

} // end namespace Edjexx

#endif // PART_EDIT_H
