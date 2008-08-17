/*
 * vim:ts=4:cino=t0
 */

#include "Express.h"

static int exp_fake_conversation(void *data);
static int exp_fake_conversation2(void *data);
static void _exp_conversation_realize(Exp_Conversation *conv);
static void _exp_conversation_intercept_resize(void *data, Evas_Object *obj, Evas_Coord w, Evas_Coord h);

Exp_Conversation *
exp_conversation_new(Exp_Buddy *buddy)
{
  Exp_Conversation *conv;

  if (!buddy || !buddy->exp) return NULL;

  conv = (Exp_Conversation *)calloc(1, sizeof(Exp_Conversation));
  conv->exp = buddy->exp;
  conv->buddy = buddy;
  conv->user = buddy->exp->user;
  conv->changed = 1;
  conv->changes.size = 1;

  buddy->chatting = 1;
  buddy->changed = 1;
  buddy->changes.state = 1;

  buddy->exp->conversations = evas_list_append(buddy->exp->conversations, conv);

  printf("new conv: (%s)\n", buddy->name);
  if (buddy->name && !strcmp(buddy->name, "tokyo"))
    ecore_timer_add(1.5, exp_fake_conversation, buddy);
  else if (buddy->name && !strcmp(buddy->name, "rephorm"))
    ecore_timer_add(1.5, exp_fake_conversation2, buddy);
  return conv; 
}

void
exp_conversation_free(Exp_Conversation *conv)
{
  if (!conv) return;

  while (conv->messages)
  {
    Exp_Message *msg = conv->messages->data;
    conv->messages = evas_list_remove(conv->messages, msg);
    exp_message_free(msg);
  }

  free(conv);
}

void
exp_conversation_update(Exp_Conversation *conv)
{
  Evas_List *l;

  printf("update conv!\n");
  if (!conv->realized) _exp_conversation_realize(conv);

  if (conv->changes.size || conv->changes.active) conv->changes.pos = 1;

  for (l = conv->messages; l; l = l->next)
  {
    Exp_Message *msg = l->data;

    if (conv->changes.size)
    {
      msg->changed = 1;
      msg->changes.size = 1;
    }
    if (msg->changed)
      exp_message_update(msg);
  }

  if (conv->new_message)
  {
    exp_conversation_pos_set(conv, 1.0);
    conv->buddy->new_message = 1;
    conv->buddy->changed = 1;
    conv->buddy->changes.state = 1;

    conv->new_message = 0;
  }

  if (conv->changes.pos)
  {
    esmart_container_scroll_percent_set(conv->cont, conv->pos);
    edje_object_part_drag_value_set(conv->exp->gui, "scrollbar.bar", conv->pos, conv->pos);
    conv->changes.pos = 0;
  }

  conv->changes.active = 0;
  conv->changes.size = 0;

  conv->changed = 0;
}

void
exp_conversation_activate(Exp_Conversation *conv)
{
  Evas_List *l;
  if (!conv || !conv->exp) return;
  if (conv->active) return;

  printf("activate! (%s)\n", conv->buddy->name);

  for (l = conv->exp->conversations; l; l = l->next)
  {
    Exp_Conversation *c = l->data;
    c->active = 0;
  }
  conv->active = 1;
  conv->exp->changes.active_conv = 1;
  conv->buddy->active = 1;
  conv->buddy->changed = 1;
  conv->buddy->changes.state = 1;
}

void
exp_conversation_destroy(Exp_Conversation *conv)
{
  if (!conv) return;

  conv->active = 0;
  conv->destroy = 1;
  conv->changed = 1;
}

Exp_Conversation *
exp_conversation_find_by_buddy(Exp_Buddy *buddy)
{
  Evas_List *l;
  if (!buddy || !buddy->exp) return NULL;
  
  for (l = buddy->exp->conversations; l; l = l->next)
  {
    Exp_Conversation *conv = l->data;
    if (conv->buddy == buddy) return conv;
  }
  return NULL;
}

void
exp_conversation_pos_set(Exp_Conversation *conv, double pos)
{
  if (!conv) return;

  if (pos < 0) pos = 0;
  else if (pos > 1) pos = 1;

  conv->pos = pos;
  conv->changed = 1;
  conv->changes.pos = 1;
}

static void
_exp_conversation_realize(Exp_Conversation *conv)
{
  if (!conv || !conv->exp) return;

  printf("realize conv!\n");

  /* FIXME make direction configurable */
  conv->cont = esmart_container_new(conv->exp->evas);
  esmart_container_direction_set(conv->cont, CONTAINER_DIRECTION_VERTICAL);
  esmart_container_fill_policy_set(conv->cont, CONTAINER_FILL_POLICY_FILL_X);
  esmart_container_spacing_set(conv->cont, -30);
  evas_object_intercept_resize_callback_add(conv->cont, _exp_conversation_intercept_resize, conv);
 
  conv->realized = 1;
}

static int
exp_fake_conversation(void *data)
{
  Exp_Buddy *buddy = data;
  Exp_Conversation *conv;
  static int i = 0;
  
  char *msgs[7] = {
    "Hey!",
    "Yo, what's up?",
    "not much, just playing around with exp a bit",
    "oh yeah? how's that going?",
    "pretty well... gettin a fake conversation going now",
    "awesome",
    "how about you?"
  };

  conv = exp_conversation_find_by_buddy(buddy);
  if (!conv) return 0;
  printf("fake it!\n");
  exp_message_new(conv, msgs[i], i%2);
  i++;
  if (i < 7) return 1;
  {
    i = 0;
    return 0;
  }
}


static int
exp_fake_conversation2(void *data)
{
  Exp_Buddy *buddy = data;
  Exp_Conversation *conv;
  static int i = 0;
  
  char *msgs[7] = {
    "Hey!",
    "Yo, what's up?",
    "not much, just playing around with exp a bit",
    "oh yeah? how's that going?",
    "pretty well... gettin a fake conversation going now",
    "awesome",
    "how about you?"
  };

  conv = exp_conversation_find_by_buddy(buddy);
  if (!conv) return 0;

  exp_message_new(conv, msgs[i], i%2);
  i++;
  if (i < 7) return 1;
  else 
  {
    i = 0;
    return 0;
  }
}

static void
_exp_conversation_intercept_resize(void *data, Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
  Exp_Conversation *conv = data;

  conv->changed = 1;
  conv->changes.size = 1;

  evas_object_resize(obj, w, h);
}
