#ifndef ENGRAVE_PART_H
#define ENGRAVE_PART_H



/**
 * @file engrave_part.h Engrave_Part object functions.
 * @brief Contains all functions to maniuplate the Engrave_Part object.
 */

/**
 * @defgroup Engrave_Part Engrave_Part: Functions to work with part objects
 *
 * @{
 */

/**
 * The Engrave_Part typedef 
 */
typedef struct _Engrave_Part Engrave_Part;

/**
 * @brief All values needed to define a part 
 */
struct _Engrave_Part
{
  char *name;                   /**< The part name */
  Engrave_Part_Type type;       /**< The part type */
  Engrave_Text_Effect effect;   /**< The parts text effect */
  int mouse_events;             /**< The mouse events flag */
  int repeat_events;            /**< The repeat events flag */
  int precise_is_inside;        /**< The precise event flag */
  int pointer_mode;             /**< The pointer mode flag */
  char *clip_to;                /**< The part to clip too */
  char *source;			/**< The source of the part */

  /**
   * The dragable settings for the part 
   */
  struct
  {
    signed char x; /**< Can drag in x direction */
    signed char y; /**< Can drag in y direction */

    /**
     * The step and count values for the dragable
     */
    struct
    {
      int x;    /**< The x value */
      int y;    /**< The y value */
    } step, count; 
    char *confine;  /**< The confine setting for the dragable */

  } dragable;

  Eina_List *states;    /**< The list of states assocated with the part */
  Engrave_Part_State *current_state; /**< The current state */

  struct
  {
    Evas_Coord x;   /**< The x position to place the object */
    Evas_Coord y;   /**< The y position to place the object */
    Evas_Coord w;   /**< The width of the object */
    Evas_Coord h;   /**< The height of the object */
  } pos;

  Evas_Object *object;  /**< The evas object used to display this state */
  
  void *parent;    /**< Pointer to parent */
};

EAPI Engrave_Part * engrave_part_new(Engrave_Part_Type type);
EAPI void engrave_part_free(Engrave_Part *ep);

EAPI void engrave_part_parent_set(Engrave_Part *ep, void *eg);
EAPI void *engrave_part_parent_get(Engrave_Part *ep);

EAPI void engrave_part_current_state_set(Engrave_Part *ep, 
                                         Engrave_Part_State *eps);
EAPI Engrave_Part_State *engrave_part_current_state_get(Engrave_Part *ep);

EAPI void engrave_part_name_set(Engrave_Part *ep, const char *name);
EAPI void engrave_part_mouse_events_set(Engrave_Part *ep, int val);
EAPI void engrave_part_repeat_events_set(Engrave_Part *ep, int val);
EAPI void engrave_part_precise_is_inside_set(Engrave_Part *ep, int val);
EAPI void engrave_part_pointer_mode_set(Engrave_Part *ep, int val);
EAPI void engrave_part_type_set(Engrave_Part *ep, Engrave_Part_Type type);
EAPI void engrave_part_source_set(Engrave_Part *ep, char * source);
EAPI void engrave_part_effect_set(Engrave_Part *ep, Engrave_Text_Effect effect);
EAPI void engrave_part_clip_to_set(Engrave_Part *ep, const char *clip_to);

EAPI Engrave_Part_State *engrave_part_state_last_get(Engrave_Part *ep);

EAPI void engrave_part_dragable_x_set(Engrave_Part *ep, int x, int step, int count);
EAPI void engrave_part_dragable_y_set(Engrave_Part *ep, int y, int step, int count);
EAPI void engrave_part_dragable_confine_set(Engrave_Part *ep, const char *confine);
EAPI void engrave_part_state_add(Engrave_Part *ep, Engrave_Part_State *eps);
EAPI void engrave_part_state_remove(Engrave_Part *ep, Engrave_Part_State *eps);
EAPI Engrave_Part_State * engrave_part_state_by_name_value_find(Engrave_Part *ep, 
                                                                const char *name, double val);

EAPI const char *engrave_part_name_get(Engrave_Part *ep);
EAPI Engrave_Part_Type engrave_part_type_get(Engrave_Part *ep);
EAPI char * engrave_part_source_get(Engrave_Part *ep);
EAPI int engrave_part_mouse_events_get(Engrave_Part *ep);
EAPI int engrave_part_repeat_events_get(Engrave_Part *ep);
EAPI int engrave_part_precise_is_inside_get(Engrave_Part *ep);
EAPI int engrave_part_pointer_mode_get(Engrave_Part *ep);
EAPI Engrave_Text_Effect engrave_part_effect_get(Engrave_Part *ep);
EAPI const char *engrave_part_clip_to_get(Engrave_Part *ep);
EAPI void engrave_part_dragable_x_get(Engrave_Part *ep, int *x, int *step, int *count);
EAPI void engrave_part_dragable_y_get(Engrave_Part *ep, int *y, int *step, int *count);
EAPI const char *engrave_part_dragable_confine_get(Engrave_Part *ep);

EAPI int engrave_part_states_count(Engrave_Part *ep);

EAPI void engrave_part_state_foreach(Engrave_Part *ep, 
                                     void (*func)(Engrave_Part_State *, Engrave_Part *, void *),
                                     void *data);
                                     
EAPI Evas_Object *engrave_part_evas_object_get(Engrave_Part *ep);
EAPI void engrave_part_evas_object_set(Engrave_Part *ep, Evas_Object *o);

/**
 * @}
 */

#endif

