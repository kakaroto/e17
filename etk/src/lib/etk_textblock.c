/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_textblock.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_textblock.h"

#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Job.h>

#include "etk_string.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Textblock
 * @{
 */

#define ETK_TB_OBJECT_DEFAULT_HEIGHT 300
#define ETK_TB_EMPTY_LINE_WIDTH 3

/* TODO: in the theme */
#define ETK_TB_OBJECT_SHOW_CURSOR_DELAY 0.75
#define ETK_TB_OBJECT_HIDE_CURSOR_DELAY 0.25

#define ETK_TB_TAG_PARAM_IS(param, len) \
   (param_len == (len) && strncasecmp(param_start, (param), (len)) == 0)

#define ETK_TB_TAG_VALUE_IS(value, len) \
   (value_len == (len) && strncasecmp(value_start, (value), (len)) == 0)

typedef struct Etk_Textblock_Object_SD Etk_Textblock_Object_SD;
typedef struct Etk_Textblock_Object_Line Etk_Textblock_Object_Line;

/* The smart data of a textblock object */
struct Etk_Textblock_Object_SD
{
   Etk_Textblock *tb;

   Etk_Textblock_Wrap wrap;
   Evas_Textblock_Style *style;

   Etk_Textblock_Object_Line *lines;
   Etk_Textblock_Object_Line *last_line;
   Etk_Textblock_Object_Line *first_visible_line;
   Etk_Textblock_Object_Line *last_visible_line;

   Etk_Textblock_Iter *cursor;
   Etk_Textblock_Iter *selection;

   Evas_Object *cursor_object;
   Eina_List *selection_rects;
   Evas_Object *clip;
   Evas_Object *bg;

   int xoffset;
   int yoffset;

   Ecore_Timer *cursor_timer;
   Ecore_Job *update_job;

   Etk_Bool cursor_visible:1;
   Etk_Bool selecting:1;
};

/* A line for a textblock object is a text terminated by '\n', <br> or </p>.
 * It can actually fill several "visual" lines because of wrapping */
struct Etk_Textblock_Object_Line
{
   Etk_Textblock_Object_Line *prev;
   Etk_Textblock_Object_Line *next;

   Etk_Textblock_Node *node;

   Etk_Geometry geometry;
   Evas_Object *object;

   Etk_Bool need_geometry_update:1;
   Etk_Bool need_content_update:1;
};

static void _etk_tb_constructor(Etk_Textblock *tb);
static void _etk_tb_destructor(Etk_Textblock *tb);

static void _etk_textblock_node_printf(Etk_Textblock_Node *node, int n_tabs);
static Etk_Textblock_Node *_etk_textblock_node_new(Etk_Textblock_Node *parent, Etk_Textblock_Node *prev, Etk_Textblock_Node_Type node_type, Etk_Textblock_Tag_Type tag_type);
static Etk_Textblock_Node *_etk_textblock_node_free(Etk_Textblock_Node *node);
static void _etk_textblock_node_type_set(Etk_Textblock_Node *node, Etk_Textblock_Node_Type node_type, Etk_Textblock_Tag_Type tag_type);
static void _etk_textblock_node_attach(Etk_Textblock_Node *node, Etk_Textblock_Node *parent, Etk_Textblock_Node *prev);
static void _etk_textblock_node_format_get(Etk_Textblock_Node *node, Etk_Textblock_Format *format);

static void _etk_textblock_nodes_clean(Etk_Textblock *tb, Etk_Textblock_Node *nodes);
static void _etk_textblock_tag_insert(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *tag, int length);
static Etk_Textblock_Node *_etk_textblock_paragraph_add(Etk_Textblock *tb, Etk_Textblock_Iter *iter);
static Etk_Textblock_Node *_etk_textblock_line_add(Etk_Textblock *tb, Etk_Textblock_Iter *iter);
static Etk_Textblock_Node *_etk_textblock_line_del(Etk_Textblock *tb, Etk_Textblock_Node *line_node);
static void _etk_textblock_node_text_get(Etk_Textblock_Node *node, Etk_Bool markup, Etk_String *text, Etk_Textblock_Iter *start_iter, Etk_Textblock_Iter *end_iter);

static void _etk_textblock_node_copy(Etk_Textblock_Node *dest, const Etk_Textblock_Node *src, Etk_Bool copy_text);
static int _etk_textblock_node_compare(Etk_Textblock_Node *node1, Etk_Textblock_Node *node2);
static Etk_Textblock_Node *_etk_textblock_node_split(Etk_Textblock *tb, Etk_Textblock_Node *node, int index, int pos);
static Etk_Bool _etk_textblock_node_close(Etk_Textblock_Iter *iter, Etk_Textblock_Node_Type node_type, Etk_Textblock_Tag_Type tag_type, Etk_Textblock_Node *replace_node);

static Etk_Bool _etk_textblock_iter_is_valid(Etk_Textblock *tb, Etk_Textblock_Iter *iter);
static Etk_Bool _etk_textblock_node_is_default_paragraph(Etk_Textblock_Node *node);
static Etk_Bool _etk_textblock_node_is_empty_line(Etk_Textblock_Node *node, const char **face, int *size);
static Etk_Bool _etk_textblock_node_is_empty(Etk_Textblock_Node *node);

static int _etk_textblock_int_parse(const char *int_string, int length, int error_value);
static float _etk_textblock_float_parse(const char *float_string, int length, float error_value);
static void _etk_textblock_color_parse(const char *color_string, int length, Etk_Color *color);
static const char *_etk_textblock_escape_parse(const char *escape, int len);
static int _etk_textblock_hex_string_get(char ch);

static Etk_Textblock_Node *_etk_textblock_prev_node_get(Etk_Textblock_Node *node);
static Etk_Textblock_Node *_etk_textblock_next_node_get(Etk_Textblock_Node *node);
static Etk_Textblock_Node *_etk_textblock_prev_text_node_get(Etk_Textblock_Node *node, Etk_Bool ignore_empty_lines, Etk_Bool *line_has_changed);
static Etk_Textblock_Node *_etk_textblock_next_text_node_get(Etk_Textblock_Node *node, Etk_Bool ignore_empty_lines, Etk_Bool *line_has_changed);
static Etk_Textblock_Node *_etk_textblock_prev_line_get(Etk_Textblock_Node *line_node);
static Etk_Textblock_Node *_etk_textblock_next_line_get(Etk_Textblock_Node *line_node);

static void _etk_textblock_iter_update(Etk_Textblock_Iter *iter);
static void _etk_textblock_node_update(Etk_Textblock *tb, Etk_Textblock_Node *node);

static void _etk_textblock_object_line_add(Evas_Object *tbo, Etk_Textblock_Node *line_node);
static void _etk_textblock_object_line_del(Evas_Object *tbo, Etk_Textblock_Node *line_node);
static void _etk_textblock_object_line_fill(Evas_Object *tbo, Evas_Textblock_Cursor *cur, Etk_Textblock_Node *node);
static void _etk_textblock_object_line_update_queue(Evas_Object *tbo, Etk_Textblock_Object_Line *line, Etk_Bool content_update, Etk_Bool geometry_update);
static void _etk_textblock_object_line_update(Evas_Object *tbo, Etk_Textblock_Object_Line *line, int y);
static void _etk_textblock_object_cursor_update_queue(Evas_Object *tbo);
static void _etk_textblock_object_cursor_update(Evas_Object *tbo);
static void _etk_textblock_object_update_queue(Evas_Object *tbo);
static void _etk_textblock_object_update(Evas_Object *tbo);
static void _etk_textblock_object_update_job_cb(void *data);
static int _etk_textblock_object_cursor_timer_cb(void *data);

static Etk_Textblock_Object_Line *_etk_textblock_object_line_get_from_node(Evas_Object *tbo, Etk_Textblock_Node *node);
static Evas_Textblock_Cursor *_etk_textblock_object_cursor_get_from_iter(Evas_Object *tbo, Etk_Textblock_Iter *iter);
static int _etk_textblock_text_nodes_count(Etk_Textblock_Node *line, Etk_Textblock_Node *node);

static Eina_List *_etk_textblock_object_range_geometry_get(Evas_Object *tbo, Etk_Textblock_Iter *start, Etk_Textblock_Iter *end);
static void _etk_textblock_object_iter_move_to(Evas_Object *tbo, Etk_Textblock_Iter *iter, int x, int y);

static void _etk_tb_object_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tb_object_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tb_object_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void _etk_tb_object_smart_add(Evas_Object *obj);
static void _etk_tb_object_smart_del(Evas_Object *obj);
static void _etk_tb_object_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_tb_object_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_tb_object_smart_show(Evas_Object *obj);
static void _etk_tb_object_smart_hide(Evas_Object *obj);
static void _etk_tb_object_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _etk_tb_object_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _etk_tb_object_smart_clip_unset(Evas_Object *obj);


static Evas_Smart *_etk_tb_object_smart = NULL;
static int _etk_tb_object_smart_use = 0;
static const char *_etk_tb_escapes[] =
{
   "&lt;", "<",
   "&gt;", ">",
   "&amp;", "&"
};


/**************************
 *
 * Implementation
 *
 **************************/

/**************************
 * Textblock's funcs
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Textblock
 * @return Returns the type of an Etk_Textblock
 */
Etk_Type *etk_textblock_type_get()
{
   static Etk_Type *textblock_type = NULL;

   if (!textblock_type)
   {
      textblock_type = etk_type_new("Etk_Textblock", ETK_OBJECT_TYPE,
         sizeof(Etk_Textblock),
         ETK_CONSTRUCTOR(_etk_tb_constructor),
         ETK_DESTRUCTOR(_etk_tb_destructor), NULL);
   }

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

/**
 * @brief Sets the text of the textblock. The current text will be replaced by @a text
 * @param tb a textblock
 * @param text the text to set
 * @param markup whether or not you want to apply markup and styling on the text.
 * If @a markup is ETK_TRUE, you can use tags to format the text.
 * @note After that, the iterators with right gravity will be at the end of the textblock,
 * and iterators with left gravity will be at the start of the textblock
 */
void etk_textblock_text_set(Etk_Textblock *tb, const char *text, Etk_Bool markup)
{
   Etk_Textblock_Iter *iter;

   if (!tb)
      return;

   etk_textblock_clear(tb);

   if (!text)
      return;

   iter = etk_textblock_iter_new(tb);
   if (!markup)
      etk_textblock_insert(tb, iter, text, -1);
   else
      etk_textblock_insert_markup(tb, iter, text, -1);
   etk_textblock_iter_free(iter);
}

/**
 * @brief Gets the text of the textblock
 * @param tb a textblock
 * @param markup whether or not you want to have tags in the returned text
 * @return Returns a string containing the text of the textblock
 * @note Once you no longer need the returned string, you should destroy it with etk_object_destroy()
 */
Etk_String *etk_textblock_text_get(Etk_Textblock *tb, Etk_Bool markup)
{
   Etk_String *text;

   if (!tb)
      return NULL;

   text = etk_string_new(NULL);
   _etk_textblock_node_text_get(&tb->root, markup, text, NULL, NULL);

   return text;
}

/**
 * @brief Gets the text of the textblock, between @a iter1 and @a iter2
 * @param tb a textblock
 * @param iter1 the first iterator
 * @param iter2 the second_iterator
 * @param markup whether or not you want to have tags in the returned text
 * @return Returns a string containing the text of the textblock, between @a iter1 and @a iter2
 * @note Once you no longer need the returned string, you should destroy it with etk_object_destroy()
 * @note @a iter1 is not necessarily before @a iter2
 */
Etk_String *etk_textblock_range_text_get(Etk_Textblock *tb, Etk_Textblock_Iter *iter1, Etk_Textblock_Iter *iter2, Etk_Bool markup)
{
   Etk_String *text;
   int compare_res;

   if (!tb || !iter1 || !iter2)
      return NULL;

   compare_res = etk_textblock_iter_compare(iter1, iter2);
   if (compare_res == 0)
      return NULL;

   text = etk_string_new(NULL);
   if (compare_res < 0)
      _etk_textblock_node_text_get(&tb->root, markup, text, iter1, iter2);
   else
      _etk_textblock_node_text_get(&tb->root, markup, text, iter2, iter1);

   return text;
}

/**
 * @brief Inserts @a length bytes of @a text at @a iter in the textblock.
 * If you want to use tags to format the text to insert, use etk_textblock_insert_markup() instead.
 * @param tb a textblock
 * @param iter the iterator where to insert the text. @n
 * If the gravity of @a iter is left, the iter will be placed before the inserted text.
 * Otherwise, the iterator will placed after the inserted text.
 * @param text the unicode-encoded text to insert
 * @param length the number of bytes to insert. If @a length is negative, the text will be entirely inserted
 */
void etk_textblock_insert(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *text, int length)
{
   Eina_List *l;
   Etk_Textblock_Iter *it, *it2;
   int unicode_length, char_length;
   int pos, index;
   int i, node_start, node_end, esc_start, esc_end;
   Etk_Bool done, new_line;
   const char *esc_char;

   if (!tb || !iter || !text)
      return;
   if (!_etk_textblock_iter_is_valid(tb, iter))
      return;

   if (iter->gravity == ETK_TEXTBLOCK_GRAVITY_LEFT)
   {
      it = etk_textblock_iter_new(tb);
      etk_textblock_iter_copy(it, iter);
      it->gravity = ETK_TEXTBLOCK_GRAVITY_RIGHT;
   }
   else
      it = iter;

   index = -1;
   node_start = -1;
   node_end = -1;
   esc_start = -1;
   esc_end = -1;
   new_line = ETK_FALSE;
   unicode_length = 0;
   for (i = 0, done = ETK_FALSE; !done; i = evas_string_char_next_get(text, i, NULL))
   {
      /* Have we finished? */
      if (text[i] == '\0' || index == i || (length >= 0 && i >= length))
      {
         if (node_start >= 0)
            node_end = index;
         if (esc_start >= 0)
            esc_end = index;
         done = ETK_TRUE;
         unicode_length--;
      }

      /* Detects the escape sequences */
      if (esc_start < 0 && text[i] == '&')
      {
         esc_start = i;
         if (node_start > 0)
            node_end = i - 1;
         unicode_length--;
      }
      if (esc_start >= 0 && text[i] == ';')
         esc_end = i;

      /* Detects the ends of line */
      if (text[i] == '\n' || text[i] == '\r')
      {
         if (node_start >= 0)
            node_end = i - 1;
         if (esc_start >= 0)
            esc_end = i - 1;
         new_line = ETK_TRUE;
         unicode_length--;

         if ((length < 0 || i < length) && text[i] == '\r' && text[i + 1] == '\n')
            i++;
      }

      if (esc_start < 0 && node_start < 0)
         node_start = i;
      unicode_length++;

      /* If we have parsed a full node, a escape sequence or a end-of-line sequence, we insert the text or
       * we create a new line */
      if ((node_start >= 0 && node_end >= 0) || (esc_start >= 0 && esc_end >= 0)  || new_line)
      {
         char_length = 0;

         /* Escape sequence */
         if (esc_start >= 0 && esc_end >= 0)
         {
            if ((esc_char = _etk_textblock_escape_parse(&text[esc_start], esc_end - esc_start + 1)))
            {
               char_length = strlen(esc_char);
               unicode_length = 1;
               it->node->text = etk_string_insert_sized(it->node->text, it->index, esc_char, char_length);
               it->node->unicode_length += unicode_length;
               node_start = -1;

               _etk_textblock_node_update(tb, it->node);
            }
            else
            {
               node_start = esc_start;
               node_end = esc_end;
            }
            esc_start = -1;
            esc_end = - 1;
         }
         /* Normal text */
         if ((node_start >= 0 && node_end >= 0))
         {
            char_length = node_end - node_start + 1;

            if (char_length > 0)
            {
               it->node->text = etk_string_insert_sized(it->node->text, it->index, &text[node_start], char_length);
               it->node->unicode_length += unicode_length;

               _etk_textblock_node_update(tb, it->node);
            }
            else
               char_length = 0;

            node_start = -1;
            node_end = - 1;
         }

         /* Updates the iterators */
         if (char_length > 0)
         {
            pos = it->pos;
            index = it->index;
            for (l = tb->iters; l; l = l->next)
            {
               it2 = l->data;
               if (it2->node == it->node)
               {
                  if (it2->pos > pos || (it2->pos == pos && it2->gravity == ETK_TEXTBLOCK_GRAVITY_RIGHT))
                  {
                     it2->pos += unicode_length;
                     it2->index += char_length;
                  }
               }
            }
         }

         /* We create a new line */
         if (new_line)
         {
            _etk_textblock_line_add(tb, it);
            node_start = -1;
            node_end = - 1;
            esc_start = -1;
            esc_end = - 1;
         }

         new_line = ETK_FALSE;
         unicode_length = 0;
      }

      index = i;
   }

   if (it->gravity == ETK_TEXTBLOCK_GRAVITY_LEFT)
      etk_textblock_iter_free(it);
}

/**
 * @brief Inserts @a length bytes of @a text at @a iter in the textblock. You can use tags to format the text to insert.
 * @param tb a textblock
 * @param iter the iterator where to insert the text. @n
 * If the gravity of @a iter is left, the iter will be placed before the inserted text.
 * Otherwise, the iterator will placed after the inserted text.
 * @param text the unicode-encoded markup text to insert
 * @param length the number of bytes to insert. If @a length is negative, the text will be entirely inserted
 */
void etk_textblock_insert_markup(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *markup_text, int length)
{
   const char *t;
   const char *text_start = NULL;
   const char *tag_start = NULL;
   Etk_Textblock_Iter *it;
   Etk_Textblock_Node *start_line, *end_line, *line, *n;

   if (!tb || !iter || !markup_text)
      return;
   if (!_etk_textblock_iter_is_valid(tb, iter))
      return;

   if (iter->gravity == ETK_TEXTBLOCK_GRAVITY_LEFT)
   {
      it = etk_textblock_iter_new(tb);
      etk_textblock_iter_copy(it, iter);
      it->gravity = ETK_TEXTBLOCK_GRAVITY_RIGHT;
   }
   else
      it = iter;

   start_line = NULL;
   for (n = it->node; n; n = n->parent)
   {
      if (n->type == ETK_TEXTBLOCK_NODE_LINE)
      {
         start_line = n;
         break;
      }
   }

   /* Insert the tags and the text */
   for (t = markup_text; *t != '\0'; t++)
   {
      if (!text_start && !tag_start)
      {
         if (*t == '<')
            tag_start = t;
         else
            text_start = t;
      }

      if (text_start)
      {
         if (*(t + 1) == '<' || *(t + 1) == '\0')
         {
            etk_textblock_insert(tb, it, text_start, t - text_start + 1);
            text_start = NULL;
         }
      }
      else if (tag_start)
      {
         if (*t == '>')
         {
            _etk_textblock_tag_insert(tb, it, tag_start, t - tag_start + 1);
            tag_start = NULL;
         }
      }
   }

   end_line = NULL;
   for (n = it->node; n; n = n->parent)
   {
      if (n->type == ETK_TEXTBLOCK_NODE_LINE)
      {
         end_line = n;
         break;
      }
   }

   /* Cleanup the modified lines */
   for (line = start_line; line; line = _etk_textblock_next_line_get(line))
   {
      _etk_textblock_nodes_clean(tb, line->children);

      if (line == end_line)
         break;
   }

   if (iter->gravity == ETK_TEXTBLOCK_GRAVITY_LEFT)
      etk_textblock_iter_free(it);
}

/**
 * @brief Clears the textblock: removes all the text and all the tags
 * @param tb the textblock to clear
 */
void etk_textblock_clear(Etk_Textblock *tb)
{
   Eina_List *l;
   Evas_Object *tbo;
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line, *next;
   Etk_Textblock_Node *node;

   if (!tb)
      return;

   /* Frees the nodes */
   while (tb->root.children)
      _etk_textblock_node_free(tb->root.children);

   /* Adds an empty line */
   node = _etk_textblock_node_new(&tb->root, NULL, ETK_TEXTBLOCK_NODE_PARAGRAPH, ETK_TEXTBLOCK_TAG_P);
   node = _etk_textblock_node_new(node, NULL, ETK_TEXTBLOCK_NODE_LINE, ETK_TEXTBLOCK_TAG_DEFAULT);

   /* Updates the textblock objects */
   for (l = tb->evas_objects; l; l = l->next)
   {
      if (!(tbo = l->data) || !(tbo_sd = evas_object_smart_data_get(tbo)))
         continue;

      for (line = tbo_sd->lines; line; line = next)
      {
         if (line->object)
            evas_object_del(line->object);
         next = line->next;
         free(line);
      }
      tbo_sd->lines = NULL;
      tbo_sd->last_line = NULL;

      _etk_textblock_object_line_add(tbo, node);
   }

   /* Updates the iters */
   for (l = tb->iters; l; l = l->next)
      etk_textblock_iter_backward_start(l->data);
}

/**
 * @brief Deletes the char before the iterator
 * @param tb a textblock
 * @param iter the iterator where to delete the char
 */
void etk_textblock_delete_before(Etk_Textblock *tb, Etk_Textblock_Iter *iter)
{
   Etk_Textblock_Iter *iter2;

   if (!tb || !iter || !_etk_textblock_iter_is_valid(tb, iter))
      return;

   iter2 = etk_textblock_iter_new(tb);
   etk_textblock_iter_copy(iter2, iter);

   etk_textblock_iter_backward_char(iter);
   etk_textblock_delete_range(tb, iter, iter2);

   etk_textblock_iter_free(iter2);
}

/**
 * @brief Deletes the char after the iterator
 * @param tb a textblock
 * @param iter the iterator where to delete the char
 */
void etk_textblock_delete_after(Etk_Textblock *tb, Etk_Textblock_Iter *iter)
{
   Etk_Textblock_Iter *iter2;

   if (!tb || !iter || !_etk_textblock_iter_is_valid(tb, iter))
      return;

   iter2 = etk_textblock_iter_new(tb);
   etk_textblock_iter_copy(iter2, iter);

   etk_textblock_iter_forward_char(iter);
   etk_textblock_delete_range(tb, iter, iter2);

   etk_textblock_iter_free(iter2);
}

/**
 * @brief Deletes the text between @a iter1 and @a iter2
 * @param tb a textblock
 * @param iter1 the first iterator
 * @param iter2 the second iterator
 * @note @a iter1 is not necessarily before @a iter2
 */
void etk_textblock_delete_range(Etk_Textblock *tb, Etk_Textblock_Iter *iter1, Etk_Textblock_Iter *iter2)
{
   int compare_res;
   Etk_Textblock_Iter *start_iter, *end_iter;
   Etk_Textblock_Iter *it;
   Etk_Textblock_Node *start_line, *end_line, *line;
   Etk_Textblock_Node *start_paragraph, *end_paragraph, *paragraph;
   Etk_Textblock_Node *n, *new_node;
   Eina_List *l;
   Etk_Bool done;

   if (!tb || !iter1 || !iter2)
      return;
   if (!_etk_textblock_iter_is_valid(tb, iter1) || !_etk_textblock_iter_is_valid(tb, iter2))
      return;

   compare_res = etk_textblock_iter_compare(iter1, iter2);
   if (compare_res == 0)
      return;
   else if (compare_res < 0)
   {
      start_iter = iter1;
      end_iter = iter2;
   }
   else
   {
      start_iter = iter2;
      end_iter = iter1;
   }

   start_line = NULL;
   start_paragraph = NULL;
   for (n = start_iter->node; n; n = n->parent)
   {
      if (n->type == ETK_TEXTBLOCK_NODE_LINE)
         start_line = n;
      else if (n->type == ETK_TEXTBLOCK_NODE_PARAGRAPH)
         start_paragraph = n;
   }

   end_line = NULL;
   end_paragraph = NULL;
   for (n = end_iter->node; n; n = n->parent)
   {
      if (n->type == ETK_TEXTBLOCK_NODE_LINE)
         end_line = n;
      else if (n->type == ETK_TEXTBLOCK_NODE_PARAGRAPH)
         end_paragraph = n;
   }

   done = ETK_FALSE;
   for (n = start_iter->node; n && !done; n = _etk_textblock_next_node_get(n))
   {
      /* We cut the text of the node */
      if (n == start_iter->node)
      {
         if (n == end_iter->node)
         {
            etk_string_delete(n->text, start_iter->index, end_iter->index - start_iter->index);
            n->unicode_length -= (end_iter->pos - start_iter->pos);
            done = ETK_TRUE;
         }
         else
         {
            etk_string_delete(n->text, start_iter->index, etk_string_length_get(n->text) - start_iter->index);
            n->unicode_length = start_iter->pos;
         }
      }
      else if (n == end_iter->node)
      {
         etk_string_delete(n->text, 0, end_iter->index);
         n->unicode_length -= end_iter->pos;
         done = ETK_TRUE;
      }
      else
      {
         etk_string_clear(n->text);
         n->unicode_length = 0;
      }

      /* We update the iterators */
      for (l = tb->iters; l; l = l->next)
      {
         it = l->data;
         if (it->node == n)
         {
            if (it->node == start_iter->node && it->pos <= start_iter->pos)
               continue;

            if (it != end_iter && it->node == end_iter->node && it->pos > end_iter->pos)
            {
               it->index -= end_iter->index;
               it->pos -= end_iter->pos;
            }
            else
               etk_textblock_iter_copy(it, start_iter);
         }
      }
   }
   etk_textblock_iter_copy(end_iter, start_iter);

   /* Merge the start and the end lines, and remove the other lines */
   if (start_line != end_line)
   {
      /* Make sure the start line node has at least one child */
      if (etk_string_length_get(start_line->text) > 0)
      {
         new_node = _etk_textblock_node_new(start_line, NULL, ETK_TEXTBLOCK_NODE_NORMAL, ETK_TEXTBLOCK_TAG_DEFAULT);
         new_node->text = start_line->text;
         new_node->unicode_length = start_line->unicode_length;
         start_line->text = NULL;
         start_line->unicode_length = 0;

         for (l = tb->iters; l; l = l->next)
         {
            it = l->data;
            if (it->node == start_line)
               it->node = new_node;
         }
      }

      /* Add the children of the end line node to the start line node */
      while (end_line->children)
         _etk_textblock_node_attach(end_line->children, start_line, start_line->last_child);

      done = ETK_FALSE;
      for (line = _etk_textblock_next_line_get(start_line); line && !done; line = _etk_textblock_line_del(tb, line))
      {
         if (line == end_line)
            done = ETK_TRUE;
      }
   }

   /* Remove the empty paragraphs */
   if (start_paragraph != end_paragraph)
   {
      for (paragraph = start_paragraph->next; paragraph && !done; paragraph = paragraph->next)
      {
         if (paragraph == end_paragraph)
            done = ETK_TRUE;

         if (_etk_textblock_node_is_empty(paragraph))
            paragraph = _etk_textblock_node_free(paragraph);
         else
            paragraph = paragraph->next;
      }
   }

   _etk_textblock_nodes_clean(tb, start_line);
   _etk_textblock_node_update(tb, start_line);
}

/**************************
 * Textblock Iter's funcs
 **************************/

/**
 * @brief Creates a new iterator for the textblock
 * @param tb a textblock
 * @return Returns the new iterator, placed at the start of the first node of the textblock
 */
Etk_Textblock_Iter *etk_textblock_iter_new(Etk_Textblock *tb)
{
   Etk_Textblock_Iter *iter;

   if (!tb)
      return NULL;

   iter = malloc(sizeof(Etk_Textblock_Iter));
   iter->tb = tb;
   iter->node = &tb->root;
   iter->gravity = ETK_TEXTBLOCK_GRAVITY_RIGHT;
   iter->pos = 0;
   iter->index = 0;

   tb->iters = eina_list_append(tb->iters, iter);
   etk_textblock_iter_backward_start(iter);

   return iter;
}

/**
 * @brief Destroys an iterator of the textblock
 * @param iter the iterator to destroy
 */
void etk_textblock_iter_free(Etk_Textblock_Iter *iter)
{
   if (!iter)
      return;

   if (iter->tb)
      iter->tb->iters = eina_list_remove(iter->tb->iters, iter);
   free(iter);
}

/**
 * @brief Sets the gravity of the iterator.
 * The gravity describes how the iterator should be placed when text is inserted at the iterator's position
 * @param iter an iterator
 * @param gravity the gravity to set to the iterator
 */
void etk_textblock_iter_gravity_set(Etk_Textblock_Iter *iter, Etk_Textblock_Gravity gravity)
{
   if (!iter)
      return;
   iter->gravity = gravity;
}

/**
 * @brief Gets the gravity of the iterator
 * @param iter an iterator
 * @return Returns the gravity of the iterator (ETK_TEXTBLOCK_GRAVITY_RIGHT by default)
 */
Etk_Textblock_Gravity etk_textblock_iter_gravity_get(Etk_Textblock_Iter *iter)
{
   if (!iter)
      return ETK_TEXTBLOCK_GRAVITY_RIGHT;
   return iter->gravity;
}

/**
 * @brief Moves the iterator to the start of the textblock
 * @param iter an iterator
 */
void etk_textblock_iter_backward_start(Etk_Textblock_Iter *iter)
{
   if (!iter || !iter->tb)
      return;

   iter->node = &iter->tb->root;
   while (iter->node->children)
      iter->node = iter->node->children;

   iter->index = 0;
   iter->pos = 0;

   _etk_textblock_iter_update(iter);
}

/**
 * @brief Moves the iterator to the end of the textblock
 * @param iter an iterator
 */
void etk_textblock_iter_forward_end(Etk_Textblock_Iter *iter)
{
   if (!iter || !iter->tb)
      return;

   iter->node = &iter->tb->root;
   while (iter->node->children)
      iter->node = iter->node->last_child;

   iter->pos = iter->node->unicode_length;
   iter->index = etk_string_length_get(iter->node->text);

   _etk_textblock_iter_update(iter);
}

/**
 * @brief Moves the iterator backward by one character offset
 * @param iter an iterator
 * @return Returns ETK_FALSE if the movement was not possible (i.e. the iterator is already
 * before the first character of the textblock)
 */
Etk_Bool etk_textblock_iter_backward_char(Etk_Textblock_Iter *iter)
{
   if (!(iter || !_etk_textblock_iter_is_valid(NULL, iter)))
      return ETK_FALSE;

   if (iter->pos > 1)
   {
      iter->pos--;
      iter->index = evas_string_char_prev_get(etk_string_get(iter->node->text), iter->index, NULL);
      _etk_textblock_iter_update(iter);
      return ETK_TRUE;
   }
   else
   {
      Etk_Textblock_Node *prev_text_node;
      Etk_Bool line_has_changed = ETK_FALSE;

      prev_text_node = _etk_textblock_prev_text_node_get(iter->node, ETK_FALSE, &line_has_changed);
      if (iter->pos == 1 && (line_has_changed || !prev_text_node))
      {
         iter->pos = 0;
         iter->index = 0;
         _etk_textblock_iter_update(iter);
         return ETK_TRUE;
      }
      else if (prev_text_node)
      {
         iter->node = prev_text_node;
         iter->pos = prev_text_node->unicode_length;
         iter->index = etk_string_length_get(prev_text_node->text);
         _etk_textblock_iter_update(iter);
         return ETK_TRUE;
      }
      else
         return ETK_FALSE;
   }
}

/**
 * @brief Moves the iterator forward by one character offset
 * @param iter an iterator
 * @return Returns ETK_FALSE if the movement was not possible (i.e. the iterator is already
 * after the last character of the textblock)
 */
Etk_Bool etk_textblock_iter_forward_char(Etk_Textblock_Iter *iter)
{
   if (!(iter || !_etk_textblock_iter_is_valid(NULL, iter)))
      return ETK_FALSE;

   if (iter->pos < iter->node->unicode_length)
   {
      iter->pos++;
      iter->index = evas_string_char_next_get(etk_string_get(iter->node->text), iter->index, NULL);
      _etk_textblock_iter_update(iter);
      return ETK_TRUE;
   }
   else
   {
      Etk_Textblock_Node *next_text_node;
      Etk_Bool line_has_changed = ETK_FALSE;

      next_text_node = _etk_textblock_next_text_node_get(iter->node, ETK_FALSE, &line_has_changed);
      if (next_text_node)
      {
         iter->node = next_text_node;
         if (line_has_changed)
         {
            iter->pos = 0;
            iter->index = 0;
         }
         else
         {
            iter->pos = 1;
            iter->index = evas_string_char_next_get(etk_string_get(next_text_node->text), 0, NULL);
         }
         _etk_textblock_iter_update(iter);
         return ETK_TRUE;
      }
      else
         return ETK_FALSE;
   }
}

/**
 * @brief Copies the iterator @a src to the iterator @a dest
 * @param dest the destination iterator
 * @param src the source iterator
 */
void etk_textblock_iter_copy(Etk_Textblock_Iter *dest, const Etk_Textblock_Iter *src)
{
   if (!dest || !src || src->tb != dest->tb)
      return;

   dest->node = src->node;
   dest->gravity = src->gravity;
   dest->pos = src->pos;
   dest->index = src->index;
   _etk_textblock_iter_update(dest);
}

/**
 * @brief Compares two iterators
 * @param iter1 the first iterator
 * @param iter2 the second iterator
 * @return Returns -1 if @a iter1 is before @a iter2, 1 if @a iter1 is after @a iter2
 * and 0 if @a iter1 is equal to @a iter2
 */
int etk_textblock_iter_compare(Etk_Textblock_Iter *iter1, Etk_Textblock_Iter *iter2)
{
   int node_compare;

   if (!iter1 || !iter2 || !iter1->tb)
      return -1;
   if (!_etk_textblock_iter_is_valid(iter1->tb, iter1) || !_etk_textblock_iter_is_valid(iter1->tb, iter2))
      return -1;
   if (iter1 == iter2)
      return 0;

   if ((node_compare = _etk_textblock_node_compare(iter1->node, iter2->node)) != 0)
      return node_compare;
   else
   {
      if (iter1->pos < iter2->pos)
         return -1;
      else if (iter1->pos > iter2->pos)
         return 1;
      else
         return 0;
   }
}

/**************************
 * Textblock Object's funcs
 **************************/

/**
 * @brief Creates a new evas object that will display the content of the textblock. @n
 * A textblock can have several evas objects which display its content. All the evas objects are automatically updated
 * when the textblock's content is changed.
 * @param tb the textblock whose content will be displayed by the evas object
 * @param evas the evas to which the object will be added
 * @return Returns a new evas object that display the content of the textblock
 */
Evas_Object *etk_textblock_object_add(Etk_Textblock *tb, Evas *evas)
{
   Evas_Object *obj;
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Node *paragraph, *line;

   if (!tb || !evas)
      return NULL;

   if (!_etk_tb_object_smart)
   {
      static const Evas_Smart_Class sc =
      {
         "Textblock_Object",
         EVAS_SMART_CLASS_VERSION,
         _etk_tb_object_smart_add,
         _etk_tb_object_smart_del,
         _etk_tb_object_smart_move,
         _etk_tb_object_smart_resize,
         _etk_tb_object_smart_show,
         _etk_tb_object_smart_hide,
         _etk_tb_object_smart_color_set,
         _etk_tb_object_smart_clip_set,
         _etk_tb_object_smart_clip_unset,
         NULL,
         NULL
      };
      _etk_tb_object_smart = evas_smart_class_new(&sc);
   }
   _etk_tb_object_smart_use++;

   obj = evas_object_smart_add(evas, _etk_tb_object_smart);
   tbo_sd = evas_object_smart_data_get(obj);
   tbo_sd->tb= tb;

   /* We create the lines */
   for (paragraph = tb->root.children; paragraph; paragraph = paragraph->next)
   {
      for (line = paragraph->children; line; line = line->next)
         _etk_textblock_object_line_add(obj, line);
   }

   tbo_sd->cursor = etk_textblock_iter_new(tb);
   tbo_sd->selection = etk_textblock_iter_new(tb);

   tb->evas_objects = eina_list_append(tb->evas_objects, obj);

   return obj;
}

/**
 * @brief Sets how the text of the textblock object should be wrapped by default (a paragraph can override this setting)
 * @param tbo a textblock object
 * @param wrap the wrap mode. Here, ETK_TEXTBLOCK_WRAP_DEFAULT is equivalent to ETK_TEXTBLOCK_WRAP_WORD
 */
void etk_textblock_object_wrap_set(Evas_Object *tbo, Etk_Textblock_Wrap wrap)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   if (wrap == ETK_TEXTBLOCK_WRAP_DEFAULT)
      wrap = ETK_TEXTBLOCK_WRAP_WORD;
   if (tbo_sd->wrap != wrap)
      tbo_sd->wrap = wrap;


   /* Updates the object's lines */
   /* TODO: Does this code work, and do we really need this (with the future optimizations)?? */
   for (line = tbo_sd->lines; line; line = line->next)
   {
      if (line->object && line->node && line->node->parent
         && line->node->parent->type == ETK_TEXTBLOCK_NODE_PARAGRAPH
         && line->node->parent->tag.params.p.wrap == ETK_TEXTBLOCK_WRAP_DEFAULT)
      {
         Evas_Textblock_Cursor *cur;
         const char *format;

         cur = evas_object_textblock_cursor_new(line->object);
         for (evas_textblock_cursor_node_first(cur); evas_textblock_cursor_node_next(cur); )
         {
            if ((format = evas_textblock_cursor_node_format_get(cur)) && strstr(format, "wrap"))
            {
               if (wrap == ETK_TEXTBLOCK_WRAP_WORD)
                  evas_textblock_cursor_format_append(cur, "+ wrap=word");
               else if (wrap == ETK_TEXTBLOCK_WRAP_CHAR)
                  evas_textblock_cursor_format_append(cur, "+ wrap=char");
               else
                  evas_textblock_cursor_format_append(cur, "+ wrap=none");
               evas_textblock_cursor_node_prev(cur);
               evas_textblock_cursor_node_delete(cur);
               break;
            }
         }
         evas_textblock_cursor_free(cur);

         _etk_textblock_object_line_update_queue(tbo, line, ETK_FALSE, ETK_TRUE);
      }
   }
}

/**
 * @brief Gets the default wrap mode of the textblock object
 * @param tbo a textblock object
 * @return Returns the wrap mode of the textblock object
 */
Etk_Textblock_Wrap etk_textblock_object_wrap_get(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return ETK_TEXTBLOCK_WRAP_WORD;
   return tbo_sd->wrap;
}

int etk_textblock_object_yoffset_get( Evas_Object *tbo )
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
       return 0;

    return tbo_sd->yoffset;
}

