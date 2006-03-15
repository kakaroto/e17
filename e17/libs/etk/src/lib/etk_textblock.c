/** @file etk_textblock.c */
#include "etk_textblock.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Evas_Engine_Buffer.h>
#include "etk_utils.h"

#define ETK_TEXTBLOCK_EVAS_SIZE 32

static void _etk_textblock_constructor(Etk_Textblock *textblock);
static void _etk_textblock_destructor(Etk_Textblock *textblock);

static void _etk_textblock_iter_goto_prev_char_full(Etk_Textblock_Iter *iter, Etk_Bool include_new_line);
static void _etk_textblock_iter_goto_next_char_full(Etk_Textblock_Iter *iter, Etk_Bool include_new_line);
static void _etk_textblock_cursor_object_update(Etk_Textblock *textblock);
static void _etk_textblock_object_evas_set(Etk_Textblock *textblock, Evas *new_evas);

static void _etk_textblock_smart_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_textblock_smart_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_textblock_smart_show_cb(Evas_Object *obj);
static void _etk_textblock_smart_hide_cb(Evas_Object *obj);
static void _etk_textblock_smart_clip_set_cb(Evas_Object *obj, Evas_Object *clip);
static void _etk_textblock_smart_clip_unset_cb(Evas_Object *obj);

static Evas *_etk_textblock_evas = NULL;
static unsigned char _etk_textblock_pixel_buffer[ETK_TEXTBLOCK_EVAS_SIZE * ETK_TEXTBLOCK_EVAS_SIZE * 3];
static Evas_Textblock_Style *_etk_textblock_style = NULL;
static Evas_Smart *_etk_textblock_smart = NULL;
static int _etk_textblock_count = 0;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Textblock
 * @return Returns the type on an Etk_Textblock
 */
Etk_Type *etk_textblock_type_get()
{
   static Etk_Type *textblock_type = NULL;

   if (!textblock_type)
      textblock_type = etk_type_new("Etk_Textblock", ETK_OBJECT_TYPE, sizeof(Etk_Textblock), ETK_CONSTRUCTOR(_etk_textblock_constructor), ETK_DESTRUCTOR(_etk_textblock_destructor));

   return textblock_type;
}

/**
 * @brief Creates a new textblock
 * @return Returns the new textblock
 */
Etk_Textblock *etk_textblock_new()
{
   return ETK_TEXTBLOCK(etk_object_new(ETK_TEXTBLOCK_TYPE, NULL));
}

/* TODO: doc */
void etk_textblock_realize(Etk_Textblock *textblock, Evas *evas)
{
   if (!textblock || !evas)
      return;
   
   _etk_textblock_object_evas_set(textblock, evas);
   
   if (!_etk_textblock_smart)
   {
      _etk_textblock_smart = evas_smart_new(
         "etk_textblock_smart_object",
         NULL, /* add */
         NULL, /* del */
         NULL, /* layer_set */
         NULL, /* raise */
         NULL, /* lower */
         NULL, /* stack_above */
         NULL, /* stack_below */
         _etk_textblock_smart_move_cb, /* move */
         _etk_textblock_smart_resize_cb, /* resize */
         _etk_textblock_smart_show_cb, /* show */
         _etk_textblock_smart_hide_cb, /* hide */
         NULL, /* color_set */
         _etk_textblock_smart_clip_set_cb, /* clip_set */
         _etk_textblock_smart_clip_unset_cb, /* clip_unset */
         NULL); /* data*/
   }
   textblock->smart_object = evas_object_smart_add(evas, _etk_textblock_smart);
   evas_object_smart_data_set(textblock->smart_object, textblock);
   evas_object_smart_member_add(textblock->textblock_object, textblock->smart_object);
   
   /*textblock->clip = evas_object_rectangle_add(evas);
   evas_object_color_set(textblock->clip, 255, 128, 0, 80);
   evas_object_smart_member_add(textblock->clip, textblock->smart_object);
   evas_object_lower(textblock->clip);*/
   /* TODO: realize: creates selection rects, clip, cursor, timer  */
   
   textblock->cursor_object = evas_object_rectangle_add(evas);
   /* TODO: theme */
   evas_object_color_set(textblock->cursor_object, 0, 0, 0, 255);
   evas_object_smart_member_add(textblock->cursor_object, textblock->smart_object);
   _etk_textblock_cursor_object_update(textblock);
}

