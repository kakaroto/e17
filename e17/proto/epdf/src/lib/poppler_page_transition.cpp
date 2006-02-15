#include <stdlib.h>
#include <string.h>

#include <Object.h>
#include <Dict.h>
#include <UGooString.h>

#include "poppler_enum.h"
#include "poppler_private.h"
#include "poppler_page_transition.h"


Evas_Poppler_Page_Transition *
evas_poppler_page_transition_new (Object *data)
{
  Evas_Poppler_Page_Transition *transition;
  Dict                         *trans_dict;
  Object                        obj;
  
  if (!data || !data->isDict())
    return NULL;

  transition = (Evas_Poppler_Page_Transition *)malloc (sizeof (Evas_Poppler_Page_Transition));
  if (!transition)
    return NULL;

  transition->type           = EVAS_POPPLER_PAGE_TRANSITION_REPLACE;
  transition->duration       = 1;
  transition->alignment      = EVAS_POPPLER_PAGE_TRANSITION_HORIZONTAL;
  transition->direction      = EVAS_POPPLER_PAGE_TRANSITION_INWARD;
  transition->angle          = 0;
  transition->scale          = 1.0;
  transition->is_rectangular = 0;
  
  trans_dict = data->getDict ();

  UGooString trans_str("S");
  if (trans_dict->lookup (trans_str, &obj)->isName ()) {
    const char *s;

    s = obj.getName();
    if (strcmp ("R", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_REPLACE;
    else if (strcmp("Split", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_SPLIT;
    else if (strcmp("Blinds", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_BLINDS;
    else if (strcmp("Box", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_BOX;
    else if (strcmp("Wipe", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_WIPE;
    else if (strcmp("Dissolve", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_DISSOLVE;
    else if (strcmp("Glitter", s) == 0)
     transition-> type = EVAS_POPPLER_PAGE_TRANSITION_GLITTER;
    else if (strcmp("Fly", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_FLY;
    else if (strcmp("Push", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_PUSH;
    else if (strcmp("Cover", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_COVER;
    else if (strcmp("Uncover", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_UNCOVER;
    else if (strcmp("Fade", s) == 0)
      transition->type = EVAS_POPPLER_PAGE_TRANSITION_FADE;
  }
  obj.free();

  if (trans_dict->lookup ("D", &obj)->isInt ()) {
    transition->duration = obj.getInt();
  }
  obj.free();

  if (trans_dict->lookup("Dm", &obj)->isName()) {
    const char *dm = obj.getName();
    if ( strcmp( "H", dm ) == 0 )
      transition->alignment = EVAS_POPPLER_PAGE_TRANSITION_HORIZONTAL;
    else if ( strcmp( "V", dm ) == 0 )
      transition->alignment = EVAS_POPPLER_PAGE_TRANSITION_VERTICAL;
  }
  obj.free();

  if (trans_dict->lookup("M", &obj)->isName()) {
    const char *m = obj.getName();
    if ( strcmp( "I", m ) == 0 )
      transition->direction = EVAS_POPPLER_PAGE_TRANSITION_INWARD;
    else if ( strcmp( "O", m ) == 0 )
      transition->direction = EVAS_POPPLER_PAGE_TRANSITION_OUTWARD;
  }
  obj.free();

  if (trans_dict->lookup("Di", &obj)->isInt()) {
    transition->angle = obj.getInt();
  }
  obj.free();

  if (trans_dict->lookup("Di", &obj)->isName()) {
    if ( strcmp( "None", obj.getName() ) == 0 )
      transition->angle = 0;
  }
  obj.free();

  if (trans_dict->lookup("SS", &obj)->isReal()) {
    transition->scale = obj.getReal();
  }
  obj.free();

  if (trans_dict->lookup("B", &obj)->isBool()) {
    transition->is_rectangular = obj.getBool();
  }
  obj.free();

  return transition;
}

void
evas_poppler_page_transition_delete (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return;

  delete transition->transition;
}


Evas_Poppler_Page_Transition_Type
evas_poppler_page_transition_type_get (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return EVAS_POPPLER_PAGE_TRANSITION_REPLACE;

  return transition->type;
}

int
evas_poppler_page_transition_duration_get (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return 1;

  return transition->duration;
}

Evas_Poppler_Page_Transition_Alignment
evas_poppler_page_transition_alignment_get (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return EVAS_POPPLER_PAGE_TRANSITION_HORIZONTAL;

      return transition->alignment;
}

Evas_Poppler_Page_Transition_Direction
evas_poppler_page_transition_direction_get (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return EVAS_POPPLER_PAGE_TRANSITION_INWARD;

  return transition->direction;
}

int
evas_poppler_page_transition_angle_get (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return 0;

  return transition->angle;
}

double
evas_poppler_page_transition_scale_get (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return 1.0;

  return transition->scale;
}

unsigned char
evas_poppler_page_transition_is_rectangular_get (Evas_Poppler_Page_Transition *transition)
{
  if (!transition)
    return 0;

  return transition->is_rectangular;
}