int etk_textblock_object_xoffset_get( Evas_Object *tbo )
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return 0;

   return tbo_sd->xoffset;
}

void etk_textblock_object_yoffset_set( Evas_Object *tbo, int yoffset )
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   tbo_sd->yoffset = yoffset;

   /* TODO: Update the object? */
}

void etk_textblock_object_xoffset_set( Evas_Object *tbo, int xoffset )
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

    tbo_sd->xoffset = xoffset;

   /* TODO: Update the object? */
}

void etk_textblock_object_full_geometry_get( Evas_Object *tbo, int *x, int *y, int *w, int *h )
{
   if(!tbo)
      return;

   _etk_textblock_object_update(tbo);
   if(x)
   {
      evas_object_geometry_get( tbo, x, NULL, NULL, NULL );
   }

   if(y)
   {
      evas_object_geometry_get( tbo, NULL, y, NULL, NULL );
   }

   if(w)
   {
      evas_object_geometry_get( tbo, NULL, NULL, w, NULL );
   }

   if(h)
   {
       Etk_Textblock_Object_SD *tbo_sd;
       Etk_Textblock_Object_Line *line;

	   *h = 0;

       if( !( tbo_sd = evas_object_smart_data_get(tbo) ) )
          return;

       for (line = tbo_sd->lines; line; line = line->next)
          *h += line->geometry.h;
    }
}

