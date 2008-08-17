/*
 * vim:ts=4:cino=t0
 */

#include "Express.h"

static void exp_gui_cb_resize(Ecore_Evas *ee);
static int exp_gui_idler_before(void *data);
static void _exp_gui_scroll_callback(void *data, Evas_Object *obj, const char *sig, const char *src);

int
exp_gui_init(Exp *exp)
{
  Evas_Coord mw, mh, dw = 0, dh = 0;
  const char *tmp;

  /* FIXME make this a config option */
  exp->theme_path = strdup(PACKAGE_DATA_DIR"/themes/express.edj");

  exp->ee = ecore_evas_software_x11_new(0,0,0,0,255,255);
  exp->evas = ecore_evas_get(exp->ee);
  ecore_evas_data_set(exp->ee, "exp", exp);

  exp->gui = edje_object_add(exp->evas);
  edje_object_file_set(exp->gui, exp->theme_path, "express.base");
  evas_object_name_set(exp->gui, "express.base");
  edje_object_signal_callback_add(exp->gui, "drag", "scrollbar.bar", _exp_gui_scroll_callback, exp);

  /* FIXME make direction configurable */
  exp->buddy_cont = esmart_container_new(exp->evas);
  esmart_container_direction_set(exp->buddy_cont, CONTAINER_DIRECTION_HORIZONTAL);
  esmart_container_fill_policy_set(exp->buddy_cont, CONTAINER_FILL_POLICY_FILL_Y | CONTAINER_FILL_POLICY_KEEP_ASPECT);
  esmart_container_move_button_set(exp->buddy_cont, 2);
  esmart_container_spacing_set(exp->buddy_cont, 5);
  edje_object_part_swallow(exp->gui, "express.buddylist", exp->buddy_cont);
  evas_object_name_set(exp->buddy_cont, "express.buddylist");
  evas_object_show(exp->buddy_cont);

  edje_object_size_min_get(exp->gui, &mw, &mh);
  tmp = edje_object_data_get(exp->gui, "default.w");
  if (tmp) dw = atoi(tmp);
  tmp = edje_object_data_get(exp->gui, "default.h");
  if (tmp) dh = atoi(tmp);

  ecore_evas_size_min_set(exp->ee, mw, mh);

  if(dw > 0 && dh > 0)
  {
    evas_object_resize(exp->gui, dw, dh);
    ecore_evas_resize(exp->ee, dw, dh);
  }
  else if (mw > 0 && mh > 0)
  {
    evas_object_resize(exp->gui, mw, mh);
    ecore_evas_resize(exp->ee, mw, mh);
  }
  else
  {
    evas_object_resize(exp->gui, 255, 255);
    ecore_evas_resize(exp->ee, 255, 255);
  }

  evas_object_show(exp->gui);

  ecore_evas_callback_resize_set(exp->ee, exp_gui_cb_resize);
  ecore_idle_enterer_add(exp_gui_idler_before, exp);

  edje_color_class_set("express.message.received", 255, 214, 214, 255, 255, 214, 214, 255, 255, 214, 214, 255);
  edje_color_class_set("express.message.sent", 229, 239, 255, 255, 229, 239, 255, 255, 229, 239, 255, 255);

  return 1;
}


static void
exp_gui_cb_resize(Ecore_Evas *ee)
{
  Exp *exp;
  Evas_Coord w, h;

  exp = ecore_evas_data_get(ee, "exp");
  if (!exp)
  {
    printf("Exp: main struct does not exist!.\n");
    return;
  }
  ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
  if (exp->gui)  evas_object_resize(exp->gui, w, h);
  exp->changes.size = 1;
}

static int
exp_gui_idler_before(void *data)
{
  /* all real work gets done from here */
  Exp *exp = data;
  Evas_List *l, *removals = NULL;

  if (!exp) return 1;

  /* remove conversations that have destroy set */
  for (l = exp->conversations; l; l = l->next)
  {
    Exp_Conversation *conv = l->data;
    if (conv->destroy)
    {
      removals = evas_list_append(removals, conv);

      if (conv == exp->active_conversation)
      {
        edje_object_part_unswallow(exp->gui, exp->active_conversation->cont);
        evas_object_hide(exp->active_conversation->cont);
        exp->active_conversation->buddy->active = 0;
        exp->active_conversation->buddy->changed = 1;
        exp->active_conversation->buddy->changes.state = 1;
        exp->active_conversation = NULL;
        exp->changes.active_conv = 1;
      }
    }
  }
  while(removals)
  {
    Exp_Conversation *conv = removals->data;
    exp->conversations = evas_list_remove(exp->conversations, conv);
    exp_conversation_free(conv);
    removals = evas_list_remove(removals, conv);
  }


  /* update all remaining conversations */
  for (l = exp->conversations; l; l = l->next)
  {
    Exp_Conversation *conv = l->data;
    if (exp->changes.size)
    {
      conv->changed = 1;
      conv->changes.size = 1;
    }
    if (conv->changed)
    {
      printf("idler up conv\n");
      exp_conversation_update(conv);
    }
  }
  exp->changes.size = 0;

  /* update buddy icons */
  for (l = exp->buddies; l; l = l->next)
  {
    Exp_Buddy *buddy = l->data;
    if (buddy->changed)
    {
      printf("idler up buddy\n");
      exp_buddy_update(buddy);
    }
  }

  /* hide previous and show new active conversation */
  if (exp->changes.active_conv)
  {
    int found = 0;

    if (exp->active_conversation)
    {
      printf("idler chg active conv\n");
      edje_object_part_unswallow(exp->gui, exp->active_conversation->cont);
      evas_object_hide(exp->active_conversation->cont);
      exp->active_conversation->buddy->active = 0;
      exp->active_conversation->buddy->changed = 1;
      exp->active_conversation->buddy->changes.state = 1;
      exp->active_conversation = NULL;
    }

    for (l = exp->conversations; l; l = l->next)
    {
      Exp_Conversation *conv = l->data;

      if (conv->active)
      {
        edje_object_part_swallow(exp->gui, "express.conversation", conv->cont);
        evas_object_show(conv->cont);

        conv->changed = 1;
        conv->buddy->active = 1;
        conv->buddy->changed = 1;
        conv->buddy->changes.state = 1;
        exp->active_conversation = conv;
        found = 1;
        break;
      }
    }
    if (!found)
    {
      /* show a different conversation? */
    }

    exp->changes.active_conv = 0;
  }

  return 1;
}

static void
_exp_gui_scroll_callback(void *data, Evas_Object *obj, const char *sig, const char *src)
{
  Exp *exp = data;
  double dx, dy;

  if (!exp) return;

  edje_object_part_drag_value_get(exp->gui, "scrollbar.bar", &dx, &dy);
  if (exp->active_conversation)
  {
    exp_conversation_pos_set(exp->active_conversation, dy);
  }
  
  return;
  obj = NULL;
  sig = NULL;
  src = NULL;
}
