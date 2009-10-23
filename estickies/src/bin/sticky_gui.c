#include "stickies.h"

/* INTERNAL FUNCTIONS, USUALLY CALLBACKS PROTOTYPES.
 */
static void _stick_toggle_signal_toggle(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _lock_toggle_signal_toggle(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _actions_toggle_signal_toggle(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _close_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source);

static void _below_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _above_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _normal_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source);

/* FUNCTIONS WHICH HAVE PROTOTYPES DEFINED IN STICKIES.H.
 */
ESAPI void
_e_sticky_win_add(E_Sticky *s)
{
   s->state[1] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
   s->state[2] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
   s->state[3] = ECORE_X_WINDOW_STATE_UNKNOWN;
   //////////
   s->win = elm_win_add(NULL, "estickies", ELM_WIN_BASIC);
   elm_win_title_set(s->win, "estickies");
   elm_win_borderless_set(s->win, 1);
   //////////
   s->xwin = elm_win_xwindow_get(s->win);
   ecore_x_netwm_window_state_set(s->xwin, s->state, 3);
}

ESAPI void
_e_sticky_edje_add(E_Sticky *s)
{
   char theme[PATH_MAX];

   if (!s->theme) s->theme = strdup(ss->theme);
   snprintf(theme, sizeof(theme), "%s/themes/%s", PACKAGE_DATA_DIR, s->theme);

   s->sticky = edje_object_add(evas_object_evas_get(s->win));
   evas_object_size_hint_weight_set(s->sticky, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   edje_object_file_set(s->sticky, theme, "base");

   edje_object_signal_callback_add(s->sticky, "estickies,close_button,click", "", _close_button_signal_click, s);
   edje_object_signal_callback_add(s->sticky, "estickies,stick_toggle,toggle", "",  _stick_toggle_signal_toggle, s);
   edje_object_signal_callback_add(s->sticky, "estickies,lock_toggle,toggle", "",  _lock_toggle_signal_toggle, s);
   edje_object_signal_callback_add(s->sticky, "estickies,actions_toggle,toggle", "",  _actions_toggle_signal_toggle, s);

   edje_object_signal_callback_add(s->sticky, "estickies,below_button,click", "", _below_button_signal_click, s);
   edje_object_signal_callback_add(s->sticky, "estickies,above_button,click", "", _above_button_signal_click, s);
   edje_object_signal_callback_add(s->sticky, "estickies,normal_button,click", "", _normal_button_signal_click, s);

   if (s->stick_toggle_state)
     edje_object_signal_emit(s->sticky, "estickies,stick_toggle,on", "estickies");
   else
     edje_object_signal_emit(s->sticky, "estickies,stick_toggle,off", "estickies");

   if (s->lock_toggle_state)
     edje_object_signal_emit(s->sticky, "estickies,lock_toggle,on", "estickies");
   else
     edje_object_signal_emit(s->sticky, "estickies,lock_toggle,off", "estickies");

   if (s->actions_toggle_state)
     edje_object_signal_emit(s->sticky, "estickies,actions_toggle,on", "estickies");
   else
   edje_object_signal_emit(s->sticky, "estickies,actions_toggle,off", "estickies");

   Evas_Modifier_Mask mask;
   mask = evas_key_modifier_mask_get(evas_object_evas_get(s->win), "Control");
   evas_object_key_grab(s->sticky, "n", mask, 0, 0);
   evas_object_key_grab(s->sticky, "d", mask, 0, 0);
   evas_object_key_grab(s->sticky, "q", mask, 0, 0);
   evas_object_key_grab(s->sticky, "a", mask, 0, 0);
}

ESAPI void
_e_sticky_scroller_add(E_Sticky *s)
{
   s->scroller = elm_scroller_add(s->win);
   elm_scroller_policy_set(s->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(s->scroller, 0, 0);
   elm_object_style_set(s->scroller, "sticky");

   evas_object_size_hint_weight_set(s->scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
}

ESAPI void
_e_sticky_entry_add(E_Sticky *s)
{
   s->textentry = elm_entry_add(s->win);
   if (s->text)
     elm_entry_entry_set(s->textentry, s->text);
   else
     elm_entry_entry_set(s->textentry, NULL);
   elm_entry_line_wrap_set(s->textentry, 1);

   evas_object_size_hint_weight_set(s->textentry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->textentry, EVAS_HINT_FILL, EVAS_HINT_FILL);
}

/* CALLBACKS THEMSELVES
 */
static void
_below_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   E_Sticky *s = data;
   ecore_x_netwm_state_request_send(s->xwin, ecore_x_window_root_get(s->xwin), s->state[3], ECORE_X_WINDOW_STATE_BELOW, 1);
   s->state[3] = ECORE_X_WINDOW_STATE_BELOW;
   s->above = EINA_FALSE;
   s->below = EINA_TRUE;
}

static void
_above_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   E_Sticky *s = data;
   ecore_x_netwm_state_request_send(s->xwin, ecore_x_window_root_get(s->xwin), s->state[3], ECORE_X_WINDOW_STATE_ABOVE, 1);
   s->state[3] = ECORE_X_WINDOW_STATE_ABOVE;
   s->above = EINA_TRUE;
   s->below = EINA_FALSE;
}

static void
_normal_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   E_Sticky *s = data;
   ecore_x_netwm_state_request_send(s->xwin, ecore_x_window_root_get(s->xwin), s->state[3], ECORE_X_WINDOW_STATE_UNKNOWN, 0);
   s->state[3] = ECORE_X_WINDOW_STATE_UNKNOWN;
   s->above = EINA_FALSE;
   s->below = EINA_FALSE;
}

static void
_close_button_signal_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   E_Sticky *s = data;
   evas_object_smart_callback_call(s->sticky, "close_button_clicked", NULL);
}

static void
_stick_toggle_signal_toggle(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   E_Sticky *s = data;
   s->stick_toggle_state = !s->stick_toggle_state;
   if (s->stick_toggle_state)
     edje_object_signal_emit(s->sticky, "estickies,stick_toggle,on", "estickies");
   else
     edje_object_signal_emit(s->sticky, "estickies,stick_toggle,off", "estickies");
   evas_object_smart_callback_call(s->sticky, "stick_toggle_changed", NULL);
}

static void
_lock_toggle_signal_toggle(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   E_Sticky *s = data;
   s->lock_toggle_state = !s->lock_toggle_state;
   if (s->lock_toggle_state)
     edje_object_signal_emit(s->sticky, "estickies,lock_toggle,on", "estickies");
   else
     edje_object_signal_emit(s->sticky, "estickies,lock_toggle,off", "estickies");
   evas_object_smart_callback_call(s->sticky, "lock_toggle_changed", NULL);
}

static void
_actions_toggle_signal_toggle(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   E_Sticky *s = data;
   s->actions_toggle_state = !s->actions_toggle_state;
   if (s->actions_toggle_state)
     edje_object_signal_emit(s->sticky, "estickies,actions_toggle,on", "estickies");
   else
     edje_object_signal_emit(s->sticky, "estickies,actions_toggle,off", "estickies");
   evas_object_smart_callback_call(s->sticky, "actions_toggle_changed", NULL);
}