/**
 * @brief Gets the cursor's iterator of the textblock object
 * @param tbo a textblock object
 * @return Returns the cursor's iterator of the textblock object
 * @warning You should not free the returned iterator
 */
Etk_Textblock_Iter *etk_textblock_object_cursor_get(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return NULL;
   return tbo_sd->cursor;
}

/**
 * @brief Gets the selection bound's iterator of the textblock object
 * @param tbo a textblock object
 * @return Returns the selection bound's iterator of the textblock object
 * @warning You should not free the returned iterator
 */
Etk_Textblock_Iter *etk_textblock_object_selection_bound_get(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return NULL;
   return tbo_sd->selection;
}

/**
 * @brief Sets whether the cursor of the textblock object is visible or not
 * @param tbo a textblock object
 * @param visible ETK_TRUE to show the cursor, ETK_FALSE to hide it
 */
void etk_textblock_object_cursor_visible_set(Evas_Object *tbo, Etk_Bool visible)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)) || tbo_sd->cursor_visible == visible)
      return;

   if (evas_object_visible_get(tbo))
   {
      if (visible)
      {
         evas_object_show(tbo_sd->cursor_object);
         if (!tbo_sd->cursor_timer)
         {
            tbo_sd->cursor_timer = ecore_timer_add(ETK_TB_OBJECT_SHOW_CURSOR_DELAY,
               _etk_textblock_object_cursor_timer_cb, tbo);
         }
         else
            ecore_timer_interval_set(tbo_sd->cursor_timer, ETK_TB_OBJECT_SHOW_CURSOR_DELAY);
      }
      else
      {
         evas_object_hide(tbo_sd->cursor_object);
         ecore_timer_del(tbo_sd->cursor_timer);
         tbo_sd->cursor_timer = NULL;
      }
   }

   tbo_sd->cursor_visible = visible;
}

/**
 * @brief Gets whether the cursor of the textblock object is visible or not
 * @param tbo a textblock object
 * @return Returns ETK_TRUE if the cursor is visible, ETK_FALSE otherwise
 */
Etk_Bool etk_textblock_object_cursor_visible_get(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return ETK_FALSE;
   return tbo_sd->cursor_visible;
}

/**************************
 * Misc funcs
 **************************/

/**
 * @brief Gets the length of a unicode-encoded string.
 * It's not always the number of bytes of the string since a character can be encoded with several bytes in unicode.
 * @param unicode_string a unicode-encoded string
 * @return Returns the unicode length of @a unicode_string
 */
int etk_textblock_unicode_length_get(const char *unicode_string)
{
   int pos = -1, i = 0;

   if (!unicode_string)
      return 0;

   while (pos < i)
   {
      pos = i;
      i = evas_string_char_next_get(unicode_string, pos, NULL);
   }
   return pos;
}

/**
 * @brief Gets whether or not the unicode char is a white char
 * @param c a unicode char
 * @return Returns ETK_TRUE if @a c is a white char, ETK_FALSE otherwise
 */
Etk_Bool etk_textblock_is_white_char(int c)
{
   /* Imported from Evas */
   return ((c == 0x20) || ((c >= 0x9) && (c <= 0xd)) || (c == 0x85)
      || (c == 0xa0) || (c == 0x1680) || (c == 0x180e) || ((c >= 0x2000) && (c <= 0x200a))
      || (c == 0x2028) || (c == 0x2029) || (c == 0x202f) || (c == 0x205f) || (c == 0x3000));
}

/**
 * @brief Gets the average size of a character written with font @a font_name
 * @param evas an evas
 * @param font_face the face of the font to use
 * @param font_size the size of the font to use
 * @param w the location where to store the average width of a character
 * @param h the location where to store the average height of a character
 */
/* TODO: cache the result, better test text, "Use bold, italic, style..." ! */
void etk_textblock_char_size_get(Evas *evas, const char *font_face, int font_size, int *w, int *h)
{
   Evas_Object *text_object;
   char *text = "This is a test";
   int len = 14;
   int tw = 0;

   if (!evas || !font_face)
      return;

   text_object = evas_object_text_add(evas);
   evas_object_text_font_set(text_object, font_face, font_size);
   evas_object_text_text_set(text_object, text);
   evas_object_geometry_get(text_object, NULL, NULL, &tw, h);
   evas_object_del(text_object);

   if (w)
      *w = tw / len;
}

/**
 * @brief Prints the textblock hierarchy for debug
 * TODO: etk_textblock_printf(): Remove this function
 */
