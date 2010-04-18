/* Project */
#include "../include/edjexx/StateEdit.h"

namespace Edjexx {

StateEdit::StateEdit (Edit &edit, const std::string &part, const std::string &state, double value) :
  mEdit (&edit),
  mPart (part),
  mState (state),
  mValue (value)
{
}
  
bool StateEdit::setName (const std::string &newName, double newValue)
{
  bool ret = edje_edit_state_name_set (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue, newName.c_str (), newValue);
  if (ret)
  {
    mState = newName;
    mValue = newValue; 
  }
  
  return ret;
}

double StateEdit::getXRelativeRel1 () const
{
  return edje_edit_state_rel1_relative_x_get (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue);
}

double StateEdit::getYRelativeRel1 () const
{
  return edje_edit_state_rel1_relative_y_get (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue);
}

double StateEdit::getXRelativeRel2 () const
{
  return edje_edit_state_rel2_relative_x_get (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue);
}

double StateEdit::getYRelativeRel2 () const
{
  return edje_edit_state_rel2_relative_y_get (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue);
}

void StateEdit::setXRelativeRel1 (double x)
{
  edje_edit_state_rel1_relative_x_set (mEdit->obj (), mPart.c_str (), mState.c_str (), x, mValue);
}

void StateEdit::setYRelativeRel1 (double y)
{
  edje_edit_state_rel1_relative_y_set (mEdit->obj (), mPart.c_str (), mState.c_str (), y, mValue);
}

void StateEdit::setXRelativeRel2 (double x)
{
  edje_edit_state_rel2_relative_x_set (mEdit->obj (), mPart.c_str (), mState.c_str (), x, mValue);
}

void StateEdit::setYRelativeRel2 (double y)
{
  edje_edit_state_rel2_relative_y_set (mEdit->obj (), mPart.c_str (), mState.c_str (), y, mValue);
}

bool StateEdit::getVisibility ()
{
  return edje_edit_state_visible_get (mEdit->obj (), mPart.c_str (), mState.c_str (), mValue);
}

void StateEdit::setVisibility (bool visible)
{
  edje_edit_state_visible_set (mEdit->obj (), mPart.c_str (), mState.c_str (), visible, mValue);
}

void StateEdit::copyFrom (StateEdit &state)
{
  setXRelativeRel1 (state.getXRelativeRel1 ());
  setYRelativeRel1 (state.getYRelativeRel1 ());
  setXRelativeRel2 (state.getXRelativeRel2 ());
  setYRelativeRel2 (state.getYRelativeRel2 ());
  
  setVisibility (state.getVisibility ());
}

} // end namespace Edjexx
