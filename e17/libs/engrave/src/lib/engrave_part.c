#include <Engrave.h>

/**
 * engrave_part_new - create a new part object.
 * @param type: The Engrave_Part_Type for the part.
 * 
 * @return Returns a pointer to the newly allocated Engrave_Part on sucess
 * or NULL on failure.
 */
Engrave_Part *
engrave_part_new(Engrave_Part_Type type)
{
  Engrave_Part *part;
  part = NEW(Engrave_Part, 1);
  if (!part) return NULL;

  part->type = type;
  return part;
}

/**
 * engrave_part_mouse_events_set - set the mouse events flag for the part.
 * @param ep: The Engrave_Part to set the mouse events flag on.
 * @param val: The value to set the mouse events flag too.
 *
 * @return Returns no value.
 */
void
engrave_part_mouse_events_set(Engrave_Part *ep, int val)
{
  if (!ep) return;
  ep->mouse_events = val;
}

/**
 * engrave_part_repeat_events_set - set the repeat events flag for the part.
 * @param ep: The Engrave_Part to set the repeat events flag on.
 * @param val: The value to set the repeat events flag too.
 *
 * @return Returns no value.
 */
void
engrave_part_repeat_events_set(Engrave_Part *ep, int val)
{
  if (!ep) return;
  ep->repeat_events = val;
}

/**
 * engrave_part_name_set - set the name of the part.
 * @param ep: The Engrave_Part to set the name off
 * @param name: The name to attach to the part.
 *
 * @return Returns no value.
 */
void
engrave_part_name_set(Engrave_Part *ep, char *name)
{
  if (!ep) return;
  IF_FREE(ep->name);
  ep->name = (name ? strdup(name) : NULL);
}

/**
 * engrave_part_name_get - Get the name of the part
 * @param ep: The Engrave_Part to retrieve the name from.
 * 
 * @return Returns a copy of the part name, this pointer must be freed by
 * the application, or NULL if no name found.
 */
char *
engrave_part_name_get(Engrave_Part *ep)
{
    if (!ep || !ep->name) return NULL;
    return strdup(ep->name);
}

/**
 * engrave_part_type_set - set the type of the part
 * @param ep: The Engrave_Part to set the type on.
 * @param type: The Engrave_Part_Type to set on the part
 *
 * @return Returns no value.
 */
void
engrave_part_type_set(Engrave_Part *ep, Engrave_Part_Type type)
{
  if (!ep) return;
  ep->type = type;
}

/**
 * engrave_part_effect_set - set the effect on the given part.
 * @param ep: The Engrave_Part to set the effect upon.
 * @param effect: The Engrave_Text_Effect to set on the part.
 *
 * @return Returns no value.
 */
void
engrave_part_effect_set(Engrave_Part *ep, Engrave_Text_Effect effect)
{
  if (!ep) return;
  ep->effect = effect;
}

/**
 * engrave_part_clip_set - set the clip of the given part.
 * @param ep: The Engrave_Part to set the clip on
 * @param clip_to: The name of the part to clip too.
 *
 * @return Returns no value.
 */
void
engrave_part_clip_to_set(Engrave_Part *ep, char *clip_to)
{
  if (!ep) return;
  IF_FREE(ep->clip_to);
  ep->clip_to = (clip_to ? strdup(clip_to) : NULL);
}

/**
 * engrave_part_dragable_x_set - set the dragable x value on the part.
 * @param ep: The Engrave_Part to set the x dragable on.
 * @param x: The x value to set on the dragable.
 * @param step: The step value to set on the dragable.
 * @param count: The count value to set on the dragable.
 *
 * @return Returns no value.
 */
void
engrave_part_dragable_x_set(Engrave_Part *ep, int x, int step, int count)
{
  if (!ep) return;
  ep->dragable.x = x;
  ep->dragable.step.x = step;
  ep->dragable.count.x = count;
}

/**
 * engrave_part_dragable_y_set - set the dragable y value on the part.
 * @param ep: The Engrave_Part to set the y dragable on.
 * @param y: The y value to set on the dragable.
 * @param step: The step value to set on the dragable.
 * @param count: The count value to set on the dragable.
 *
 * @return Returns no value.
 */
void
engrave_part_dragable_y_set(Engrave_Part *ep, int y, int step, int count)
{
  if (!ep) return;
  ep->dragable.y = y;
  ep->dragable.step.y = step;
  ep->dragable.count.y = count;
}

/**
 * engrave_part_dragable_confine_set - set the confine of the parts dragable.
 * @param ep: The Engrave_Part to confine the dragable on.
 * @param confine: The name of the part to confine the dragable to.
 *
 * @return Returns no value.
 */
void
engrave_part_dragable_confine_set(Engrave_Part *ep, char *confine)
{
  if (!ep) return;
  IF_FREE(ep->dragable.confine);
  ep->dragable.confine = (confine ? strdup(confine) : NULL);
}

/**
 * engrave_part_state_last_get - get the last state in the part.
 * @param ep: The Engrave_Part to retrieve the state from.
 *
 * @return Returns the last @a Engrave_Part_State value in the part or NULL if no
 * such value exists.
 */
Engrave_Part_State *
engrave_part_state_last_get(Engrave_Part *ep)
{
  if (!ep) return NULL;
  return evas_list_data(evas_list_last(ep->states));
}

/**
 * engrave_part_state_add - add the state to the part.
 * @param ep: The Engrave_Part to add the state too.
 * @param eps: The Engrave_Part_State to add to the part.
 *
 * @return Returns no value.
 */
void
engrave_part_state_add(Engrave_Part *ep, Engrave_Part_State *eps)
{
  if (!ep || !eps) return;
  ep->states = evas_list_append(ep->states, eps);
}

/**
 * engrave_part_state_by_name_value_find - Find the Engrave_Part State that * matches the given name and value in this part
 * @param ep: The Engrave_Part to search.
 * @param name: The name to search for
 * @param val: The value to search for.
 * 
 * @return Returns a pointer to the Engrave_Part_State matching the given
 * criteria or NULL if not found.
 */
Engrave_Part_State *
engrave_part_state_by_name_value_find(Engrave_Part *ep, 
                                            char *name, double val)
{
  Evas_List *l;
  if (!ep || !name) return NULL;

  for (l = ep->states; l; l = l->next) {
    char *state_name;
    double state_val;
    Engrave_Part_State *eps;
    
    eps = (Engrave_Part_State *)l->data;
    state_name = engrave_part_state_name_get(eps, &state_val);

    if ((!strcmp(state_name, name)) && (strlen(name) == strlen(state_name))
        && (state_val == val)) {
      free(state_name);
      return eps;
    }
    free(state_name);
  }

  return NULL;
}