void etk_textblock_printf(Etk_Textblock *tb)
{
   Eina_List *l;
   Etk_Textblock_Object_SD *sd;

   if (!tb)
      return;

   printf("TEXTBLOCK PRINTF\n"
          "----------------\n");
   _etk_textblock_node_printf(&tb->root, -1);
   printf("\n");
   for (l = tb->evas_objects; l; l = l->next)
   {
      sd = evas_object_smart_data_get(l->data);
      printf("Iterator node: %d %d | %s\n", sd->cursor->node->type, sd->cursor->node->tag.type,
         etk_string_get(sd->cursor->node->text) ? etk_string_get(sd->cursor->node->text) : "NULL");
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the textblock */
static void _etk_tb_constructor(Etk_Textblock *tb)
{
   Etk_Textblock_Node *node;

   if (!tb)
      return;

   tb->root.type = ETK_TEXTBLOCK_NODE_ROOT;
   tb->root.tag.type = ETK_TEXTBLOCK_TAG_DEFAULT;
   tb->root.text = NULL;
   tb->root.unicode_length = 0;
   tb->root.parent = NULL;
   tb->root.prev = NULL;
   tb->root.next = NULL;
   tb->root.children = NULL;
   tb->root.last_child = NULL;

   /* Adds an empty line */
   node = _etk_textblock_node_new(&tb->root, NULL, ETK_TEXTBLOCK_NODE_PARAGRAPH, ETK_TEXTBLOCK_TAG_P);
   _etk_textblock_node_new(node, NULL, ETK_TEXTBLOCK_NODE_LINE, ETK_TEXTBLOCK_TAG_DEFAULT);

   tb->iters = NULL;
   tb->evas_objects = NULL;
}

/* Destroys the textblock */
static void _etk_tb_destructor(Etk_Textblock *tb)
{
   if (!tb)
      return;

   while (tb->evas_objects)
      evas_object_del(tb->evas_objects->data);

   while (tb->iters)
      etk_textblock_iter_free(tb->iters->data);

   while (tb->root.children)
      _etk_textblock_node_free(tb->root.children);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Print the node hierarchy for debug */
static void _etk_textblock_node_printf(Etk_Textblock_Node *node, int n_tabs)
{
   int i;
   Etk_Textblock_Node *n;

   if (!node)
      return;

   for (i = 0; i < n_tabs; i++)
      printf("\t");
   printf("NODE TAG: %d %d\n", node->type, node->tag.type);
   for (i = 0; i < n_tabs; i++)
      printf("\t");
   printf("NODE TEXT: %d %d\n", etk_string_length_get(node->text), node->unicode_length);
   for (i = 0; i < n_tabs; i++)
      printf("\t");
   printf("%s\n", etk_string_get(node->text) ? etk_string_get(node->text) : "NULL");
   printf("\n");

   for (n = node->children; n; n = n->next)
      _etk_textblock_node_printf(n, n_tabs + 1);
}

/* Creates a new node, and attachs it to the "parent" node, after the "prev" node */
/* If "prev" == NULL, the new node is prepended to the list of nodes of the "parent" */
static Etk_Textblock_Node *_etk_textblock_node_new(Etk_Textblock_Node *parent, Etk_Textblock_Node *prev, Etk_Textblock_Node_Type node_type, Etk_Textblock_Tag_Type tag_type)
{
   Etk_Textblock_Node *node;

   node = malloc(sizeof(Etk_Textblock_Node));
   node->type = ETK_TEXTBLOCK_NODE_NORMAL;
   node->tag.type = ETK_TEXTBLOCK_TAG_DEFAULT;
   node->text = NULL;
   node->unicode_length = 0;
   node->parent = NULL;
   node->prev = NULL;
   node->next = NULL;
   node->children = NULL;
   node->last_child = NULL;

   _etk_textblock_node_type_set(node, node_type, tag_type);
   _etk_textblock_node_attach(node, parent, prev);

   return node;
}

/* Frees the node and its child nodes */
/* Returns the next node (or the previous node if there is no next node) */
static Etk_Textblock_Node *_etk_textblock_node_free(Etk_Textblock_Node *node)
{
   Etk_Textblock_Node *n;

   if (!node)
      return NULL;

   if (node->tag.type == ETK_TEXTBLOCK_TAG_FONT)
      free(node->tag.params.font.face);
   etk_object_destroy(ETK_OBJECT(node->text));

   _etk_textblock_node_attach(node, NULL, NULL);

   while (node->children)
      _etk_textblock_node_free(node->children);

   n = node->next ? node->next : node->prev;
   free(node);

   return n;
}

/* Sets the type of the node */
static void _etk_textblock_node_type_set(Etk_Textblock_Node *node, Etk_Textblock_Node_Type node_type, Etk_Textblock_Tag_Type tag_type)
{
   if (!node)
      return;

   if (node->tag.type == ETK_TEXTBLOCK_TAG_FONT)
      free(node->tag.params.font.face);

   node->type = node_type;
   node->tag.type = tag_type;
   switch (tag_type)
   {
      case ETK_TEXTBLOCK_TAG_UNDERLINE:
         node->tag.params.u.type = ETK_TEXTBLOCK_UNDERLINE_SINGLE;
         node->tag.params.u.color1.r = -1;
         node->tag.params.u.color2.r = -1;
         break;
      case ETK_TEXTBLOCK_TAG_STRIKETHROUGH:
         node->tag.params.s.color.r = -1;
         break;
      case ETK_TEXTBLOCK_TAG_P:
         node->tag.params.p.align = -1.0;
         node->tag.params.p.left_margin = 0;
         node->tag.params.p.right_margin = 0;
         node->tag.params.p.wrap = ETK_TEXTBLOCK_WRAP_DEFAULT;
         break;
      case ETK_TEXTBLOCK_TAG_STYLE:
         node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_NONE;
         node->tag.params.style.color1.r = -1;
         node->tag.params.style.color2.r = -1;
         break;
      case ETK_TEXTBLOCK_TAG_FONT:
         node->tag.params.font.face = NULL;
         node->tag.params.font.size = -1;
         node->tag.params.font.color.r = -1;
         break;
      default:
         break;
   }
}

/* Attachs a node to the "parent" node, after the "prev" node */
/* If "prev" == NULL, the new node is prepended to the list of nodes of the "parent" */
static void _etk_textblock_node_attach(Etk_Textblock_Node *node, Etk_Textblock_Node *parent, Etk_Textblock_Node *prev)
{
   if (!node)
      return;
   if ((prev && prev->parent != parent) || (node->parent == parent && node->prev == prev))
      return;

   /* First, we detach the node */
   if (node->parent)
   {
      if (node->parent->children == node)
         node->parent->children = node->next;
      if (node->parent->last_child == node)
         node->parent->last_child = node->prev;
      if (node->prev)
         node->prev->next = node->next;
      if (node->next)
         node->next->prev = node->prev;
   }

   /* Then we re-attach it */
   if (parent)
   {
      node->prev = prev;
      node->next = prev ? prev->next : parent->children;
      if (node->next)
         node->next->prev = node;
      if (prev)
         prev->next = node;
      else
         parent->children = node;
      if (parent->last_child == prev)
         parent->last_child = node;
   }

   node->parent = parent;
}

/* Gets the format of the node and stores it in "format" */
static void _etk_textblock_node_format_get(Etk_Textblock_Node *node, Etk_Textblock_Format *format)
{
   Etk_Textblock_Node *n;
   Etk_Bool paragraph_met = ETK_FALSE;
   Etk_Bool style_met = ETK_FALSE;

   if (!format)
      return;

   /* Initializes the format: */
   format->align = -1.0;
   format->left_margin = 0.0;
   format->right_margin = 0.0;
   format->wrap = ETK_TEXTBLOCK_WRAP_DEFAULT;
   format->style = ETK_TEXTBLOCK_STYLE_NONE;
   format->style_color1.r = -1;
   format->style_color2.r = -1;
   format->underline = ETK_TEXTBLOCK_UNDERLINE_NONE;
   format->underline_color1.r = -1;
   format->underline_color2.r = -1;
   format->strikethrough = ETK_FALSE;
   format->strikethrough = ETK_FALSE;
   format->font_face = NULL;
   format->font_size = -1;
   format->font_color.r = -1;
   format->bold = ETK_FALSE;
   format->italic = ETK_FALSE;

   for (n = node; n; n = n->parent)
   {
      switch (n->tag.type)
      {
         case ETK_TEXTBLOCK_TAG_BOLD:
            format->bold = ETK_TRUE;
            break;
         case ETK_TEXTBLOCK_TAG_ITALIC:
            format->italic = ETK_TRUE;
            break;
         case ETK_TEXTBLOCK_TAG_UNDERLINE:
            if (format->underline == ETK_TEXTBLOCK_UNDERLINE_NONE)
            {
               format->underline = n->tag.params.u.type;
               format->underline_color1 = n->tag.params.u.color1;
               format->underline_color2 = n->tag.params.u.color2;
            }
            break;
         case ETK_TEXTBLOCK_TAG_STRIKETHROUGH:
            if (!format->strikethrough)
            {
               format->strikethrough = ETK_TRUE;
               format->strikethrough_color = n->tag.params.s.color;
            }
            break;
         case ETK_TEXTBLOCK_TAG_STYLE:
            if (!style_met)
            {
               format->style = n->tag.params.style.type;
               format->style_color1 = n->tag.params.style.color1;
               format->style_color2 = n->tag.params.style.color2;
               style_met = ETK_TRUE;
            }
            break;
         case ETK_TEXTBLOCK_TAG_P:
            if (!paragraph_met)
            {
               format->align = n->tag.params.p.align;
               format->left_margin = n->tag.params.p.left_margin;
               format->right_margin = n->tag.params.p.right_margin;
               format->wrap = n->tag.params.p.wrap;
               paragraph_met = ETK_TRUE;
            }
            break;
         case ETK_TEXTBLOCK_TAG_FONT:
            if (!format->font_face)
               format->font_face = n->tag.params.font.face;
            if (format->font_size < 0)
               format->font_size = n->tag.params.font.size;
            if (format->font_color.r < 0)
               format->font_color = n->tag.params.font.color;
            break;
         default:
            break;
      }
   }
}

/* Removes the empty nodes in the list of 'nodes' */
static void _etk_textblock_nodes_clean(Etk_Textblock *tb, Etk_Textblock_Node *nodes)
{
   Etk_Textblock_Node *n, *p;
   Etk_Textblock_Iter *it;
   Eina_List *l;
   Etk_Bool delete_node;

   if (!tb || !nodes)
      return;

   for (n = nodes; n; )
   {
      _etk_textblock_nodes_clean(tb, n->children);

      delete_node = ETK_FALSE;
      if (n->type == ETK_TEXTBLOCK_NODE_NORMAL && !n->children && etk_string_length_get(n->text) <= 0)
      {
         delete_node = ETK_TRUE;
         /* Delete the nodes that do not contain an iterator */
         for (l = tb->iters; l; l = l->next)
         {
            it = l->data;
            if (it->node == n)
            {
               delete_node = ETK_FALSE;
               break;
            }
         }

         /* Delete the format nodes belonging to an empty line,
          * only if the node doesn't really affect the format of the empty line */
         if (delete_node && _etk_textblock_node_is_empty_line(n, NULL, NULL))
         {
            delete_node  = ETK_FALSE;
            for (p = n; p && p->type != ETK_TEXTBLOCK_NODE_LINE; p = p->parent)
            {
               if (!p->prev)
               {
                  delete_node = ETK_TRUE;
                  break;
               }
            }
         }
      }

      if (delete_node)
         n = _etk_textblock_node_free(n);
      else
         n = n->next;
   }
}

/* Inserts a tag to the textblock (used by etk_textblock_text_markup_insert()) */
static void _etk_textblock_tag_insert(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *tag, int length)
{
   const char *c;
   const char *tag_start, *tag_end;
   const char *name_start, *name_end;
   char *tag_name;
   Etk_Bool closing_tag;
   Etk_Textblock_Tag_Type tag_type;

   if (!tb || !iter || !tag)
      return;
   if (!_etk_textblock_iter_is_valid(tb, iter))
      return;

   if (length < 0)
      length = strlen(tag);

   tag_start = NULL;
   tag_end = NULL;
   for (c = tag; c < tag + length; c++)
   {
      if (!tag_start && *c == '<')
         tag_start = c + 1;
      else if (tag_start && *c == '>')
      {
         tag_end = c - 1;
         break;
      }
   }
   if (!tag_start || !tag_end || tag_end < tag_start)
   {
      ETK_WARNING("Invalid tag format");
      return;
   }

   /* We identify the tag type */
   name_start = tag_start;
   name_end = tag_end;
   for (c = name_start; c < tag_end; c++)
   {
      if (*(c + 1) == ' ')
      {
         name_end = c;
         break;
      }
   }

   if (*name_start == '/')
   {
      closing_tag = ETK_TRUE;
      name_start++;
   }
   else
      closing_tag = ETK_FALSE;

   tag_name = malloc(name_end - name_start + 2);
   strncpy(tag_name, name_start, name_end - name_start + 1);
   tag_name[name_end - name_start + 1] = '\0';

   if (strcasecmp(tag_name, "b") == 0)
      tag_type = ETK_TEXTBLOCK_TAG_BOLD;
   else if (strcasecmp(tag_name, "i") == 0)
      tag_type = ETK_TEXTBLOCK_TAG_ITALIC;
   else if (strcasecmp(tag_name, "u") == 0)
      tag_type = ETK_TEXTBLOCK_TAG_UNDERLINE;
   else if (strcasecmp(tag_name, "s") == 0)
      tag_type = ETK_TEXTBLOCK_TAG_STRIKETHROUGH;
   else if (strcasecmp(tag_name, "p") == 0)
      tag_type = ETK_TEXTBLOCK_TAG_P;
   else if (strcasecmp(tag_name, "style") == 0)
      tag_type = ETK_TEXTBLOCK_TAG_STYLE;
   else if (strcasecmp(tag_name, "font") == 0)
      tag_type = ETK_TEXTBLOCK_TAG_FONT;
   else
   {
      ETK_WARNING("The tag \"%s\" is not supported", tag_name);
      free(tag_name);
      return;
   }
   free(tag_name);

   /* We close the node */
   if (closing_tag)
   {
      if (tag_type == ETK_TEXTBLOCK_TAG_P)
         _etk_textblock_paragraph_add(tb, iter);
      else
         _etk_textblock_node_close(iter, ETK_TEXTBLOCK_NODE_NORMAL, tag_type, NULL);
   }
   /* Or we create the new nodes for the opening tag */
   else
   {
      Etk_Textblock_Node *node;
      Etk_Textblock_Node *new_node;
      const char *param_start = NULL, *param_end = NULL;
      const char *value_start = NULL, *value_end = NULL;
      Etk_Bool equal_met = ETK_FALSE;
      Etk_Bool open_quote = ETK_FALSE;
      int param_len;
      int value_len;

      node = iter->node;

      if (tag_type == ETK_TEXTBLOCK_TAG_P)
      {
         Etk_Textblock_Node *n;

         new_node = NULL;
         for (n = node; n; n = n->parent)
         {
            if (n->type == ETK_TEXTBLOCK_NODE_PARAGRAPH)
            {
               new_node = n;
               break;
            }
            if (n->prev || iter->index > 0)
            {
               new_node = _etk_textblock_paragraph_add(tb, iter);
               break;
            }
         }
         if (!new_node)
            return;
      }
      else
      {
         if (node->type == ETK_TEXTBLOCK_NODE_NORMAL && node->tag.type == ETK_TEXTBLOCK_TAG_DEFAULT)
         {
            if (node->text)
            {
               new_node = _etk_textblock_node_split(tb, node, iter->index, iter->pos);
               _etk_textblock_node_type_set(new_node, ETK_TEXTBLOCK_NODE_NORMAL, tag_type);
            }
            else
            {
               _etk_textblock_node_type_set(node, ETK_TEXTBLOCK_NODE_NORMAL, tag_type);
               new_node = node;
            }
         }
         else
         {
            if (node->text)
            {
               Etk_Textblock_Node *parent_node;

               new_node = _etk_textblock_node_split(tb, node, iter->index, iter->pos);

               parent_node = _etk_textblock_node_new(node->parent, node->prev,
                  ETK_TEXTBLOCK_NODE_NORMAL, ETK_TEXTBLOCK_TAG_DEFAULT);
               _etk_textblock_node_copy(parent_node, node, ETK_FALSE);

               _etk_textblock_node_type_set(node, ETK_TEXTBLOCK_NODE_NORMAL, ETK_TEXTBLOCK_TAG_DEFAULT);
               _etk_textblock_node_attach(node, parent_node, NULL);
               _etk_textblock_node_type_set(new_node, ETK_TEXTBLOCK_NODE_NORMAL, tag_type);
               _etk_textblock_node_attach(new_node, parent_node, node);
            }
            else
            {
               Eina_List *l;
               Etk_Textblock_Iter *it;

               new_node = _etk_textblock_node_new(node, NULL, ETK_TEXTBLOCK_NODE_NORMAL, tag_type);
               for (l = tb->iters; l; l = l->next)
               {
                  it = l->data;
                  if (it->node == node)
                     it->node = new_node;
               }
            }
         }
      }

      /* Finally, we parse the params of the opening tag */
      for (c = name_end + 1; c <= tag_end && *c != '\0'; c++)
      {
         if (!param_start && *c != ' ' && *c != '=')
            param_start = c;
         if (param_start && !param_end && (*(c + 1) == ' ' || *(c + 1) == '='))
         {
            param_end = c;
            continue;
         }
         if (param_start && param_end && !equal_met && *c == '=')
         {
            equal_met = ETK_TRUE;
            continue;
         }
         if (param_start && param_end && equal_met && !value_start)
         {
            if (*c == '\"')
            {
               value_start = c + 1;
               open_quote = ETK_TRUE;
               continue;
            }
            else if (*c != ' ')
            {
               value_start = c;
               continue;
            }
         }
         if (param_start && param_end && value_start)
         {
            if (open_quote && *c == '\"')
               value_end = c - 1;
            else if (!open_quote && *c == ' ')
               value_end = c - 1;
            else if (c == tag_end)
               value_end = c;
         }

         if (param_start && param_end && value_start && value_end)
         {
            param_len = param_end - param_start + 1;
            value_len = value_end - value_start + 1;
            if (param_len > 0 && value_len > 0)
            {
               switch (tag_type)
               {
                  /* Underline */
                  case ETK_TEXTBLOCK_TAG_UNDERLINE:
                     if (ETK_TB_TAG_PARAM_IS("type", 4))
                     {
                        if (ETK_TB_TAG_VALUE_IS("single", 6))
                           new_node->tag.params.u.type = ETK_TEXTBLOCK_UNDERLINE_SINGLE;
                        else if (ETK_TB_TAG_VALUE_IS("double", 6) == 0)
                           new_node->tag.params.u.type = ETK_TEXTBLOCK_UNDERLINE_DOUBLE;
                     }
                     else if (ETK_TB_TAG_PARAM_IS("color1", 6))
                        _etk_textblock_color_parse(value_start, value_len, &new_node->tag.params.u.color1);
                     else if (ETK_TB_TAG_PARAM_IS("color2", 6))
                        _etk_textblock_color_parse(value_start, value_len, &new_node->tag.params.u.color2);
                     break;
                  /* Striketrhough */
                  case ETK_TEXTBLOCK_TAG_STRIKETHROUGH:
                     if (ETK_TB_TAG_PARAM_IS("color", 5))
                        _etk_textblock_color_parse(value_start, value_len, &new_node->tag.params.s.color);
                     break;
                  /* Paragraph */
                  case ETK_TEXTBLOCK_TAG_P:
                     if (ETK_TB_TAG_PARAM_IS("align", 5))
                     {
                        if (ETK_TB_TAG_VALUE_IS("left", 4))
                           new_node->tag.params.p.align = 0.0;
                        else if (ETK_TB_TAG_VALUE_IS("center", 6))
                           new_node->tag.params.p.align = 0.5;
                        else if (ETK_TB_TAG_VALUE_IS("right", 5))
                           new_node->tag.params.p.align = 1.0;
                        else
                           new_node->tag.params.p.align = _etk_textblock_float_parse(value_start, value_len, -1.0);
                     }
                     else if (ETK_TB_TAG_PARAM_IS("left_margin", 11))
                        new_node->tag.params.p.left_margin = _etk_textblock_int_parse(value_start, value_len, 0);
                     else if (ETK_TB_TAG_PARAM_IS("right_margin", 12))
                        new_node->tag.params.p.right_margin = _etk_textblock_int_parse(value_start, value_len, 0);
                     break;
                  /* Style */
                  case ETK_TEXTBLOCK_TAG_STYLE:
                     if (ETK_TB_TAG_PARAM_IS("effect", 6))
                     {
                        if (ETK_TB_TAG_VALUE_IS("none", 4))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_NONE;
                        else if (ETK_TB_TAG_VALUE_IS("outline", 7))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_OUTLINE;
                        else if (ETK_TB_TAG_VALUE_IS("shadow", 6))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_SHADOW;
                        else if (ETK_TB_TAG_VALUE_IS("soft_outline", 12))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_SOFT_OUTLINE;
                        else if (ETK_TB_TAG_VALUE_IS("glow", 4))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_GLOW;
                        else if (ETK_TB_TAG_VALUE_IS("outline_shadow", 14))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_OUTLINE_SHADOW;
                        else if (ETK_TB_TAG_VALUE_IS("far_shadow", 10))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_FAR_SHADOW;
                        else if (ETK_TB_TAG_VALUE_IS("outline_soft_shadow", 19))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_OUTLINE_SOFT_SHADOW;
                        else if (ETK_TB_TAG_VALUE_IS("soft_shadow", 11))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_SOFT_SHADOW;
                        else if (ETK_TB_TAG_VALUE_IS("far_soft_shadow", 15))
                           new_node->tag.params.style.type = ETK_TEXTBLOCK_STYLE_FAR_SOFT_SHADOW;
                     }
                     else if (ETK_TB_TAG_PARAM_IS("color1", 6))
                        _etk_textblock_color_parse(value_start, value_len, &new_node->tag.params.style.color1);
                     else if (ETK_TB_TAG_PARAM_IS("color2", 6))
                        _etk_textblock_color_parse(value_start, value_len, &new_node->tag.params.style.color2);
                     break;
                  /* Font */
                  case ETK_TEXTBLOCK_TAG_FONT:
                     if (ETK_TB_TAG_PARAM_IS("face", 4))
                     {
                        free(new_node->tag.params.font.face);
                        new_node->tag.params.font.face = malloc(value_len + 1);
                        strncpy(new_node->tag.params.font.face, value_start, value_len);
                        new_node->tag.params.font.face[value_len] = '\0';
                     }
                     else if (ETK_TB_TAG_PARAM_IS("size", 4))
                        new_node->tag.params.font.size = _etk_textblock_int_parse(value_start, value_len, -1);
                     else if (ETK_TB_TAG_PARAM_IS("color", 5))
                        _etk_textblock_color_parse(value_start, value_len, &new_node->tag.params.font.color);
                     break;

                  default:
                     break;
               }
            }

            param_start = NULL;
            param_end = NULL;
            value_start = NULL;
            value_end = NULL;
            equal_met = ETK_FALSE;
            open_quote = ETK_FALSE;
         }
      }
   }
}

/* Adds a new paragragh in the textblock, at the iterator's position. Returns the new paragraph node*/
static Etk_Textblock_Node *_etk_textblock_paragraph_add(Etk_Textblock *tb, Etk_Textblock_Iter *iter)
{
   Etk_Textblock_Node *new_paragraph;

   if (!tb || !iter || !_etk_textblock_iter_is_valid(tb, iter))
      return NULL;

   new_paragraph = _etk_textblock_node_new(NULL, NULL, ETK_TEXTBLOCK_NODE_PARAGRAPH, ETK_TEXTBLOCK_TAG_P);
   if (!_etk_textblock_node_close(iter, ETK_TEXTBLOCK_NODE_PARAGRAPH, ETK_TEXTBLOCK_TAG_P, new_paragraph))
   {
      ETK_WARNING("Could not add a new paragraph!\n");
      _etk_textblock_node_free(new_paragraph);
      return NULL;
   }

   return new_paragraph;
}

/* Adds a new line in the textblock, at the iterator's position. Returns the new line node */
static Etk_Textblock_Node *_etk_textblock_line_add(Etk_Textblock *tb, Etk_Textblock_Iter *iter)
{
   Etk_Textblock_Node *prev_node, *new_line;
   Eina_List *l;

   if (!tb || !iter || !_etk_textblock_iter_is_valid(tb, iter))
      return NULL;

   _etk_textblock_node_update(tb, iter->node);

   prev_node = iter->node;
   new_line = _etk_textblock_node_new(NULL, NULL, ETK_TEXTBLOCK_NODE_LINE, ETK_TEXTBLOCK_TAG_DEFAULT);
   if (!_etk_textblock_node_close(iter, ETK_TEXTBLOCK_NODE_LINE, ETK_TEXTBLOCK_TAG_DEFAULT, new_line))
   {
      ETK_WARNING("Could not add a new line!\n");
      _etk_textblock_node_free(new_line);
      return NULL;
   }

   for (l = tb->evas_objects; l; l = l->next)
      _etk_textblock_object_line_add(l->data, new_line);

   return new_line;
}

/* Deletes the line in the textblock, corresponding to the node "line_node". Returns the next line node */
static Etk_Textblock_Node *_etk_textblock_line_del(Etk_Textblock *tb, Etk_Textblock_Node *line_node)
{
   Etk_Textblock_Node *next_line;
   Eina_List *l;

   if (!tb || !line_node || line_node->type != ETK_TEXTBLOCK_NODE_LINE)
      return NULL;

   for (l = tb->evas_objects; l; l = l->next)
      _etk_textblock_object_line_del(l->data, line_node);

   next_line = _etk_textblock_next_line_get(line_node);
   _etk_textblock_node_free(line_node);

   return next_line;
}

/* Gets recursively the text of the node */
static void _etk_textblock_node_text_get(Etk_Textblock_Node *node, Etk_Bool markup, Etk_String *text, Etk_Textblock_Iter *start_iter, Etk_Textblock_Iter *end_iter)
{
   Etk_String *start_tag = NULL;
   Etk_String *end_tag = NULL;
   int text_len;
   Etk_Bool end_of_line = ETK_TRUE;

   if (!node || !text)
      return;

   if (markup)
   {
      /* Gets the start/end tags of the node */
      switch (node->tag.type)
      {
         case ETK_TEXTBLOCK_TAG_BOLD:
            start_tag = etk_string_new("<b>");
            end_tag = etk_string_new("</b>");
            break;
         case ETK_TEXTBLOCK_TAG_ITALIC:
            start_tag = etk_string_new("<i>");
            end_tag = etk_string_new("</i>");
            break;
         case ETK_TEXTBLOCK_TAG_UNDERLINE:
            start_tag = etk_string_new("<u>");
            end_tag = etk_string_new("</u>");

            if (node->tag.params.u.color2.r >= 0)
            {
               etk_string_insert_printf(start_tag, 2, " color2=#%.2X%.2X%.2X%.2X", node->tag.params.u.color2.r,
                  node->tag.params.u.color2.g, node->tag.params.u.color2.b, node->tag.params.u.color2.a);
            }
            if (node->tag.params.u.color1.r >= 0)
            {
               etk_string_insert_printf(start_tag, 2, " color1=#%.2X%.2X%.2X%.2X", node->tag.params.u.color1.r,
                  node->tag.params.u.color1.g, node->tag.params.u.color1.b, node->tag.params.u.color1.a);
            }
            if (node->tag.params.u.type == ETK_TEXTBLOCK_UNDERLINE_DOUBLE)
               etk_string_insert(start_tag, 2, " type=\"double\"");
            break;
         case ETK_TEXTBLOCK_TAG_STRIKETHROUGH:
            start_tag = etk_string_new("<s>");
            end_tag = etk_string_new("</s>");

            if (node->tag.params.s.color.r >= 0)
            {
               etk_string_insert_printf(start_tag, 2, " color=#%.2X%.2X%.2X%.2X", node->tag.params.s.color.r,
                  node->tag.params.s.color.g, node->tag.params.s.color.b, node->tag.params.s.color.a);
            }
            break;
         case ETK_TEXTBLOCK_TAG_P:
            if (_etk_textblock_node_is_default_paragraph(node))
               break;

            start_tag = etk_string_new("<p>");
            end_tag = etk_string_new("</p>");

            if (node->tag.params.p.right_margin != 0)
               etk_string_insert_printf(start_tag, 2, " right_margin=%d", node->tag.params.p.right_margin);
            if (node->tag.params.p.left_margin != 0)
               etk_string_insert_printf(start_tag, 2, " left_margin=%d", node->tag.params.p.left_margin);
            if (node->tag.params.p.align >= 0.0)
            {
               if (node->tag.params.p.align == 0.0)
                  etk_string_insert(start_tag, 2, " align=\"left\"");
               else if (node->tag.params.p.align == 0.5)
                  etk_string_insert(start_tag, 2, " align=\"center\"");
               else if (node->tag.params.p.align == 1.0)
                  etk_string_insert(start_tag, 2, " align=\"right\"");
               else
                  etk_string_insert_printf(start_tag, 2, " align=%.2f", node->tag.params.p.align);
            }
            break;
         case ETK_TEXTBLOCK_TAG_STYLE:
            start_tag = etk_string_new("<style>");
            end_tag = etk_string_new("</style>");

            if (node->tag.params.style.color2.r >= 0)
            {
               etk_string_insert_printf(start_tag, 6, " color2=#%.2X%.2X%.2X%.2X", node->tag.params.style.color2.r,
                  node->tag.params.style.color2.g, node->tag.params.style.color2.b, node->tag.params.style.color2.a);
            }
            if (node->tag.params.style.color1.r >= 0)
            {
               etk_string_insert_printf(start_tag, 6, " color1=#%.2X%.2X%.2X%.2X", node->tag.params.style.color1.r,
                  node->tag.params.style.color1.g, node->tag.params.style.color1.b, node->tag.params.style.color1.a);
            }
            switch (node->tag.params.style.type)
            {
               case ETK_TEXTBLOCK_STYLE_NONE:
                  etk_string_insert(start_tag, 6, " effect=\"none\"");
                  break;
               case ETK_TEXTBLOCK_STYLE_OUTLINE:
                  etk_string_insert(start_tag, 6, " effect=\"outline\"");
                  break;
               case ETK_TEXTBLOCK_STYLE_SHADOW:
                  etk_string_insert(start_tag, 6, " effect=\"shadow\"");
                  break;
               default:
                  break;
            }
            break;
         case ETK_TEXTBLOCK_TAG_FONT:
            start_tag = etk_string_new("<font>");
            end_tag = etk_string_new("</font>");

            if (node->tag.params.font.color.r >= 0)
            {
               etk_string_insert_printf(start_tag, 5, " color=#%.2X%.2X%.2X%.2X", node->tag.params.font.color.r,
                  node->tag.params.font.color.g, node->tag.params.font.color.b, node->tag.params.font.color.a);
            }
            if (node->tag.params.font.size >= 0)
               etk_string_insert_printf(start_tag, 5, " size=\"%d\"", node->tag.params.font.size);
            if (node->tag.params.font.face)
               etk_string_insert_printf(start_tag, 5, " face=\"%s\"", node->tag.params.font.face);
            break;
         default:
            break;
      }
   }

   /* TODO: escape code for < and > and & in markup mode!! */
   /* Builds the text of the node */
   etk_string_append(text, etk_string_get(start_tag));

   /* Add the text of the node */
   text_len = etk_string_length_get(node->text);
   if (text_len > 0)
   {
      const char *node_text;
      int start_pos;
      int len;

      node_text = etk_string_get(node->text);
      start_pos = (start_iter && node == start_iter->node) ? start_iter->index : 0;
      start_pos = ETK_MIN(start_pos, text_len - 1);
      if (end_iter && node == end_iter->node)
      {
         len = end_iter->pos - start_pos;
         len = ETK_MIN(len, text_len - start_pos);
         etk_string_append_sized(text, &node_text[start_pos], len);
      }
      else
         etk_string_append(text, &node_text[start_pos]);
   }

   /* Adds recursively the text of the children */
   if (node->children)
   {
      Eina_List *node_hierarchy;
      Etk_Textblock_Node *start_node, *end_node;
      Etk_Textblock_Node *n;
      int depth;
      Etk_Bool add_child;
      Etk_Bool found;

      depth = 0;
      for (n = node; n; n = n->parent)
         depth++;

      if (!start_iter)
         start_node = NULL;
      else
      {
         node_hierarchy = NULL;
         found = ETK_FALSE;
         for (n = start_iter->node; n; n = n->parent)
         {
            node_hierarchy = eina_list_prepend(node_hierarchy, n);
            if (node == n)
               found = ETK_TRUE;
         }
         start_node = found ? eina_list_nth(node_hierarchy, depth) : NULL;
         eina_list_free(node_hierarchy);
      }

      if (!end_iter)
         end_node = NULL;
      else
      {
         node_hierarchy = NULL;
         found = ETK_FALSE;
         for (n = end_iter->node; n; n = n->parent)
         {
            node_hierarchy = eina_list_prepend(node_hierarchy, n);
            if (node == n)
            {
               end_of_line = ETK_FALSE;
               found = ETK_TRUE;
            }
         }
         end_node = found ? eina_list_nth(node_hierarchy, depth) : NULL;
         eina_list_free(node_hierarchy);
      }

      add_child = (start_node == NULL);
      for (n = node->children; n; n = n->next)
      {
         if (n == start_node)
            add_child = ETK_TRUE;
         if (add_child)
            _etk_textblock_node_text_get(n, markup, text, start_iter, end_iter);
         if (n == end_node)
            add_child = ETK_FALSE;
      }
   }

   etk_string_append(text, etk_string_get(end_tag));

   if (node->type == ETK_TEXTBLOCK_NODE_LINE && _etk_textblock_next_line_get(node) && end_of_line)
      etk_string_append_char(text, '\n');

   etk_object_destroy(ETK_OBJECT(start_tag));
   etk_object_destroy(ETK_OBJECT(end_tag));
}

/* Copies the content of the node "src" to the node "dest" */
static void _etk_textblock_node_copy(Etk_Textblock_Node *dest, const Etk_Textblock_Node *src, Etk_Bool copy_text)
{
   if (!dest || !src)
      return;

   if (copy_text)
   {
      dest->text = etk_string_set_sized(dest->text, etk_string_get(src->text), etk_string_length_get(src->text));
      dest->unicode_length = src->unicode_length;
   }

   dest->type = src->type;
   dest->tag = src->tag;
   if (dest->tag.type == ETK_TEXTBLOCK_TAG_FONT && dest->tag.params.font.face)
      dest->tag.params.font.face = strdup(dest->tag.params.font.face);
}

/* Compares the two nodes. */
/* TODO: OPTIMIZE this!!! Use a binary tree (AVL?) for the lines! */
static int _etk_textblock_node_compare(Etk_Textblock_Node *node1, Etk_Textblock_Node *node2)
{
   Etk_Textblock_Node *p1, *p2;
   Etk_Textblock_Node *c1, *c2;
   Etk_Textblock_Node *children = NULL, *n;

   if (!node1 || !node2)
      return -1;
   if (node1 == node2)
      return 0;

   /* We search the common parent node */
   c1 = node1;
   c2 = node2;
   for (p1 = node1->parent; p1; p1 = p1->parent)
   {
      c2 = node2;
      for (p2 = node2->parent; p2; p2 = p2->parent)
      {
         if (p1 == p2)
         {
            children = p1->children;
            break;
         }
         c2 = p2;
      }

      if (children)
         break;
      c1 = p1;
   }

   /* Then we compare the positions of the nodes in the list of children of the common parent */
   for (n = children; n; n = n->next)
   {
      if (n == c1 && n != c2)
         return -1;
      else if (n != c1 && n == c2)
         return 1;
      else if (n == c1 && n == c2)
         return 0;
   }

   return -1;
}

/* Splits the node at "index". It returns the right node (the left node being "node")
 * "pos" is the unicode position where to split the node, it *has* to be in accord with "index".
 * If you don't know the unicode position, use -1 */
static Etk_Textblock_Node *_etk_textblock_node_split(Etk_Textblock *tb, Etk_Textblock_Node *node, int index, int pos)
{
   const char *node_text;
   int node_len;
   Etk_Textblock_Node *rnode;
   Etk_Textblock_Iter *it;
   Eina_List *l;

   if (!node)
      return NULL;

   node_text = etk_string_get(node->text);
   node_len = etk_string_length_get(node->text);
   index = ETK_CLAMP(index, 0, node_len);

   if (pos < 0 || pos > node->unicode_length)
   {
      int i;

      /* We calculate the unicode split position */
      for (pos = 0, i = 0; i < index; i++)
         pos = evas_string_char_next_get(node_text, pos, NULL);
   }

   rnode = _etk_textblock_node_new(node->parent, node, node->type, node->tag.type);

   rnode->tag.params = node->tag.params;
   if (rnode->tag.type == ETK_TEXTBLOCK_TAG_FONT && node->tag.params.font.face)
      rnode->tag.params.font.face = strdup(node->tag.params.font.face);

   rnode->text = etk_string_new_sized(&node_text[index], node_len - index);
   rnode->unicode_length = node->unicode_length - pos;

   etk_string_truncate(node->text, index);
   node->unicode_length = pos;

   /* We update the iterators */
   if (tb)
   {
      for (l = tb->iters; l; l = l->next)
      {
         it = l->data;
         if (it->node == node)
         {
            if (it->index > index || (it->index == index && it->gravity == ETK_TEXTBLOCK_GRAVITY_RIGHT))
            {
               it->node = rnode;
               it->index -= index;
               it->pos -= pos;
            }
         }
      }
   }

   return rnode;
}

/* Closes a parent node of the node attached to "iter" whose type is "node_type" and tag type is "tag_type" */
static Etk_Bool _etk_textblock_node_close(Etk_Textblock_Iter *iter, Etk_Textblock_Node_Type node_type, Etk_Textblock_Tag_Type tag_type, Etk_Textblock_Node *replace_node)
{
   Etk_Textblock_Node *node, *right_node;

   if (!iter || !_etk_textblock_iter_is_valid(NULL, iter))
      return ETK_FALSE;

   node = iter->node;

   if (node->type == node_type && node->tag.type == tag_type)
   {
      right_node = _etk_textblock_node_split(iter->tb, node, iter->index, iter->pos);
      _etk_textblock_node_type_set(right_node, ETK_TEXTBLOCK_NODE_NORMAL, ETK_TEXTBLOCK_TAG_DEFAULT);

      if (replace_node)
      {
         _etk_textblock_node_attach(replace_node, node->parent, node);
         _etk_textblock_node_attach(right_node, replace_node, NULL);
      }

      return ETK_TRUE;
   }
   else
   {
      Etk_Textblock_Node *node_to_close, *right_node, *left_node;
      Etk_Textblock_Node *new_node, *parent, *n;
      Eina_List *node_hierarchy;

      node_to_close = NULL;
      node_hierarchy = NULL;
      /* We search the node to close and we save the node hierarchy */
      for (n = node->parent; n; n = n->parent)
      {
         if (n->type == node_type && n->tag.type == tag_type)
         {
            node_to_close = n;
            break;
         }
         node_hierarchy = eina_list_prepend(node_hierarchy, n);
      }
      if (!node_to_close)
      {
         ETK_WARNING("There is no node to close");
         eina_list_free(node_hierarchy);
         return ETK_FALSE;
      }

      /* Then, we attach a new node next to the node to close */
      if (replace_node)
      {
         _etk_textblock_node_attach(replace_node, node_to_close->parent, node_to_close);
         parent = replace_node;
         left_node = NULL;
      }
      else
      {
         parent = node_to_close->parent;
         left_node = node_to_close;
      }

      /* And we create new nodes to rebuild the node hierarchy */
      while (node_hierarchy)
      {
         n = node_hierarchy->data;

         new_node = _etk_textblock_node_new(parent, left_node, ETK_TEXTBLOCK_NODE_NORMAL, ETK_TEXTBLOCK_TAG_DEFAULT);
         _etk_textblock_node_copy(new_node, n, ETK_FALSE);
         _etk_textblock_node_attach(n->next, parent, new_node);
         parent = new_node;
         left_node = NULL;

         node_hierarchy = eina_list_remove_list(node_hierarchy, node_hierarchy);
      }

      /* Finally, we split the original node, and we attach the right nodes to the new node hierarchy */
      right_node = _etk_textblock_node_split(iter->tb, node, iter->index, iter->pos);
      while (right_node)
      {
         node = right_node->next;
         _etk_textblock_node_attach(right_node, parent, left_node);
         left_node = right_node;
         right_node = node;
      }

      return ETK_TRUE;
   }
}

/* Checks if the iterator is valid */
static Etk_Bool _etk_textblock_iter_is_valid(Etk_Textblock *tb, Etk_Textblock_Iter *iter)
{
   if (!iter)
      return ETK_TRUE;

   if (tb && iter->tb != tb)
   {
      ETK_WARNING("The iterator does not belong to the textblock");
      return ETK_FALSE;
   }
   if (!iter->tb)
   {
      ETK_WARNING("The iterator is not attached to a textblock");
      return ETK_FALSE;
   }
   else if (!iter->node)
   {
      ETK_WARNING("The iterator is not attached to a textblock node");
      return ETK_FALSE;
   }
   else if (iter->node->type == ETK_TEXTBLOCK_NODE_ROOT)
   {
      ETK_WARNING("The iterator can't be attached to the root node");
      return ETK_FALSE;
   }
   else if (iter->node->type == ETK_TEXTBLOCK_NODE_PARAGRAPH)
   {
      ETK_WARNING("The iterator can't be attached to a paragraph node");
      return ETK_FALSE;
   }
   else if (iter->node->children)
   {
      ETK_WARNING("The node of the iterator is not a leaf");
      return ETK_FALSE;
   }
   else if (iter->pos < 0 || iter->pos > iter->node->unicode_length
      || iter->index < 0 || iter->index > etk_string_length_get(iter->node->text))
   {
      ETK_WARNING("The pos or the index of the iterator are incorrect");
      return ETK_FALSE;
   }

   return ETK_TRUE;
}

/* Checks if the node has the default params of the paragraph tag */
static Etk_Bool _etk_textblock_node_is_default_paragraph(Etk_Textblock_Node *node)
{
   if (!node)
      return ETK_TRUE;
   if (node->tag.type != ETK_TEXTBLOCK_TAG_P)
      return ETK_FALSE;

   if (node->tag.params.p.align >= 0.0)
      return ETK_FALSE;
   if (node->tag.params.p.left_margin != 0)
      return ETK_FALSE;
   if (node->tag.params.p.right_margin != 0)
      return ETK_FALSE;
   if (node->tag.params.p.wrap != ETK_TEXTBLOCK_WRAP_DEFAULT)
      return ETK_FALSE;

   return ETK_TRUE;
}

/* Checks if the node belongs to an empty line (i.e. a line containing no text, but which may contain empty format nodes)
 * If the line is empty, the format of the line is stored in "face", "size" */
/* TODO: return also the style?? (effect, italic, bold, ...) */
static Etk_Bool _etk_textblock_node_is_empty_line(Etk_Textblock_Node *node, const char **face, int *size)
{
   Etk_Textblock_Node *line, *n;
   Etk_Textblock_Format format;

   for (n = node; n; n = n->parent)
   {
      if (n->type == ETK_TEXTBLOCK_NODE_LINE)
         break;
   }
   if (!(line = n))
      return ETK_FALSE;

   if (_etk_textblock_node_is_empty(line))
   {
      for (n = line; n->children; n = n->children);
      _etk_textblock_node_format_get(n, &format);
      if (face)
         *face = format.font_face;
      if (size)
         *size = format.font_size;

      return ETK_TRUE;
   }
   else
      return ETK_FALSE;
}

/* Checks recursively whether the node and its children contain some text. If not, it returns ETK_TRUE */
static Etk_Bool _etk_textblock_node_is_empty(Etk_Textblock_Node *node)
{
   Etk_Textblock_Node *n;

   if (!node)
      return ETK_FALSE;

   if (node->unicode_length > 0)
      return ETK_FALSE;

   for (n = node->children; n; n = n->next)
   {
      if (!_etk_textblock_node_is_empty(n))
         return ETK_FALSE;
   }
   return ETK_TRUE;
}

/* Parses an integer and returns 'error_value' if it fails */
static int _etk_textblock_int_parse(const char *int_string, int length, int error_value)
{
   char *str;
   int value;

   if (length <= 0)
      return error_value;

   str = malloc(length + 1);
   strncpy(str, int_string, length);
   str[length] = '\0';
   if (sscanf(str, "%d", &value) != 1)
      value = error_value;
   free(str);

   return value;
}

/* Parses a float and returns 'error_value' if it fails */
static float _etk_textblock_float_parse(const char *float_string, int length, float error_value)
{
   char *str;
   float value;

   if (length <= 0)
      return error_value;

   str = malloc(length + 1);
   strncpy(str, float_string, length);
   str[length] = '\0';
   if (sscanf(str, "%f", &value) != 1)
      value = error_value;
   free(str);

   return value;
}

/* Parses a color and stores the result in 'color' */
/* Imported from Evas */
/* TODO: _etk_textblock_color_parse(): support common color names ("red", "blue", ...) */
static void _etk_textblock_color_parse(const char *color_string, int length, Etk_Color *color)
{
   const char *str;

   if (!(str = color_string))
      return;

   /* #RRGGBB */
   if (length == 7)
   {
      color->r = (_etk_textblock_hex_string_get(str[1]) << 4) | (_etk_textblock_hex_string_get(str[2]));
      color->g = (_etk_textblock_hex_string_get(str[3]) << 4) | (_etk_textblock_hex_string_get(str[4]));
      color->b = (_etk_textblock_hex_string_get(str[5]) << 4) | (_etk_textblock_hex_string_get(str[6]));
      color->a = 0xff;
   }
   /* #RRGGBBAA */
   else if (length == 9)
   {
      color->r = (_etk_textblock_hex_string_get(str[1]) << 4) | (_etk_textblock_hex_string_get(str[2]));
      color->g = (_etk_textblock_hex_string_get(str[3]) << 4) | (_etk_textblock_hex_string_get(str[4]));
      color->b = (_etk_textblock_hex_string_get(str[5]) << 4) | (_etk_textblock_hex_string_get(str[6]));
      color->a = (_etk_textblock_hex_string_get(str[7]) << 4) | (_etk_textblock_hex_string_get(str[8]));
   }
   /* #RGB */
   else if (length == 4)
   {
      color->r = _etk_textblock_hex_string_get(str[1]);
      color->r = (color->r << 4) | color->r;
      color->g = _etk_textblock_hex_string_get(str[2]);
      color->g = (color->g << 4) | color->g;
      color->b = _etk_textblock_hex_string_get(str[3]);
      color->b = (color->b << 4) | color->b;
      color->a = 0xff;
   }
   /* #RGBA */
   else if (length == 5)
   {
      color->r = _etk_textblock_hex_string_get(str[1]);
      color->r = (color->r << 4) | color->r;
      color->g = _etk_textblock_hex_string_get(str[2]);
      color->g = (color->g << 4) | color->g;
      color->b = _etk_textblock_hex_string_get(str[3]);
      color->b = (color->b << 4) | color->b;
      color->a = _etk_textblock_hex_string_get(str[4]);
      color->a = (color->a << 4) | color->a;
   }
   else
      color->r = -1;
}

/* Get the utf-8 char associated to the escape code */
/* TODO: support more escape codes? (need to use a hash table if we do so) */
static const char *_etk_textblock_escape_parse(const char *escape, int len)
{
   int i;
   int num_escapes;

   if (!escape)
      return NULL;

   num_escapes = sizeof(_etk_tb_escapes) / sizeof(char *);
   for (i = 0; i < num_escapes; i += 2)
   {
      if (strncmp(escape, _etk_tb_escapes[i], len) == 0)
         return _etk_tb_escapes[i + 1];
   }
   return NULL;
}

/* Gets the decimal value of the hexadecimal figure */
/* Imported from Evas */
static int _etk_textblock_hex_string_get(char ch)
{
   if ((ch >= '0') && (ch <= '9'))
      return (ch - '0');
   else if ((ch >= 'A') && (ch <= 'F'))
      return (ch - 'A' + 10);
   else if ((ch >= 'a') && (ch <= 'f'))
      return (ch - 'a' + 10);

   return 0;
}

/* Gets the node just before "node" */
static Etk_Textblock_Node *_etk_textblock_prev_node_get(Etk_Textblock_Node *node)
{
   Etk_Textblock_Node *n;

   if (!node)
      return NULL;

   for (n = node; n; n = n->parent)
   {
      if (n->prev)
      {
         n = n->prev;
         break;
      }
   }
   if (!n)
      return NULL;

   while (n->last_child)
      n = n->last_child;

   return n;
}

/* Gets the node just after "node" */
static Etk_Textblock_Node *_etk_textblock_next_node_get(Etk_Textblock_Node *node)
{
   Etk_Textblock_Node *n;

   if (!node)
      return NULL;

   for (n = node; n; n = n->parent)
   {
      if (n->next)
      {
         n = n->next;
         break;
      }
   }
   if (!n)
      return NULL;

   while (n->children)
      n = n->children;

   return n;
}

/* Gets the text node just before "node" */
static Etk_Textblock_Node *_etk_textblock_prev_text_node_get(Etk_Textblock_Node *node, Etk_Bool ignore_empty_lines, Etk_Bool *line_has_changed)
{
   Etk_Textblock_Node *prev;
   Etk_Textblock_Node *current_line, *new_line;

   if (line_has_changed)
      *line_has_changed = ETK_FALSE;

   if (!node)
      return NULL;

   for (current_line = node; current_line; current_line = current_line->parent)
   {
      if (current_line->type == ETK_TEXTBLOCK_NODE_LINE)
         break;
   }

   for (prev = _etk_textblock_prev_node_get(node); prev; prev = _etk_textblock_prev_node_get(prev))
   {
      for (new_line = prev; new_line; new_line = new_line->parent)
      {
         if (new_line->type == ETK_TEXTBLOCK_NODE_LINE)
            break;
      }

      if (new_line != current_line && !ignore_empty_lines && _etk_textblock_node_is_empty(new_line))
      {
         for (prev = new_line; prev->children; prev = prev->children);
         if (line_has_changed)
            *line_has_changed = ETK_TRUE;
         return prev;
      }

      if (etk_string_length_get(prev->text) > 0)
      {
         if (line_has_changed && new_line != current_line)
            *line_has_changed = ETK_TRUE;
         return prev;
      }
   }

   return NULL;
}

/* Gets the text node just after "node" */
static Etk_Textblock_Node *_etk_textblock_next_text_node_get(Etk_Textblock_Node *node, Etk_Bool ignore_empty_lines, Etk_Bool *line_has_changed)
{
   Etk_Textblock_Node *next;
   Etk_Textblock_Node *current_line, *new_line;

   if (line_has_changed)
      *line_has_changed = ETK_FALSE;

   if (!node)
      return NULL;

   for (current_line = node; current_line; current_line = current_line->parent)
   {
      if (current_line->type == ETK_TEXTBLOCK_NODE_LINE)
         break;
   }

   for (next = _etk_textblock_next_node_get(node); next; next = _etk_textblock_next_node_get(next))
   {
      for (new_line = next; new_line; new_line = new_line->parent)
      {
         if (new_line->type == ETK_TEXTBLOCK_NODE_LINE)
            break;
      }

      if (new_line != current_line && !ignore_empty_lines && _etk_textblock_node_is_empty(new_line))
      {
         for (next = new_line; next->children; next = next->children);
         if (line_has_changed)
            *line_has_changed = ETK_TRUE;
         return next;
      }

      if (etk_string_length_get(next->text) > 0)
      {
         if (line_has_changed && new_line != current_line)
            *line_has_changed = ETK_TRUE;
         return next;
      }
   }

   return NULL;
}

/* Gets the node of the line before "line_node" */
static Etk_Textblock_Node *_etk_textblock_prev_line_get(Etk_Textblock_Node *line_node)
{
   if (!line_node || line_node->type != ETK_TEXTBLOCK_NODE_LINE)
      return NULL;

   if (line_node->prev)
      return line_node->prev;
   else
   {
      Etk_Textblock_Node *paragraph;

      for (paragraph = line_node->parent->prev; paragraph; paragraph = paragraph->prev)
      {
         if (paragraph->last_child)
            return paragraph->last_child;
      }
      return NULL;
   }
}

/* Gets the node of the line after "line_node" */
static Etk_Textblock_Node *_etk_textblock_next_line_get(Etk_Textblock_Node *line_node)
{
   if (!line_node || line_node->type != ETK_TEXTBLOCK_NODE_LINE)
      return NULL;

   if (line_node->next)
      return line_node->next;
   else
   {
      Etk_Textblock_Node *paragraph;

      for (paragraph = line_node->parent->next; paragraph; paragraph = paragraph->next)
      {
         if (paragraph->children)
            return paragraph->children;
      }
      return NULL;
   }
}

/* Queues an update of the textblock object that uses "iter" as its cursor or selection bound */
static void _etk_textblock_iter_update(Etk_Textblock_Iter *iter)
{
   Eina_List *l;
   Evas_Object *tbo;
   Etk_Textblock_Object_SD *tbo_sd;

   if (!iter || !iter->tb)
      return;

   for (l = iter->tb->evas_objects; l; l = l->next)
   {
      tbo = l->data;
      if (!(tbo_sd = evas_object_smart_data_get(tbo)))
         continue;

      if (iter == tbo_sd->cursor || iter == tbo_sd->selection)
      {
         _etk_textblock_object_cursor_update_queue(tbo);

         if (iter == tbo_sd->cursor && tbo_sd->cursor_timer)
         {
            evas_object_show(tbo_sd->cursor_object);
            ecore_timer_interval_set(tbo_sd->cursor_timer, ETK_TB_OBJECT_SHOW_CURSOR_DELAY);
         }

         break;
      }
   }
}

/* Queues an update of the node "node" in all the textblock objects (to call when the content of a node has changed) */
static void _etk_textblock_node_update(Etk_Textblock *tb, Etk_Textblock_Node *node)
{
   Eina_List *l;
   Evas_Object *tbo;
   Etk_Textblock_Object_Line *line;

   if (!tb || !node)
      return;

   for (l = tb->evas_objects; l; l = l->next)
   {
      tbo = l->data;
      line = _etk_textblock_object_line_get_from_node(tbo, node);
      _etk_textblock_object_line_update_queue(tbo, line, ETK_TRUE, ETK_TRUE);
   }
}

/**************************
 *
 * Textblock object's rendering
 *
 **************************/

/* Adds a new line to a the textblock object, associated to a line node */
static void _etk_textblock_object_line_add(Evas_Object *tbo, Etk_Textblock_Node *line_node)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *prev_line, *new_line;
   Etk_Textblock_Node *prev;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   new_line = malloc(sizeof(Etk_Textblock_Object_Line));
   new_line->prev = NULL;
   new_line->next = NULL;
   new_line->node = line_node;
   new_line->geometry.x = 0;
   new_line->geometry.y = 0;
   new_line->geometry.w = 0;
   new_line->geometry.h = 0;
   new_line->object = NULL;
   new_line->need_geometry_update = ETK_FALSE;
   new_line->need_content_update = ETK_FALSE;

   if (!(prev = _etk_textblock_prev_line_get(line_node)) || !tbo_sd->lines)
   {
      new_line->next = tbo_sd->lines;
      if (tbo_sd->lines)
         tbo_sd->lines->prev = new_line;
      tbo_sd->lines = new_line;
      if (!tbo_sd->last_line)
         tbo_sd->last_line = new_line;
   }
   else if (prev == tbo_sd->last_line->node)
   {
      new_line->prev = tbo_sd->last_line;
      tbo_sd->last_line->next = new_line;
      tbo_sd->last_line = new_line;
   }
   else if ((prev_line = _etk_textblock_object_line_get_from_node(tbo, prev)))
   {
      new_line->prev = prev_line;
      new_line->next = prev_line->next;
      prev_line->next->prev = new_line;
      prev_line->next = new_line;
   }
   else
   {
      ETK_WARNING("Could not add a line the textblock object");
      free(new_line);
      return;
   }

   _etk_textblock_object_line_update_queue(tbo, new_line, ETK_TRUE, ETK_TRUE);
}

/* Deletes the line of the textblock object, according to a line node */
static void _etk_textblock_object_line_del(Evas_Object *tbo, Etk_Textblock_Node *line_node)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;
   if (!(line = _etk_textblock_object_line_get_from_node(tbo, line_node)))
      return;

   if (line->prev)
      line->prev->next = line->next;
   if (line->next)
      line->next->prev = line->prev;
   if (tbo_sd->lines == line)
      tbo_sd->lines = line->next;
   if (tbo_sd->last_line == line)
      tbo_sd->last_line = line->prev;
   /* TODO: update first_visible and last_visible too? */

   if (line->object)
      evas_object_del(line->object);
   free(line);

   _etk_textblock_object_update_queue(tbo);
}

/* Fills recursively the line of the textblock object with the content of a node */
static void _etk_textblock_object_line_fill(Evas_Object *tbo, Evas_Textblock_Cursor *cur, Etk_Textblock_Node *node)
{
   Etk_String *fmt_str = NULL;
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Node *n;
   int opened_nodes = 0, i;

   if (!tbo || !cur || !node || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   /* A line node */
   if (node->type == ETK_TEXTBLOCK_NODE_LINE)
   {
      Etk_Textblock_Node *paragraph;

      if ((paragraph = node->parent) && paragraph->type == ETK_TEXTBLOCK_NODE_PARAGRAPH)
      {
         Etk_Textblock_Wrap wrap;

         /* Wrapping */
         if ((wrap = paragraph->tag.params.p.wrap) == ETK_TEXTBLOCK_WRAP_DEFAULT)
            wrap = tbo_sd->wrap;
         if (wrap == ETK_TEXTBLOCK_WRAP_WORD)
            evas_textblock_cursor_format_append(cur, "+ wrap=word");
         else if (wrap == ETK_TEXTBLOCK_WRAP_CHAR)
            evas_textblock_cursor_format_append(cur, "+ wrap=char");
         else
            evas_textblock_cursor_format_append(cur, "+ wrap=none");
         opened_nodes++;

         /* Alignment */
         if (paragraph->tag.params.p.align == 0.5)
            evas_textblock_cursor_format_append(cur, "+ align=center");
         else if (paragraph->tag.params.p.align == 1.0)
            evas_textblock_cursor_format_append(cur, "+ align=right");
         else
         {
            fmt_str = etk_string_set_printf(fmt_str, "+ align=%d%%", (int)paragraph->tag.params.p.align * 100);
            evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
         }
         opened_nodes++;

         /* Margins */
         if (paragraph->tag.params.p.left_margin > 0)
         {
            fmt_str = etk_string_set_printf(fmt_str, "+ left_margin=+%d", paragraph->tag.params.p.left_margin);
            evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
            opened_nodes++;
         }
         if (paragraph->tag.params.p.right_margin > 0)
         {
            fmt_str = etk_string_set_printf(fmt_str, "+ right_margin=+%d", paragraph->tag.params.p.right_margin);
            evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
            opened_nodes++;
         }
      }
   }
   /* A normal node */
   else if (node->type == ETK_TEXTBLOCK_NODE_NORMAL && etk_string_length_get(node->text) > 0)
   {
      Etk_Textblock_Format format;
      Etk_Color color;
      /* TODO: use default color here */
      Etk_Color black = { .r = 0, .g = 0, .b = 0, .a = 255 };

      /* We add the format nodes */
      /* TODO: we need to make the font changeable */
      _etk_textblock_node_format_get(node, &format);

      /* Font styles */
      opened_nodes++;
      if (format.bold && !format.italic)
         evas_textblock_cursor_format_append(cur, "+ font=Vera-Bold");
      else if (!format.bold && format.italic)
         evas_textblock_cursor_format_append(cur, "+ font=Vera-Italic");
      else if (format.bold && format.italic)
         evas_textblock_cursor_format_append(cur, "+ font=Vera-Bold-Italic");
      else
         opened_nodes--;

      /* Underline */
      if (format.underline != ETK_TEXTBLOCK_UNDERLINE_NONE)
      {
         if (format.underline == ETK_TEXTBLOCK_UNDERLINE_SINGLE)
         {
            evas_textblock_cursor_format_append(cur, "+ underline=single");
            opened_nodes++;
         }
         else if (format.underline == ETK_TEXTBLOCK_UNDERLINE_DOUBLE)
         {
            evas_textblock_cursor_format_append(cur, "+ underline=double");
            opened_nodes++;
         }

         /* First color */
         if (format.underline_color1.r >= 0)
            color = format.underline_color1;
         else if (format.font_color.r >= 0)
            color = format.font_color;
         else
            color = black;
         fmt_str = etk_string_set_printf(fmt_str, "+ underline_color=#%.2X%.2X%.2X%.2X",
            color.r, color.g, color.b, color.a);
         evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
         opened_nodes++;

         /* Second color */
         if (format.underline == ETK_TEXTBLOCK_UNDERLINE_DOUBLE)
         {
            if (format.underline_color2.r >= 0)
               color = format.underline_color2;
            fmt_str = etk_string_set_printf(fmt_str, "+ underline_color2=#%.2X%.2X%.2X%.2X",
               color.r, color.g, color.b, color.a);
            evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
            opened_nodes++;
         }
      }

      /* Strikethrough */
      if (format.strikethrough)
      {
         evas_textblock_cursor_format_append(cur, "+ strikethrough=on");
         opened_nodes++;

         if (format.strikethrough_color.r >= 0)
            color = format.underline_color1;
         else if (format.font_color.r >= 0)
            color = format.font_color;
         else
            color = black;
         fmt_str = etk_string_set_printf(fmt_str, "+ strikethrough_color=#%.2X%.2X%.2X%.2X",
            color.r, color.g, color.b, color.a);
         evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
         opened_nodes++;
      }

      /* Font settings */
      if (format.font_size >= 0)
      {
         fmt_str = etk_string_set_printf(fmt_str, "+ font_size=%d", format.font_size);
         evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
         opened_nodes++;
      }
      if (format.font_color.r >= 0)
      {
         fmt_str = etk_string_set_printf(fmt_str, "+ color=#%.2X%.2X%.2X%.2X", format.font_color.r,
            format.font_color.g, format.font_color.b, format.font_color.a);
         evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
         opened_nodes++;
      }

      /* Effects */
      if (format.style != ETK_TEXTBLOCK_STYLE_NONE)
      {
         Etk_Bool append_color;

         switch (format.style)
         {
            case ETK_TEXTBLOCK_STYLE_OUTLINE:
               evas_textblock_cursor_format_append(cur, "+ style=outline");
               break;
            case ETK_TEXTBLOCK_STYLE_SHADOW:
               evas_textblock_cursor_format_append(cur, "+ style=shadow");
               break;
            case ETK_TEXTBLOCK_STYLE_SOFT_OUTLINE:
               evas_textblock_cursor_format_append(cur, "+ style=soft_outline");
               break;
            case ETK_TEXTBLOCK_STYLE_GLOW:
               evas_textblock_cursor_format_append(cur, "+ style=glow");
               break;
            case ETK_TEXTBLOCK_STYLE_OUTLINE_SHADOW:
               evas_textblock_cursor_format_append(cur, "+ style=outline_shadow");
               break;
            case ETK_TEXTBLOCK_STYLE_FAR_SHADOW:
               evas_textblock_cursor_format_append(cur, "+ style=far_shadow");
               break;
            case ETK_TEXTBLOCK_STYLE_OUTLINE_SOFT_SHADOW:
               evas_textblock_cursor_format_append(cur, "+ style=outline_soft_shadow");
               break;
            case ETK_TEXTBLOCK_STYLE_SOFT_SHADOW:
               evas_textblock_cursor_format_append(cur, "+ style=soft_shadow");
               break;
            case ETK_TEXTBLOCK_STYLE_FAR_SOFT_SHADOW:
               evas_textblock_cursor_format_append(cur, "+ style=far_soft_shadow");
               break;
            default:
               opened_nodes--;
               break;
         }
         opened_nodes++;

         /* First color */
         if (format.style_color1.r >= 0)
         {
            append_color = ETK_TRUE;

            switch (format.style)
            {
               case ETK_TEXTBLOCK_STYLE_OUTLINE:
               case ETK_TEXTBLOCK_STYLE_SOFT_OUTLINE:
               case ETK_TEXTBLOCK_STYLE_OUTLINE_SHADOW:
               case ETK_TEXTBLOCK_STYLE_OUTLINE_SOFT_SHADOW:
                  fmt_str = etk_string_set(fmt_str, "+ outline_color=");
                  break;
               case ETK_TEXTBLOCK_STYLE_SHADOW:
               case ETK_TEXTBLOCK_STYLE_FAR_SHADOW:
               case ETK_TEXTBLOCK_STYLE_SOFT_SHADOW:
               case ETK_TEXTBLOCK_STYLE_FAR_SOFT_SHADOW:
                  fmt_str = etk_string_set(fmt_str, "+ shadow_color=");
                  break;
               case ETK_TEXTBLOCK_STYLE_GLOW:
                  fmt_str = etk_string_set(fmt_str, "+ glow_color=");
                  break;
               default:
                  append_color = ETK_FALSE;
                  break;
            }

            if (append_color)
            {
               fmt_str = etk_string_append_printf(fmt_str, "#%.2X%.2X%.2X%.2X", format.style_color1.r,
                  format.style_color1.g, format.style_color1.b, format.style_color1.a);
               evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
               opened_nodes++;
            }
         }

         /* Second color */
         if (format.style_color2.r >= 0)
         {
            append_color = ETK_TRUE;

            switch (format.style)
            {
               case ETK_TEXTBLOCK_STYLE_OUTLINE_SHADOW:
               case ETK_TEXTBLOCK_STYLE_OUTLINE_SOFT_SHADOW:
                  fmt_str = etk_string_set(fmt_str, "+ shadow_color=");
                  break;
               case ETK_TEXTBLOCK_STYLE_GLOW:
                  fmt_str = etk_string_set(fmt_str, "+ glow_color2=");
                  break;
               default:
                  append_color = ETK_FALSE;
                  break;
            }

            if (append_color)
            {
               fmt_str = etk_string_append_printf(fmt_str, "#%.2X%.2X%.2X%.2X", format.style_color2.r,
                  format.style_color2.g, format.style_color2.b, format.style_color2.a);
               evas_textblock_cursor_format_append(cur, etk_string_get(fmt_str));
               opened_nodes++;
            }
         }
      }
   }
   etk_object_destroy(ETK_OBJECT(fmt_str));

   /* Then, we insert the text */
   if (etk_string_length_get(node->text) > 0)
      evas_textblock_cursor_text_append(cur, etk_string_get(node->text));

   /* Finally, we fill the child nodes and we close the format nodes that we have opened */
   if (node->type != ETK_TEXTBLOCK_NODE_LINE)
   {
      for (i = 0; i < opened_nodes; i++)
         evas_textblock_cursor_format_append(cur, "-");
   }

   for (n = node->children; n; n = n->next)
      _etk_textblock_object_line_fill(tbo, cur, n);

   if (node->type == ETK_TEXTBLOCK_NODE_LINE)
   {
      for (i = 0; i < opened_nodes; i++)
         evas_textblock_cursor_format_append(cur, "-");
   }
}

/* Queues an update for a line of the textblock object */
static void _etk_textblock_object_line_update_queue(Evas_Object *tbo, Etk_Textblock_Object_Line *line, Etk_Bool content_update, Etk_Bool geometry_update)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !line || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   line->need_geometry_update |= geometry_update;
   line->need_content_update |= content_update;

   if (!tbo_sd->update_job)
      tbo_sd->update_job = ecore_job_add(_etk_textblock_object_update_job_cb, tbo);
}

/* Updates the line of the textblock object */
static void _etk_textblock_object_line_update(Evas_Object *tbo, Etk_Textblock_Object_Line *line, int y)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Evas *evas;
   int ox, oy, ow, oh;
   const char *font_face;
   int font_size;

   if (!tbo || !line || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;
   if (!(evas = evas_object_evas_get(tbo)))
      return;

   line->geometry.x = 0;
   line->geometry.y = y;

   if (!line->need_content_update && !line->need_geometry_update)
      return;

   evas_object_geometry_get(tbo, &ox, &oy, &ow, &oh);
   line->geometry.w = ow;

   if (!_etk_textblock_node_is_empty_line(line->node, &font_face, &font_size))
   {
      if (!line->object)
      {
         line->object = evas_object_textblock_add(evas);
         evas_object_textblock_style_set(line->object, tbo_sd->style);
         evas_object_clip_set(line->object, tbo_sd->clip);
         evas_object_smart_member_add(line->object, tbo);

         line->need_content_update = ETK_TRUE;
         line->need_geometry_update = ETK_TRUE;
      }

      if (line->need_content_update)
      {
         Evas_Textblock_Cursor *cur;

         evas_object_textblock_clear(line->object);
         cur = evas_object_textblock_cursor_new(line->object);
         evas_textblock_cursor_node_first(cur);
         _etk_textblock_object_line_fill(tbo, cur, line->node);
         evas_textblock_cursor_free(cur);

         line->need_content_update = ETK_FALSE;
      }

      if (line->need_geometry_update)
      {
         evas_object_resize(line->object, line->geometry.w, ETK_TB_OBJECT_DEFAULT_HEIGHT);
         evas_object_textblock_size_formatted_get(line->object, NULL, &line->geometry.h);

         line->need_geometry_update = ETK_FALSE;
      }
   }
   else
   {
      if (line->object)
      {
         evas_object_del(line->object);
         line->object = NULL;
      }

      etk_textblock_char_size_get(evas, "Vera", 10, NULL, &line->geometry.h);

      line->need_content_update = ETK_FALSE;
      line->need_geometry_update = ETK_FALSE;
   }
}

/* Queues an update for the cursor and the selection of the textblock object */
static void _etk_textblock_object_cursor_update_queue(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   if (!tbo_sd->update_job)
      tbo_sd->update_job = ecore_job_add(_etk_textblock_object_update_job_cb, tbo);
}

/* Updates the cursor object and the selection of the textblock object */
static void _etk_textblock_object_cursor_update(Evas_Object *tbo)
{
   Evas *evas;
   Etk_Textblock_Object_SD *tbo_sd;
   int visible;
   int ox, oy;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)) || !(evas = evas_object_evas_get(tbo)))
      return;

   visible = evas_object_visible_get(tbo);
   evas_object_geometry_get(tbo, &ox, &oy, NULL, NULL);

   /* Update the selection */
   {
      Eina_List *selection_geometry;
      Etk_Geometry *rect_geometry;
      Evas_Object *rect;
      int res;

      /* TODO: selection stacking ?! */
      while (tbo_sd->selection_rects)
      {
         evas_object_del(tbo_sd->selection_rects->data);
         tbo_sd->selection_rects = eina_list_remove_list(tbo_sd->selection_rects, tbo_sd->selection_rects);
      }
      res = etk_textblock_iter_compare(tbo_sd->cursor, tbo_sd->selection);
      if (res != 0)
      {
         if (res < 0)
            selection_geometry = _etk_textblock_object_range_geometry_get(tbo, tbo_sd->cursor, tbo_sd->selection);
         else
            selection_geometry = _etk_textblock_object_range_geometry_get(tbo, tbo_sd->selection, tbo_sd->cursor);

         while (selection_geometry)
         {
            rect_geometry = selection_geometry->data;

            rect = evas_object_rectangle_add(evas);
            /* TODO: make the selection themeable */
            evas_object_color_set(rect, 245, 205, 109, 102);
            evas_object_move(rect, ox + rect_geometry->x, oy + rect_geometry->y);
            evas_object_resize(rect, rect_geometry->w, rect_geometry->h);
            evas_object_clip_set(rect, tbo_sd->clip);
            evas_object_smart_member_add(rect, tbo);
            if (visible)
               evas_object_show(rect);

            free(rect_geometry);
            tbo_sd->selection_rects = eina_list_append(tbo_sd->selection_rects, rect);
            selection_geometry = eina_list_remove_list(selection_geometry, selection_geometry);
         }
      }
   }


   /* Update the cursor */
   {
      Evas_Textblock_Cursor *cur;
      Etk_Textblock_Object_Line *line;
      int cx = 0, cy = 0, cw = 0, ch = 0;   /* TODO: valg (= 0)? */
      const char *font_face;
      int font_size;

      line = _etk_textblock_object_line_get_from_node(tbo, tbo_sd->cursor->node);
      if (_etk_textblock_node_is_empty_line(tbo_sd->cursor->node, &font_face, &font_size))
      {
         /* TODO: align, margins... */
         /* TODO: correct font, and align */
         etk_textblock_char_size_get(evas, "Vera", 10, NULL, &ch);
         evas_object_move(tbo_sd->cursor_object, ox + line->geometry.x, oy + line->geometry.y);
         evas_object_resize(tbo_sd->cursor_object, 1, ch);
      }
      else
      {
         /* TODO: optimize?! */
         cur = _etk_textblock_object_cursor_get_from_iter(tbo, tbo_sd->cursor);
         evas_textblock_cursor_char_geometry_get(cur, &cx, &cy, &cw, &ch);

         /* TODO: make sure the cursor is visible?! */
         if (tbo_sd->cursor->pos < tbo_sd->cursor->node->unicode_length)
            evas_object_move(tbo_sd->cursor_object, ox + ETK_MAX(0, cx + line->geometry.x - 1), cy + line->geometry.y + oy);
         else
            evas_object_move(tbo_sd->cursor_object, ox + ETK_MAX(0, cx + line->geometry.x + cw - 1), cy + line->geometry.y + oy);

         evas_object_resize(tbo_sd->cursor_object, 1, ch);
      }
   }
}