/* TODO: doc */
void etk_textblock_unrealize(Etk_Textblock *textblock)
{
   if (!textblock)
      return;
   
   if (textblock->cursor_timer)
   {
      ecore_timer_del(textblock->cursor_timer);
      textblock->cursor_timer = NULL;
   }
   
   while (textblock->selection_rects)
   {
      evas_object_del(textblock->selection_rects->data);
      textblock->selection_rects = evas_list_remove_list(textblock->selection_rects, textblock->selection_rects);
   }
   
   if (textblock->cursor_object)
   {
      evas_object_del(textblock->cursor_object);
      textblock->cursor_object = NULL;
   }
   if (textblock->clip)
   {
      evas_object_del(textblock->clip);
      textblock->clip = NULL;
   }
   if (textblock->smart_object)
   {
      if (textblock->textblock_object)
         evas_object_smart_member_del(textblock->textblock_object);
      evas_object_del(textblock->smart_object);
      textblock->smart_object = NULL;
   }
   
   _etk_textblock_object_evas_set(textblock, _etk_textblock_evas);
}

/* TODO: doc */
void etk_textblock_text_set(Etk_Textblock *textblock, const char *text)
{
   Evas_List *l;
   Etk_Textblock_Iter *iter;
   
   if (!textblock)
      return;
   
   evas_object_textblock_text_markup_set(textblock->textblock_object, text ? text : "");
   for (l = textblock->iterators; l; l = l->next)
   {
      iter = l->data;
      etk_textblock_iter_goto_start(iter);
   }
}

/* TODO: doc */
Etk_Textblock_Iter *etk_textblock_iter_new(Etk_Textblock *textblock)
{
   Etk_Textblock_Iter *new_iter;
   
   if (!textblock)
      return NULL;
   
   new_iter = malloc(sizeof(Etk_Textblock_Iter));
   new_iter->cursor = evas_object_textblock_cursor_new(textblock->textblock_object);
   new_iter->textblock = textblock;
   
   textblock->iterators = evas_list_append(textblock->iterators, new_iter);
   etk_textblock_iter_goto_start(new_iter);
   
   return new_iter;
}

/* TODO: doc */
void etk_textblock_iter_free(Etk_Textblock_Iter *iter)
{
   if (!iter)
      return;
   
   evas_textblock_cursor_free(iter->cursor);
   if (iter->textblock)
      iter->textblock->iterators = evas_list_remove(iter->textblock->iterators, iter);
   free(iter);
}

/* TODO: doc */
void etk_textblock_iter_copy(Etk_Textblock_Iter *iter, Etk_Textblock_Iter *dest_iter)
{
   if (!iter || !dest_iter || iter->textblock != dest_iter->textblock)
      return;
   evas_textblock_cursor_copy(iter->cursor, dest_iter->cursor);
}

/* TODO: doc */
void etk_textblock_iter_goto_start(Etk_Textblock_Iter *iter)
{
   if (!iter || !iter->textblock || !iter->cursor)
      return;
   
   evas_textblock_cursor_node_first(iter->cursor);
   while (!evas_textblock_cursor_node_text_get(iter->cursor) && evas_textblock_cursor_node_next(iter->cursor));
   evas_textblock_cursor_char_first(iter->cursor);
   
   /* TODO */
   _etk_textblock_cursor_object_update(iter->textblock);
}

/* TODO: doc */
void etk_textblock_iter_goto_end(Etk_Textblock_Iter *iter)
{
   if (!iter || !iter->textblock || !iter->cursor)
      return;
   
   evas_textblock_cursor_node_last(iter->cursor);
   while (!evas_textblock_cursor_node_text_get(iter->cursor) && evas_textblock_cursor_node_prev(iter->cursor));
   evas_textblock_cursor_char_last(iter->cursor);
   
   /* TODO */
   _etk_textblock_cursor_object_update(iter->textblock);
}

/* TODO: doc */
void etk_textblock_iter_goto_prev_char(Etk_Textblock_Iter *iter)
{
   _etk_textblock_iter_goto_prev_char_full(iter, ETK_TRUE);
}

