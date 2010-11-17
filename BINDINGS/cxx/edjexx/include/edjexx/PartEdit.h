#ifndef PART_EDIT_H
#define PART_EDIT_H

/* STD */
#include <string>

/* EFL */
#include <Edje.h>
#define EDJE_EDIT_IS_UNSTABLE_AND_I_KNOW_ABOUT_IT
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
  PartEdit (Edit &edit, const std::string &part);
  
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
  Edje_Part_Type getType () const;
  
  /*!
   * Create a new state to the give part
   *
   * @param part The name of the part
   * @param name The name for the new state (not including the state value)
   */
  void addState (const std::string &name, double value);
  
  /*!
   * Delete the given part state from the edje
   *
   * @param part The name of the part that contain state
   * @param state The current name of the state (including the state value)
   */
  void delState (const std::string &state, double value);

  /*!
   * Check if a part state with the given name exist.
   *
   * @param part The name of the partial
   * @param state The name of the state to check (including the state value)
   *
   * @return true if the part state exist, false otherwise
   */
  bool hasState (const std::string &state, double value);
  
#if 0
/**Move the given part below the previous one.*/
EAPI Eina_Bool         ///@return 1 on success, 0 on failure
edje_edit_part_restack_below(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part to restack
);

/**Move the given part above the next one.*/
EAPI Eina_Bool         ///@return 1 on success, 0 on failure
edje_edit_part_restack_above(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part to restack
);
  
/**Get the clip_to part.
 * NULL is returned on errors and if the part don't have a clip.
 */
EAPI const char *          ///@return The name of the part to clip part to. Use edje_edit_string_free() when you don't need it anymore.
edje_edit_part_clip_to_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set a part to clip part to.
 * If clip_to is NULL then the clipping value will be cancelled (unset clipping)
 */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise.
edje_edit_part_clip_to_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *clip_to     ///< The name of the part to clip part to.
);

/**Get the source of part.
 * Source is used in part of type EDJE_PART_TYPE_GROUP to specify the group to
 * 'autoswallow' inside the given part.
 */
EAPI const char *          ///@return The name of the group to source the given part. Use edje_edit_string_free() when you don't need it anymore.
edje_edit_part_source_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set the source of part.
 * Source is used in part of type EDJE_PART_TYPE_GROUP to specify the group to
 * 'autoswallow' inside the given part.
 * NOTE: This is not applied now. You must reload the edje to see the change.
 */
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise.
edje_edit_part_source_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *source      ///< The name of the group to autoswallow in the given part
);

/**Get the effect for a given part. */
EAPI Edje_Text_Effect         ///@return One of: EDJE_TEXT_EFFECT_NONE, _PLAIN, _OUTLINE, _SOFT_OUTLINE, _SHADOW, _SOFT_SHADOW, _OUTLINE_SHADOW, _OUTLINE_SOFT_SHADOW, _FAR_SHADOW, _FAR_SOFT_SHADOW, _GLOW.
edje_edit_part_effect_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set the effect for a given part. */
EAPI void
edje_edit_part_effect_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   Edje_Text_Effect effect    ///< The effect to set. See edje_edit_part_effect_get() for possible value
);

/**Get the current selected state in part. */
EAPI const char *          ///@return The name of the selected state including the float value. Use edje_edit_string_free() when you don't need it anymore.
edje_edit_part_selected_state_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set the current state in part.*/
EAPI Eina_Bool         ///@return 1 on success, 0 otherwise.
edje_edit_part_selected_state_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   const char *state       ///< The name of the state to set (including the float value)
);

/**Get mouse_events for part.*/
EAPI Eina_Bool         ///@return 1 if part accept mouse events, 0 if not
edje_edit_part_mouse_events_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set mouse_events for part.*/
EAPI void
edje_edit_part_mouse_events_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   Eina_Bool mouse_events ///< If set to 1 part will accept mouse events, 0 to ignore all mouse events from part.
);

/**Get repeat_events for part.*/
EAPI Eina_Bool         ///@return 1 if part will pass all events to the other parts, 0 if not
edje_edit_part_repeat_events_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set repeat_events for part. */
EAPI void
edje_edit_part_repeat_events_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   Eina_Bool repeat_events /**< If set to 1 part will repeat
                                 * all the received mouse events to other parts.
                                 * If set to 0 the events received will not propagate to other parts.*/
);

/**Get ignore_flags for part.*/
EAPI Evas_Event_Flags        ///@return event flags ignored
edje_edit_part_ignore_flags_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set repeat_events for part. */
EAPI void
edje_edit_part_ignore_flags_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   Evas_Event_Flags ignore_flags ///< event flags to be ignored
);

/**Get horizontal dragable state for part.(1, -1 or 0) */
EAPI int                   ///@return 1 (or -1) if the part can be dragged horizontally
edje_edit_part_drag_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set horizontal dragable state for part.(1, -1 or 0) */
EAPI void
edje_edit_part_drag_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   int drag                ///< Set to 1 (or -1) if the part should be dragged horizontally
);

/**Get vertical dragable state for part.(1, -1 or 0) */
EAPI int                   ///@return 1 (or -1) if the part can be dragged vertically
edje_edit_part_drag_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set vertical dragable state for part.(1, -1 or 0) */
EAPI void
edje_edit_part_drag_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   int drag                ///< Set to 1 (or -1) if the part should be dragged vertically
);

/**Get horizontal dragable step for part.*/
EAPI int                   ///@return The drag horizontal step value
edje_edit_part_drag_step_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set horizontal dragable state for part.*/
EAPI void
edje_edit_part_drag_step_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   int step                ///< The step value
);

/**Get vertical dragable step for part.*/
EAPI int                   ///@return The drag vertical step value
edje_edit_part_drag_step_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set vertical dragable state for part.*/
EAPI void
edje_edit_part_drag_step_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   int step                ///< The step value
);

/**Get horizontal dragable count for part.*/
EAPI int                   ///@return The drag horizontal count value
edje_edit_part_drag_count_x_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set horizontal dragable count for part.*/
EAPI void
edje_edit_part_drag_count_x_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   int count               ///< The count value
);

/**Get vertical dragable count for part.*/
EAPI int                   ///@return The drag vertical count value
edje_edit_part_drag_count_y_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the part
);

/**Set vertical dragable count for part.*/
EAPI void
edje_edit_part_drag_count_y_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the part
   int count               ///< The count value
);

/**Get the name of the part that is used as 'confine' for the given draggies.*/
EAPI const char*
edje_edit_part_drag_confine_get(
   Evas_Object *obj,       ///< The edje object
   const char *part        ///< The name of the drag part
);

/**Set the name of the part that is used as 'confine' for the given draggies.*/
EAPI void
edje_edit_part_drag_confine_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the drag part
   const char *confine     ///< The name of the confine part, or NULL to unset confine
);

/**Get the name of the part that is used as the receiver of the drag event.*/
EAPI const char*
edje_edit_part_drag_event_get(
   Evas_Object *obj,      ///< The edje object
   const char *part       ///< The name of the drag part
);

/**Set the name of the part that will recive events from the given draggies.*/
EAPI void
edje_edit_part_drag_event_set(
   Evas_Object *obj,       ///< The edje object
   const char *part,       ///< The name of the drag part
   const char *event       ///< The name of the part that will receive events, or NULL to unset.
);
#endif
  
protected:  
  Edit *mEdit;
  std::string mPart;
};

} // end namespace Edjexx

#endif // PART_EDIT_H