/* Queues an update for the textblock object */
static void _etk_textblock_object_update_queue(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   if (!tbo_sd->update_job)
      tbo_sd->update_job = ecore_job_add(_etk_textblock_object_update_job_cb, tbo);
}

/* Updates the textblock object */
static void _etk_textblock_object_update(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;
   int ox, oy, ow, oh;
   int y;

   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   tbo_sd->first_visible_line = NULL;
   tbo_sd->last_visible_line = NULL;

   /* We update the lines */
   /* TODO: optimize: Updates all the lines in several times and use a binary tree! */
   y = - tbo_sd->yoffset;
   evas_object_geometry_get(tbo, &ox, &oy, &ow, &oh);
   for (line = tbo_sd->lines; line; line = line->next)
   {
      _etk_textblock_object_line_update(tbo, line, y);

      if (line->object)
      {
         if ((y + line->geometry.h <= 0 || y >= oh))
         {
            evas_object_del(line->object);
            line->object = NULL;
         }
         else
         {
            evas_object_move(line->object, tbo_sd->xoffset + ox + line->geometry.x, oy + line->geometry.y);
            evas_object_resize(line->object, line->geometry.w, line->geometry.h);
            evas_object_show(line->object);

            if (!tbo_sd->first_visible_line)
               tbo_sd->first_visible_line = line;
            tbo_sd->last_visible_line = line;
         }
      }

      y += line->geometry.h;
   }

   /* Then we update the cursor and the selection */
   _etk_textblock_object_cursor_update(tbo);
}