/* TODO: doc */
void etk_textblock_iter_goto_next_char(Etk_Textblock_Iter *iter)
{
   _etk_textblock_iter_goto_next_char_full(iter, ETK_TRUE);
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the textblock */
static void _etk_textblock_constructor(Etk_Textblock *textblock)
{
   if (!textblock)
      return;
   
   textblock->smart_object = NULL;
   textblock->textblock_object = NULL;
   textblock->cursor_object = NULL;
   textblock->clip = NULL;
   textblock->selection_rects = NULL;
   textblock->cursor = NULL;
   textblock->selection_start = NULL;
   textblock->iterators = NULL;
   textblock->cursor_timer = NULL;
   _etk_textblock_count++;
   
   if (!_etk_textblock_evas)
   {
      Evas_Engine_Info_Buffer *engine_info;
      int render_method;
      
      if (!(render_method = evas_render_method_lookup("buffer")))
      {
         ETK_WARNING("Unable to use the buffer engine of Evas. Unable to create the textblock");
         return;
      }
      
      _etk_textblock_evas = evas_new();
      evas_output_method_set(_etk_textblock_evas, render_method);
      evas_output_size_set(_etk_textblock_evas, ETK_TEXTBLOCK_EVAS_SIZE, ETK_TEXTBLOCK_EVAS_SIZE);
      evas_output_viewport_set(_etk_textblock_evas, 0, 0, ETK_TEXTBLOCK_EVAS_SIZE, ETK_TEXTBLOCK_EVAS_SIZE);
      
      if (!(engine_info = (Evas_Engine_Info_Buffer *)evas_engine_info_get(_etk_textblock_evas)))
      {
         ETK_WARNING("Unable to get the info of the buffer engine of Evas. Unable to create the textblock");
         evas_free(_etk_textblock_evas);
         _etk_textblock_evas = NULL;
         return;
      }
      
      engine_info->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_RGB24;
      engine_info->info.dest_buffer = _etk_textblock_pixel_buffer;
      engine_info->info.dest_buffer_row_bytes = ETK_TEXTBLOCK_EVAS_SIZE * 3;
      engine_info->info.use_color_key = 0;
      engine_info->info.alpha_threshold = 0;
      engine_info->info.func.new_update_region = NULL;
      engine_info->info.func.free_update_region = NULL;
      evas_engine_info_set(_etk_textblock_evas, (Evas_Engine_Info *)engine_info);
   }
   
   if (!_etk_textblock_style)
   {
      _etk_textblock_style = evas_textblock_style_new();
      evas_textblock_style_set(_etk_textblock_style,
         "DEFAULT='font=Vera font_size=8 align=left color=#000000 wrap=word'"
         "center='+ font=Vera font_size=10 align=center'"
         "/center='- \n'"
         "right='+ font=Vera font_size=10 align=right'"
         "/right='- \n'"
         "blockquote='+ left_margin=+24 right_margin=+24 font=Vera font_size=10 align=left'"
         "h1='+ font_size=20'"
         "b='+font=Vera-Bold'"
         "i='+font=Vera-Italic'"
         "glow='+ style=glow color=#fff glow2_color=#fe87 glow_color=#fa14'"
         "link='+ underline=on underline_color=#0000aa color=#0000aa'"		       
         "red='+ color=#ff0000'"
         "p='+ font=Vera font_size=10 align=left'"
         "/p='- \n'"
         "br='\n'"
         "tab='\t'");
   }
   
   textblock->textblock_object = evas_object_textblock_add(_etk_textblock_evas);
   evas_object_textblock_style_set(textblock->textblock_object, _etk_textblock_style);
   /* TODO: does it need to be shown */
   evas_object_show(textblock->textblock_object);
   
   textblock->cursor = etk_textblock_iter_new(textblock);
   textblock->selection_start = etk_textblock_iter_new(textblock);
}

/* Destroys the textblock */
static void _etk_textblock_destructor(Etk_Textblock *textblock)
{
   if (!textblock)
      return;
   
   if (textblock->cursor_timer)
      ecore_timer_del(textblock->cursor_timer);
   
   while (textblock->selection_rects)
   {
      evas_object_del(textblock->selection_rects->data);
      textblock->selection_rects = evas_list_remove_list(textblock->selection_rects, textblock->selection_rects);
   }
   
   if (textblock->cursor_object)
      evas_object_del(textblock->cursor_object);
   if (textblock->clip)
      evas_object_del(textblock->clip);
   if (textblock->textblock_object)
      evas_object_del(textblock->textblock_object);
   if (textblock->smart_object)
      evas_object_del(textblock->smart_object);
   
   _etk_textblock_count--;
   if (_etk_textblock_count <= 0)
   {
      if (_etk_textblock_style)
      {
         evas_textblock_style_free(_etk_textblock_style);
         _etk_textblock_style = NULL;
      }
      if (_etk_textblock_evas)
      {
         evas_free(_etk_textblock_evas);
         _etk_textblock_evas = NULL;
      }
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* TODO: doc */
static void _etk_textblock_iter_goto_prev_char_full(Etk_Textblock_Iter *iter, Etk_Bool include_new_line)
{
   Evas_Textblock_Cursor *tmp;
   const char *format;
   
   if (!iter || !iter->textblock || !iter->cursor)
      return;
   
   if (!evas_textblock_cursor_char_prev(iter->cursor))
   {
      tmp = evas_object_textblock_cursor_new(iter->textblock->textblock_object);
      evas_textblock_cursor_copy(iter->cursor, tmp);
      while (evas_textblock_cursor_node_prev(tmp))
      {
         if (evas_textblock_cursor_node_text_get(tmp))
         {
            evas_textblock_cursor_char_last(tmp);
            evas_textblock_cursor_copy(tmp, iter->cursor);
            break;
         }
         if ((format = evas_textblock_cursor_node_format_get(tmp)))
         {
            if ((*format == '\t') || ((*format == '\n') && include_new_line))
            {
               evas_textblock_cursor_copy(tmp, iter->cursor);
               break;
            }
         }
      }
      evas_textblock_cursor_free(tmp);
   }
   
   /* TODO */
   _etk_textblock_cursor_object_update(iter->textblock);
}

/* TODO: doc */
static void _etk_textblock_iter_goto_next_char_full(Etk_Textblock_Iter *iter, Etk_Bool include_new_line)
{
   Evas_Textblock_Cursor *tmp;
   const char *format;
   
   if (!iter || !iter->textblock || !iter->cursor)
      return;
   
   if (!evas_textblock_cursor_char_next(iter->cursor))
   {
      tmp = evas_object_textblock_cursor_new(iter->textblock->textblock_object);
      evas_textblock_cursor_copy(iter->cursor, tmp);
      while (evas_textblock_cursor_node_next(tmp))
      {
         if (evas_textblock_cursor_node_text_get(tmp))
         {
            evas_textblock_cursor_char_first(tmp);
            evas_textblock_cursor_copy(tmp, iter->cursor);
            break;
         }
         if ((format = evas_textblock_cursor_node_format_get(tmp)))
         {
            if ((*format == '\t') || ((*format == '\n') && include_new_line))
            {
               evas_textblock_cursor_copy(tmp, iter->cursor);
               break;
            }
         }
      }
      evas_textblock_cursor_free(tmp);
   }
   
   /* TODO */
   _etk_textblock_cursor_object_update(iter->textblock);
}

/* Updates the position and the size of the cursor object */
static void _etk_textblock_cursor_object_update(Etk_Textblock *textblock)
{
   Evas_Coord tbx, tby;
   Evas_Coord cx, cy, cw, ch;
   
   if (!textblock || !textblock->cursor || !textblock->cursor->cursor)
      return;
   if (!textblock->textblock_object || !textblock->cursor_object)
      return;
   
   evas_object_geometry_get(textblock->textblock_object, &tbx, &tby, NULL, NULL);
   evas_textblock_cursor_char_geometry_get(textblock->cursor->cursor, &cx, &cy, &cw, &ch);
   evas_object_move(textblock->cursor_object, tbx + cx, tby + cy);
   evas_object_resize(textblock->cursor_object, 1, ch);
}

/* Changes the evas used by the textblock object */
static void _etk_textblock_object_evas_set(Etk_Textblock *textblock, Evas *new_evas)
{
   Evas_Object *new_tbo;
   Evas_Textblock_Cursor *old_cursor, *new_cursor, *cursor;
   const char *format;
   const char *text;
   Evas_List *l, *l2;
   Etk_Textblock_Iter *iter;
   
   if (!textblock || !new_evas)
      return;
   if (new_evas == evas_object_evas_get(textblock->textblock_object))
      return;
   
   new_tbo = evas_object_textblock_add(new_evas);
   evas_object_textblock_style_set(new_tbo, _etk_textblock_style);
   
   for (l = textblock->iterators; l; l = l->next)
   {
      iter = l->data;
      iter->evas_changed = ETK_FALSE;
   }
   
   new_cursor = evas_object_textblock_cursor_new(new_tbo);
   old_cursor = evas_object_textblock_cursor_new(textblock->textblock_object);
   evas_textblock_cursor_node_first(new_cursor);
   evas_textblock_cursor_node_first(old_cursor);
   
   do
   {
      /* Copy the text or the format of the node */
      if ((format = evas_textblock_cursor_node_format_get(old_cursor)) && *format != 0)
         evas_textblock_cursor_format_append(new_cursor, format);
      if ((text = evas_textblock_cursor_node_text_get(old_cursor)) && *text != 0)
         evas_textblock_cursor_text_append(new_cursor, text);
      
      /* Copy the iterators of the node */
      for (l = textblock->iterators; l; l = l->next)
      {
         iter = l->data;
         if (iter->evas_changed)
            continue;
         
         evas_textblock_cursor_char_first(old_cursor);
         if (evas_textblock_cursor_compare(old_cursor, iter->cursor) <= 0)
         {
            evas_textblock_cursor_char_last(old_cursor);
            if (evas_textblock_cursor_compare(old_cursor, iter->cursor) >= 0)
            {
               cursor = iter->cursor;
               iter->cursor = evas_object_textblock_cursor_new(new_tbo);
               evas_textblock_cursor_copy(new_cursor, iter->cursor);
               evas_textblock_cursor_pos_set(iter->cursor, evas_textblock_cursor_pos_get(cursor));
               evas_textblock_cursor_free(cursor);
               iter->evas_changed = ETK_TRUE;
            }
         }
      }
   }
   while (evas_textblock_cursor_node_next(old_cursor));
   
   if (!textblock->cursor->evas_changed)
   {
      ETK_WARNING("The cursor iterator has been lost during the realisation of the textblock");
      textblock->cursor->cursor = evas_object_textblock_cursor_new(new_tbo);
      /* TODO: move to first text node */
      evas_textblock_cursor_node_first(textblock->cursor->cursor);
      textblock->cursor->evas_changed = ETK_TRUE;
   }
   if (!textblock->selection_start->evas_changed)
   {
      ETK_WARNING("The selection_start iterator has been lost during the realisation of the textblock");
      textblock->selection_start->cursor = evas_object_textblock_cursor_new(new_tbo);
      evas_textblock_cursor_copy(textblock->cursor->cursor, textblock->selection_start->cursor);
      textblock->selection_start->evas_changed = ETK_TRUE;
   }
   
   for (l = textblock->iterators; l; l = l2)
   {
      l2 = l->next;
      iter = l->data;
      /* Shouldn't happen: we can't loose iterators! */
      if (!iter->evas_changed)
      {
         ETK_WARNING("An iterator has been lost during the realisation of the textblock");
         evas_textblock_cursor_free(iter->cursor);
         free(iter);
         textblock->iterators = evas_list_remove_list(textblock->iterators, l);
      }
   }
   
   evas_textblock_cursor_free(new_cursor);
   evas_textblock_cursor_free(old_cursor);
   evas_object_del(textblock->textblock_object);
   textblock->textblock_object = new_tbo;
}

/**************************
 *
 * Smart object functions
 *
 **************************/

/* Called when the smart object of the textblock is moved */
static void _etk_textblock_smart_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   /* TODO: smart_move: update selection_rects, cursor_object  */
   if (textblock->textblock_object)
      evas_object_move(textblock->textblock_object, x, y);
   if (textblock->clip)
      evas_object_move(textblock->clip, x, y);
}

/* Called when the smart object of the textblock is resized */
static void _etk_textblock_smart_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   /* TODO: smart_resize: update selection_rects  */
   if (textblock->textblock_object)
      evas_object_resize(textblock->textblock_object, w, h);
   if (textblock->clip)
      evas_object_resize(textblock->clip, w, h);
   _etk_textblock_cursor_object_update(textblock);
}

/* Called when the smart object of the textblock is shown */
static void _etk_textblock_smart_show_cb(Evas_Object *obj)
{
   Evas_List *l;
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   if (textblock->textblock_object)
      evas_object_show(textblock->textblock_object);
   /* TODO: smart_show: restart timer */
   if (textblock->cursor_object)
      evas_object_show(textblock->cursor_object);
   if (textblock->clip)
      evas_object_show(textblock->clip);
   
   for (l = textblock->selection_rects; l; l = l->next)
      evas_object_show(l->data);
}

/* Called when the smart object of the textblock is hidden */
static void _etk_textblock_smart_hide_cb(Evas_Object *obj)
{
   Evas_List *l;
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   if (textblock->textblock_object)
      evas_object_hide(textblock->textblock_object);
   /* TODO: smart_hide: stop timer */
   if (textblock->cursor_object)
      evas_object_hide(textblock->cursor_object);
   if (textblock->clip)
      evas_object_hide(textblock->clip);
   
   for (l = textblock->selection_rects; l; l = l->next)
      evas_object_hide(l->data);
}

/* Called when the smart object of the textblock is clipped */
static void _etk_textblock_smart_clip_set_cb(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Textblock *textblock;
   
   if (!obj || !clip || !(textblock = evas_object_smart_data_get(obj)) || !textblock->clip)
      return;
   evas_object_clip_set(textblock->clip, clip);
}

/* Called when the smart object of the textblock is unclipped */
static void _etk_textblock_smart_clip_unset_cb(Evas_Object *obj)
{
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)) || !textblock->clip)
      return;
   evas_object_clip_unset(textblock->clip);
}
