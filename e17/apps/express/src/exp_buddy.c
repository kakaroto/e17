/*
 * vim:ts=4:cino=t0
 */

#include "Express.h"

static void _exp_buddy_realize(Exp_Buddy *buddy);
static void _exp_buddy_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);

Exp_Buddy *exp_buddy_new(Exp *exp)
{
  Exp_Buddy *buddy = calloc(1, sizeof(Exp_Buddy));

  exp->buddies = evas_list_append(exp->buddies, buddy);
  buddy->exp = exp;
  buddy->changed = 1;
  return buddy;
}

void exp_buddy_free(Exp_Buddy *buddy)
{
  evas_object_del(buddy->icon);
  evas_object_del(buddy->gui);

  if (buddy->name) free(buddy->name);
  if (buddy->username) free(buddy->username);
  if (buddy->icon_data) free(buddy->icon_data);
 
  free(buddy);
}

void exp_buddy_update(Exp_Buddy *buddy)
{
  if (!buddy) return;
  printf ("buddy update\n");
  if (!buddy->realized)
  {
    _exp_buddy_realize(buddy);
    buddy->changes.icon = 0; /* _realize creates the icon using the data... */
  }

  if (buddy->changes.icon && buddy->icon)
  {
    evas_object_image_alpha_set(buddy->icon, 1);
    evas_object_image_size_set(buddy->icon, buddy->icon_w, buddy->icon_h);
    evas_object_image_data_copy_set(buddy->icon, buddy->icon_data);
    buddy->changes.icon = 0;
  }

  if (buddy->changes.name)
  {
    edje_object_part_text_set(buddy->gui, "express.buddy.name", buddy->name);
    buddy->changes.name = 0;
  }

  if (buddy->changes.username)
  {
    /* FIXME handle */
    buddy->changes.username = 0;
  }

  if (buddy->changes.state)
  {
    if (buddy->active)
      edje_object_signal_emit(buddy->gui, "active", "");
    else if (buddy->new_message)
      edje_object_signal_emit(buddy->gui, "new_message", "");
    else if (buddy->chatting)
      edje_object_signal_emit(buddy->gui, "chatting", "");
    else
      edje_object_signal_emit(buddy->gui, "unactive", "");
   
    buddy->new_message = 0;
    buddy->changes.state = 0;
  }

  buddy->changed = 0;
}

void
exp_buddy_name_set(Exp_Buddy *buddy, char *name)
{
  if (!buddy) return;
  
  if (buddy->name) free(name);
  buddy->name = strdup(name);

  buddy->changed = 1;
  buddy->changes.name = 1;
}
 
const char *
exp_buddy_name_get(Exp_Buddy *buddy)
{
  if (!buddy) return NULL;
  return buddy->name;
}
 
void
exp_buddy_username_set(Exp_Buddy *buddy, char *username)
{
  if (!buddy) return;
  
  if (buddy->username) free(username);
  buddy->username = strdup(username);

  buddy->changed = 1;
  buddy->changes.username = 1;
}

const char *
exp_buddy_username_get(Exp_Buddy *buddy)
{
  if (!buddy) return NULL;
  return buddy->username;
}

void
exp_buddy_icon_data_set(Exp_Buddy *buddy, void *icon_data, int w, int h)
{
  int size;
  
  if (!buddy) return;

  if (!icon_data) return;
  
  if (buddy->icon_data) free(buddy->icon_data);

  size = w * h * sizeof(unsigned int);
  buddy->icon_data = malloc(size);
  memcpy (buddy->icon_data, icon_data, size);
  buddy->icon_w = w;
  buddy->icon_h = h;

  buddy->changed = 1;
  buddy->changes.icon = 1;
}

Evas_Object *
exp_buddy_icon_new(Exp_Buddy *buddy)
{
  Evas_Object *obj;
  
  if (!buddy) return NULL;
  if (!buddy->exp) return NULL;

  printf("buddy icon new\n");
  obj = evas_object_image_add(buddy->exp->evas);
  if (buddy->icon_data && buddy->icon_w > 0 && buddy->icon_h > 0)
  {
    evas_object_image_alpha_set(obj, 1);
    evas_object_image_size_set(obj, buddy->icon_w, buddy->icon_h);
    evas_object_image_data_copy_set(obj, buddy->icon_data);
  }
  else
  {
    /* FIXME use a sane default here */
    printf("using gryffon\n");
    evas_object_image_file_set(obj, "../data/gryffon.png", "");
    /* FIXME set image_data also */
  }

  return obj;
}

void
exp_buddy_show_name_set(Exp_Buddy *buddy, int show)
{
  if (!buddy) return;
  
  if (buddy->show_name == show) return;

  buddy->show_name = show;
  buddy->changed = 1;
}

void
exp_buddy_activate(Exp_Buddy *buddy)
{
  Exp_Conversation *conv;
  if (!buddy) return;

  conv = exp_conversation_find_by_buddy(buddy);
  if (!conv) conv = exp_conversation_new(buddy);
  if (!conv) return;
  exp_conversation_activate(conv);
}

void
exp_buddy_deactivate(Exp_Buddy *buddy)
{
  Exp_Conversation *conv;
  if (!buddy) return;

  conv = exp_conversation_find_by_buddy(buddy);
  if (!conv) return;

  exp_conversation_destroy(conv);
  buddy->active = 0;
  buddy->chatting = 0;
  buddy->new_message = 0;
  buddy->changed = 1;
  buddy->changes.state = 1;
}

static void
_exp_buddy_realize(Exp_Buddy *buddy)
{
  Evas_Coord dw, dh, mw, mh;
  const char *tmp;

  if (!buddy) return;
  if (!buddy->exp) return;
 
  printf("buddy realize!\n");
  buddy->gui = edje_object_add(buddy->exp->evas);
  edje_object_file_set(buddy->gui, buddy->exp->theme_path, "express.buddy");
  evas_object_show(buddy->gui);
  
  edje_object_size_min_get(buddy->gui, &mw, &mh);
  tmp = edje_object_data_get(buddy->gui, "default.w");
  if (tmp) dw = atoi(tmp);
  tmp = edje_object_data_get(buddy->gui, "default.h");
  if (tmp) dh = atoi(tmp);

  if(dw > 0 && dh > 0)
    evas_object_resize(buddy->gui, dw, dh);
  else if (mw > 0 && mh > 0)
    evas_object_resize(buddy->gui, mw, mh);
  else
    evas_object_resize(buddy->gui, 50, 50); /* arbitrary */

  printf("dw, dh: %d, %d\n", dw, dh);


  buddy->icon = exp_buddy_icon_new(buddy);
  edje_object_part_swallow(buddy->gui, "express.buddy.icon", buddy->icon);
  evas_object_show(buddy->icon);


  if (buddy->name) 
    edje_object_part_text_set(buddy->icon, "express.buddy.name", buddy->name);

  esmart_container_element_append(buddy->exp->buddy_cont, buddy->gui);

  /* add callbacks */
  evas_object_event_callback_add(buddy->gui, EVAS_CALLBACK_MOUSE_DOWN,
                                 _exp_buddy_cb_mouse_down, buddy);
 
  buddy->realized = 1;
}

static void
_exp_buddy_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
  Evas_Event_Mouse_Down *ev = event_info;
  Exp_Buddy *buddy = data;

  if (!buddy) return;

  if (ev->button == 1)
  {
    exp_buddy_activate(buddy);
  }
  else if (ev->button == 3)
  {
    exp_buddy_deactivate(buddy);
  }

  return;
  e = NULL;
  obj = NULL;
}