/* The job used to update the textblock object */
static void _etk_textblock_object_update_job_cb(void *data)
{
   Evas_Object *tbo;
   Etk_Textblock_Object_SD *tbo_sd;

   if (!(tbo = data) || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   _etk_textblock_object_update(tbo);
   tbo_sd->update_job = NULL;
}

/* The timer that shows/hides the cursor of a textblock object */
static int _etk_textblock_object_cursor_timer_cb(void *data)
{
   Evas_Object *tbo;
   Etk_Textblock_Object_SD *tbo_sd;

   if (!(tbo = data) || !(tbo_sd = evas_object_smart_data_get(tbo)) || !tbo_sd->cursor_visible)
      return 1;

   if (evas_object_visible_get(tbo_sd->cursor_object))
   {
      evas_object_hide(tbo_sd->cursor_object);
      ecore_timer_interval_set(tbo_sd->cursor_timer, ETK_TB_OBJECT_HIDE_CURSOR_DELAY);
   }
   else
   {
      evas_object_show(tbo_sd->cursor_object);
      ecore_timer_interval_set(tbo_sd->cursor_timer, ETK_TB_OBJECT_SHOW_CURSOR_DELAY);
   }

   return 1;
}

/* Gets the line of a textblock object that contains the given node */
static Etk_Textblock_Object_Line *_etk_textblock_object_line_get_from_node(Evas_Object *tbo, Etk_Textblock_Node *node)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;
   Etk_Textblock_Node *n;

   if (!tbo || !node || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return NULL;

   for (n = node; n; n = n->parent)
   {
      if (n->type == ETK_TEXTBLOCK_NODE_LINE)
         break;
   }
   if (!n)
      return NULL;

   for (line = tbo_sd->lines; line; line = line->next)
   {
      if (line->node == n)
         return line;
   }

   return NULL;
}

/* Gets the evas cursor corresponding to the iterator.
 * The returned cursor should be freed with evas_textblock_cursor_free()
 * The returned cursor could be invalidated during the next update, so you can't keep the cursor */
static Evas_Textblock_Cursor *_etk_textblock_object_cursor_get_from_iter(Evas_Object *tbo, Etk_Textblock_Iter *iter)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;
   Evas_Textblock_Cursor *cur;
   int num_text_nodes, i;
   const char *text;

   if (!tbo || !iter || !(tbo_sd = evas_object_smart_data_get(tbo)) || !_etk_textblock_iter_is_valid(NULL, iter))
      return NULL;
   if (!(line = _etk_textblock_object_line_get_from_node(tbo, iter->node)))
      return NULL;

   if (!line->object)
   {
      line->need_geometry_update = ETK_TRUE;
      line->need_content_update = ETK_TRUE;
      _etk_textblock_object_line_update(tbo, line, line->geometry.y);
   }

   num_text_nodes = _etk_textblock_text_nodes_count(line->node, iter->node);
   cur = evas_object_textblock_cursor_new(line->object);

   i = 0;
   for (;;)
   {
      if ((text = evas_textblock_cursor_node_text_get(cur)) && *text)
         i++;
      if (i >= num_text_nodes)
         break;
      if (!evas_textblock_cursor_node_next(cur))
         break;
   }

   if (iter->pos >= iter->node->unicode_length)
      evas_textblock_cursor_pos_set(cur, ETK_MAX(0, iter->node->unicode_length - 1));
   else
      evas_textblock_cursor_pos_set(cur, iter->pos);

   return cur;
}

