/*
 * vim:ts=4:cino=t0
 */

#include "Express.h"

static void _exp_message_realize(Exp_Message *msg);
/*static void _exp_message_intercept_resize(void *data, Evas_Object *o, Evas_Coord w, Evas_Coord h);
*/

Exp_Message *
exp_message_new(Exp_Conversation *conv, char *text, int sender)
{
  Exp_Message *msg; 

  if (!conv) return NULL;

  msg = (Exp_Message *)calloc(1, sizeof(Exp_Message));
  msg->sender = sender;
  msg->msg_text = strdup(text);
  msg->conv = conv;
  conv->messages = evas_list_append(conv->messages, msg);
  conv->new_message = 1;
  conv->changed = 1;
  msg->changed = 1;
  msg->changes.size = 1;

  return msg;
}

void
exp_message_free(Exp_Message *msg)
{
  if(!msg) return;
  
  evas_object_del(msg->icon);
  evas_object_del(msg->text);
  evas_object_del(msg->gui);

  if (msg->msg_text) free(msg->msg_text);
  free(msg);
}


void
exp_message_update(Exp_Message *msg)
{
  if (!msg) return;
  Evas_Coord tw, th, aw, ah, mw, mh;

  printf("update msg\n");
  if (!msg->realized) _exp_message_realize(msg);

  if (msg->changes.size)
  {
    evas_object_geometry_get(msg->gui, NULL, NULL, &mw, &mh);
/*    etox_text_geometry_get(msg->text, &tw, &th); */
//    evas_object_geometry_get(msg->text, NULL, NULL, &tw, &th);
    evas_object_textblock_native_size_get(msg->text, &tw, &th);
    printf("native: %f, %f\n", (double)tw, (double)th);
    edje_object_part_geometry_get(msg->gui, "express.message.text",
                                  NULL, NULL, &aw, &ah);
  if (th != ah)
    evas_object_resize(msg->gui, mw, mh - ah + th);
  
    msg->changes.size = 0;
  }

  msg->changed = 0;
}

static void
_exp_message_realize(Exp_Message *msg)
{
  Evas *evas;
  /*Etox_Context *ctx;*/
  char *theme_path;
  
  if (!msg) return;
  if (msg->realized) return;
  if (!msg->conv || !msg->conv->exp) return;
 
  printf("realize message\n");
  evas = msg->conv->exp->evas; 
  theme_path = msg->conv->exp->theme_path;

  msg->gui = edje_object_add(evas);
  edje_object_file_set(msg->gui, theme_path, "express.message");
  evas_object_resize(msg->gui, 100, 100);
  evas_object_show(msg->gui);

#if 0
  msg->text = etox_new(evas);
  ctx = etox_get_context(msg->text);
  etox_context_set_color(ctx, 50, 50, 50, 255);
  etox_context_set_font(ctx, "Vera", 10);
  etox_context_set_wrap_marker(ctx, "", "");
  etox_set_context(msg->text, ctx);
  etox_set_soft_wrap(msg->text, 1);
  etox_set_word_wrap(msg->text, 1);
  etox_set_text(msg->text, msg->msg_text);
  evas_object_resize(msg->text, 100, 100);
  evas_object_show(msg->text);
  edje_object_part_swallow(msg->gui, "express.message.text", msg->text);
#endif

  msg->text = evas_object_textblock_add(evas);
  evas_object_textblock_format_insert(msg->text, "font=/usr/local/share/evas/data/Vera.ttf size=10 color=#222222");
  evas_object_textblock_text_insert(msg->text, msg->msg_text); 
  evas_object_resize(msg->text, 100, 100);
  evas_object_show(msg->text);
  edje_object_part_swallow(msg->gui, "express.message.text", msg->text);

  if (msg->sender)
  {
    msg->icon = exp_buddy_icon_new(msg->conv->buddy);
    edje_object_signal_emit(msg->gui, "sender", "buddy");
  }
  else
  {
    msg->icon = exp_buddy_icon_new(msg->conv->user);
    edje_object_signal_emit(msg->gui, "sender", "user");
  }

  edje_object_part_swallow(msg->gui, "express.message.icon", msg->icon);

  esmart_container_element_append(msg->conv->cont, msg->gui);

  msg->realized = 1;
}