/* Counts the number of text nodes that should be walked through to reach "node", in the line "line".
 * This function is used by _etk_textblock_object_cursor_get_from_iter() */
static int _etk_textblock_text_nodes_count(Etk_Textblock_Node *line, Etk_Textblock_Node *node)
{
   int count = 0;
   Etk_Textblock_Node *n;

   if (!line || !node || line->type != ETK_TEXTBLOCK_NODE_LINE || node->children)
      return 0;

   /* Check if "node" is a child node of "line" */
   for (n = node; n; n = n->parent)
   {
      if (n->type == ETK_TEXTBLOCK_NODE_LINE)
      {
         if (n != line)
            return 0;
         else
            break;
      }
   }

   for (n = line; n->children; n = n->children);
   for ( ; n; n = _etk_textblock_next_node_get(n))
   {
      if (etk_string_length_get(n->text) > 0)
         count++;
      if (n == node)
         break;
   }

   return count;
}


/* Gets a list of Etk_Geometry's corresponding to the geometry of the text between @a start and @a end.
 * The items of the returned list should be freed, and the list should be destroyed manually.
 * Only the visible lines are stored in the list.  */
static Eina_List *_etk_textblock_object_range_geometry_get(Evas_Object *tbo, Etk_Textblock_Iter *start, Etk_Textblock_Iter *end)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line, *start_line, *end_line;
   Eina_List *selection_geometry = NULL;
   Etk_Geometry *rect_geometry;
   Etk_Bool add_line;
   Evas *evas;

   if (!tbo || !start || !end)
      return NULL;
   if (!(tbo_sd = evas_object_smart_data_get(tbo)) || !(evas = evas_object_evas_get(tbo)))
      return NULL;
   if (!tbo_sd->first_visible_line)
      return NULL;

   if (_etk_textblock_node_compare(end->node, tbo_sd->first_visible_line->node) < 0)
      return NULL;

   if (_etk_textblock_node_compare(start->node, tbo_sd->first_visible_line->node) <= 0)
      add_line = ETK_TRUE;
   else
      add_line = ETK_FALSE;

   start_line = _etk_textblock_object_line_get_from_node(tbo, start->node);
   end_line = _etk_textblock_object_line_get_from_node(tbo, end->node);

   for (line = tbo_sd->first_visible_line; line; line = line->next)
   {
      if (!add_line && line == start_line)
         add_line = ETK_TRUE;

      /* We add the geometry of the line to the list */
      if (add_line)
      {
         Eina_List *rects;
         Evas_Textblock_Rectangle *rect;
         const char *font_face;
         int font_size;

         if (_etk_textblock_node_is_empty_line(line->node, &font_face, &font_size))
         {
            rect_geometry = malloc(sizeof(Etk_Geometry));
            rect_geometry->x = line->geometry.x;
            rect_geometry->y = line->geometry.y;
            rect_geometry->w = ETK_TB_EMPTY_LINE_WIDTH;
            /* TODO: correct font, and align */
            etk_textblock_char_size_get(evas, "Vera", 10, NULL, &rect_geometry->h);

            selection_geometry = eina_list_append(selection_geometry, rect_geometry);
         }
         else if (line->object)
         {
            Evas_Textblock_Cursor *cur1, *cur2;

            cur1 = NULL;
            /* TODO: optimize */
            if (line == start_line)
               cur1 = _etk_textblock_object_cursor_get_from_iter(tbo, start);
            if (!cur1)
            {
               cur1 = evas_object_textblock_cursor_new(line->object);
               evas_textblock_cursor_node_first(cur1);
               while (!evas_textblock_cursor_node_text_get(cur1))
               {
                  if (!evas_textblock_cursor_node_next(cur1))
                     break;
               }
               evas_textblock_cursor_char_first(cur1);
            }

            cur2 = NULL;
            /* TODO: optimize */
            if (line == end_line)
            {
               cur2 = _etk_textblock_object_cursor_get_from_iter(tbo, end);
               evas_textblock_cursor_char_prev(cur2);
            }
            if (!cur2)
            {
               cur2 = evas_object_textblock_cursor_new(line->object);
               evas_textblock_cursor_node_last(cur2);
               while (!evas_textblock_cursor_node_text_get(cur2))
               {
                  if (!evas_textblock_cursor_node_prev(cur2))
                     break;
               }
               evas_textblock_cursor_char_last(cur2);
            }

            rects = evas_textblock_cursor_range_geometry_get(cur1, cur2);
            while (rects)
            {
               rect = rects->data;

               rect_geometry = malloc(sizeof(Etk_Geometry));
               rect_geometry->x = rect->x + line->geometry.x;
               rect_geometry->y = rect->y + line->geometry.y;
               rect_geometry->w = rect->w;
               rect_geometry->h = rect->h;
               selection_geometry = eina_list_append(selection_geometry, rect_geometry);

               free(rect);
               rects = eina_list_remove_list(rects, rects);
            }
         }
      }

      if (line == end_line)
         break;
   }

   return selection_geometry;
}

/* Moves the iter to the position (x, y) */
static void _etk_textblock_object_iter_move_to(Evas_Object *tbo, Etk_Textblock_Iter *iter, int x, int y)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line, *l;

   if (!tbo || !iter || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;

   /* TODO: optimize: use an AVL to store the object's lines */
   /* TODO: take account of scrolling */
   line = tbo_sd->last_line;
   for (l = tbo_sd->lines; l; l = l->next)
   {
      if (l->geometry.y + l->geometry.h >= y)
      {
         line = l;
         break;
      }
   }

   if (!line)
      return;

   for (iter->node = line->node; iter->node->children; iter->node = iter->node->children);
   iter->pos = 0;
   iter->index = 0;

   if (line->object && !_etk_textblock_node_is_empty(line->node))
   {
      Etk_Textblock_Node *node;
      Evas_Textblock_Cursor *cur;
      const char *text;
      int line_num;
      int pos, i;
      int lx, lw;

      cur = evas_object_textblock_cursor_new(line->object);
      if (!evas_textblock_cursor_char_coord_set(cur, x - line->geometry.x, y - line->geometry.y))
      {
	 line_num = evas_textblock_cursor_line_coord_set(cur, y - line->geometry.y);
	 if (line_num >= 0)
	 {
            evas_textblock_cursor_line_geometry_get(cur, &lx, NULL, &lw, NULL);

            if (x - (line->geometry.x + lx) <= (lw / 2))
               evas_textblock_cursor_line_first(cur);
            else
               evas_textblock_cursor_line_last(cur);
         }
      }
      pos = evas_textblock_cursor_pos_get(cur);

      while (evas_textblock_cursor_node_prev(cur))
      {
         if ((text = evas_textblock_cursor_node_text_get(cur)) && *text)
         {
            if ((node = _etk_textblock_next_text_node_get(iter->node, ETK_TRUE, NULL)))
               iter->node = node;
         }
      }

      iter->pos = pos;
      iter->index = 0;
      for (i = 0; i < pos; i++)
         iter->index = evas_string_char_next_get(etk_string_get(iter->node->text), iter->index, NULL);

      evas_textblock_cursor_free(cur);
   }

   _etk_textblock_iter_update(iter);
}

/* Called when the textblock object is pressed by the mouse */
static void _etk_tb_object_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Evas_Event_Mouse_Down *event;
   int ox, oy;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)) || !(event = event_info))
      return;

   evas_object_geometry_get(obj, &ox, &oy, NULL, NULL);
   _etk_textblock_object_iter_move_to(obj, tbo_sd->cursor, event->canvas.x - ox, event->canvas.y - oy);
   etk_textblock_iter_copy(tbo_sd->selection, tbo_sd->cursor);
   tbo_sd->selecting = ETK_TRUE;
}

/* Called when the textblock object is released by the mouse */
static void _etk_tb_object_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
   tbo_sd->selecting = ETK_FALSE;
}

/* Called when the mouse moves over the textblock object */
static void _etk_tb_object_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Evas_Event_Mouse_Move *event;
   int ox, oy;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)) || !(event = event_info) || !tbo_sd->selecting)
      return;

   evas_object_geometry_get(obj, &ox, &oy, NULL, NULL);
   _etk_textblock_object_iter_move_to(obj, tbo_sd->cursor, event->cur.canvas.x - ox, event->cur.canvas.y - oy);
}

/**************************
 *
 * Textblock object's smart object
 *
 **************************/

/* Initializes the new textblock object */
static void _etk_tb_object_smart_add(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Evas *evas;

   if (!obj || !(evas = evas_object_evas_get(obj)))
      return;

   tbo_sd = malloc(sizeof(Etk_Textblock_Object_SD));
   tbo_sd->tb = NULL;
   tbo_sd->wrap = ETK_TEXTBLOCK_WRAP_WORD;
   tbo_sd->cursor_visible = ETK_TRUE;
   tbo_sd->selecting = ETK_FALSE;
   tbo_sd->cursor_timer = NULL;
   tbo_sd->update_job = NULL;

   tbo_sd->clip = evas_object_rectangle_add(evas);
   evas_object_smart_member_add(tbo_sd->clip, obj);

   tbo_sd->bg = evas_object_rectangle_add(evas);
   evas_object_color_set(tbo_sd->bg, 255, 255, 255, 0);
   evas_object_clip_set(tbo_sd->bg, tbo_sd->clip);
   evas_object_smart_member_add(tbo_sd->bg, obj);

   tbo_sd->cursor_object = evas_object_rectangle_add(evas);
   /* TODO: make the cursor color themeable */
   evas_object_color_set(tbo_sd->cursor_object, 0, 0, 0, 255);
   evas_object_clip_set(tbo_sd->cursor_object, tbo_sd->clip);
   evas_object_smart_member_add(tbo_sd->cursor_object, obj);

   tbo_sd->selection_rects = NULL;

   tbo_sd->lines = NULL;
   tbo_sd->last_line = NULL;
   tbo_sd->first_visible_line = NULL;
   tbo_sd->last_visible_line = NULL;

   tbo_sd->xoffset = 0;
   tbo_sd->yoffset = 0;

   tbo_sd->cursor = NULL;
   tbo_sd->selection = NULL;

   tbo_sd->style = evas_textblock_style_new();
   evas_textblock_style_set(tbo_sd->style, "DEFAULT='font=Vera font_size=10 align=left color=#000000'");

   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN, _etk_tb_object_mouse_down_cb, NULL);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP, _etk_tb_object_mouse_up_cb, NULL);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE, _etk_tb_object_mouse_move_cb, NULL);

   evas_object_smart_data_set(obj, tbo_sd);
}

/* Destroys the textblock object */
static void _etk_tb_object_smart_del(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line, *next;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;

   if (tbo_sd->cursor_timer)
      ecore_timer_del(tbo_sd->cursor_timer);
   if (tbo_sd->update_job)
      ecore_job_del(tbo_sd->update_job);

   while (tbo_sd->selection_rects)
   {
      evas_object_del(tbo_sd->selection_rects->data);
      tbo_sd->selection_rects = eina_list_remove_list(tbo_sd->selection_rects, tbo_sd->selection_rects);
   }
   for (line = tbo_sd->lines; line; line = next)
   {
      if (line->object)
         evas_object_del(line->object);
      next = line->next;
      free(line);
   }
   evas_object_del(tbo_sd->cursor_object);
   evas_object_del(tbo_sd->clip);
   evas_object_del(tbo_sd->bg);
   evas_textblock_style_free(tbo_sd->style);

   tbo_sd->tb->evas_objects = eina_list_remove(tbo_sd->tb->evas_objects, obj);
   free(tbo_sd);

   _etk_tb_object_smart_use--;
   if (_etk_tb_object_smart_use <= 0)
   {
      evas_smart_free(_etk_tb_object_smart);
      _etk_tb_object_smart = NULL;
   }
}

/* Moves the textblock object */
static void _etk_tb_object_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Textblock_Object_SD *tbo_sd;
   int prev_x, prev_y;
   int current_x, current_y;
   Etk_Textblock_Object_Line *line;
   Evas_Object *selection_rect;
   Eina_List *l;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;

   evas_object_move(tbo_sd->bg, x, y);
   evas_object_move(tbo_sd->clip, x, y);

   evas_object_geometry_get(obj, &prev_x, &prev_y, NULL, NULL);
   for (line = tbo_sd->first_visible_line; line; line = line->next)
   {
      if (line->object)
      {
         evas_object_geometry_get(line->object, &current_x, &current_y, NULL, NULL);
         evas_object_move(line->object, current_x + x - prev_x, current_y + y - prev_y);
      }

      if (line == tbo_sd->last_visible_line)
         break;
   }

   evas_object_geometry_get(tbo_sd->cursor_object, &current_x, &current_y, NULL, NULL);
   evas_object_move(tbo_sd->cursor_object, current_x + x - prev_x, current_y + y - prev_y);

   for (l = tbo_sd->selection_rects; l; l = l->next)
   {
      selection_rect = l->data;
      evas_object_geometry_get(selection_rect, &current_x, &current_y, NULL, NULL);
      evas_object_move(selection_rect, current_x + x - prev_x, current_y + y - prev_y);
   }
}

/* Resizes the textblock object */
static void _etk_tb_object_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;

   evas_object_resize(tbo_sd->bg, w, h);
   evas_object_resize(tbo_sd->clip, w, h);

   /* TODO: optimization for non-wrapped lines? */
   for (line = tbo_sd->lines; line; line = line->next)
      _etk_textblock_object_line_update_queue(obj, line, ETK_FALSE, ETK_TRUE);
}

/* Shows the textblock object */
static void _etk_tb_object_smart_show(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;
   Eina_List *l;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;

   evas_object_show(tbo_sd->bg);
   evas_object_show(tbo_sd->clip);

   for (line = tbo_sd->first_visible_line; line; line = line->next)
   {
      if (line->object)
         evas_object_show(line->object);

      if (line == tbo_sd->last_visible_line)
         break;
   }

   if (tbo_sd->cursor_visible)
   {
      evas_object_show(tbo_sd->cursor_object);
      if (!tbo_sd->cursor_timer)
      {
         tbo_sd->cursor_timer = ecore_timer_add(ETK_TB_OBJECT_SHOW_CURSOR_DELAY,
            _etk_textblock_object_cursor_timer_cb, obj);
      }
      else
         ecore_timer_interval_set(tbo_sd->cursor_timer, ETK_TB_OBJECT_SHOW_CURSOR_DELAY);
   }

   for (l = tbo_sd->selection_rects; l; l = l->next)
      evas_object_show(l->data);
}

/* Hides the textblock object */
static void _etk_tb_object_smart_hide(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Etk_Textblock_Object_Line *line;
   Eina_List *l;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;

   evas_object_hide(tbo_sd->bg);
   evas_object_hide(tbo_sd->clip);

   for (line = tbo_sd->first_visible_line; line; line = line->next)
   {
      if (line->object)
         evas_object_hide(line->object);

      if (line == tbo_sd->last_visible_line)
         break;
   }

   evas_object_hide(tbo_sd->cursor_object);
   if (tbo_sd->cursor_timer)
   {
      ecore_timer_del(tbo_sd->cursor_timer);
      tbo_sd->cursor_timer = NULL;
   }

   for (l = tbo_sd->selection_rects; l; l = l->next)
      evas_object_hide(l->data);
}

/* Set the color of the textblock object */
static void _etk_tb_object_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
   evas_object_color_set(tbo_sd->clip, r, g, b, a);
}

/* Clips the textblock object */
static void _etk_tb_object_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
   evas_object_clip_set(tbo_sd->clip, clip);
}

/* Unclips the textblock object */
static void _etk_tb_object_smart_clip_unset(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;

   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
   evas_object_clip_unset(tbo_sd->clip);
}

/** @} */
