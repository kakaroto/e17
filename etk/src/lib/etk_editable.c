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

/** @file etk_editable.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_editable.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_string.h"
#include "etk_theme.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Editable
 * @{
 */

#define CURSOR_MARGIN 5
#define SELECTION_MARGIN 10

typedef struct Etk_Editable_Smart_Data
{
   Evas_Object *clip_object;
   Evas_Object *event_object;
   Evas_Object *text_object;
   Evas_Object *cursor_object;
   Evas_Object *selection_object;
   Etk_String *text;

   int cursor_pos;
   int selection_pos;
   int unicode_length;
   int cursor_width;
   int average_char_w;
   int average_char_h;

   float align;

   Etk_Bool cursor_visible:1;
   Etk_Bool selection_visible:1;
   Etk_Bool password_mode:1;
   Etk_Bool disabled:1;
   Etk_Bool selection_on_fg:1;
} Etk_Editable_Smart_Data;


static int _etk_editable_text_insert(Evas_Object *editable, int pos, const char *text);
static int _etk_editable_text_delete(Evas_Object *editable, int start, int end);
static void _etk_editable_cursor_update(Evas_Object *editable);
static void _etk_editable_selection_update(Evas_Object *editable);
static void _etk_editable_text_update(Evas_Object *editable);
static void _etk_editable_text_position_update(Evas_Object *editable, int ow, int oh);
static int _etk_editable_char_geometry_get_from_pos(Evas_Object *editable, int pos, int *cx, int *cy, int *cw, int *ch);

static void _etk_editable_smart_add(Evas_Object *object);
static void _etk_editable_smart_del(Evas_Object *object);
static void _etk_editable_smart_move(Evas_Object *object, int x, int y);
static void _etk_editable_smart_resize(Evas_Object *object, int w, int h);
static void _etk_editable_smart_show(Evas_Object *object);
static void _etk_editable_smart_hide(Evas_Object *object);
static void _etk_editable_color_set(Evas_Object *object, int r, int g, int b, int a);
static void _etk_editable_clip_set(Evas_Object *object, Evas_Object *clip);
static void _etk_editable_clip_unset(Evas_Object *object);


static Evas_Smart *_etk_editable_smart = NULL;
static int _etk_editable_smart_use = 0;


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Creates a new editable object
 * @param evas the evas where to add the editable object
 * @return Returns the new editable object
 */
Evas_Object *etk_editable_add(Evas *evas)
{
   if (!_etk_editable_smart)
   {
      static const Evas_Smart_Class sc =
      {
         "Editable_Object",
         EVAS_SMART_CLASS_VERSION,
         _etk_editable_smart_add,
         _etk_editable_smart_del,
         _etk_editable_smart_move,
         _etk_editable_smart_resize,
         _etk_editable_smart_show,
         _etk_editable_smart_hide,
         _etk_editable_color_set,
         _etk_editable_clip_set,
         _etk_editable_clip_unset,
         NULL,
         NULL
      };
      _etk_editable_smart = evas_smart_class_new(&sc);
      _etk_editable_smart_use = 0;
   }

   return evas_object_smart_add(evas, _etk_editable_smart);
}

/**
 * @brief Sets the theme of the editable object.
 * If this function is not called, the editable object won't be visible
 * @param editable an editable object
 * @param file the path of the theme-file to use (NULL to use the current theme-file of Etk)
 * @param group the theme-group to use ("entry", "spinner", ...)
 */
void etk_editable_theme_set(Evas_Object *editable, const char *file, const char *group)
{
   Etk_Editable_Smart_Data *sd;
   const char *data;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;

   etk_theme_edje_object_set(sd->text_object, NULL, "text", group);
   sd->average_char_w = -1;
   sd->average_char_h = -1;

   etk_theme_edje_object_set(sd->cursor_object, NULL, "cursor", group);
   edje_object_size_min_get(sd->cursor_object, &sd->cursor_width, NULL);
   sd->cursor_width = ETK_MAX(sd->cursor_width, 1);

   etk_theme_edje_object_set(sd->selection_object, NULL, "selection", group);
   data = edje_object_data_get(sd->selection_object, "on_foreground");
   if (data && strcmp(data, "1") == 0)
      evas_object_stack_above(sd->selection_object, sd->text_object);
   else
      evas_object_stack_below(sd->selection_object, sd->text_object);

   _etk_editable_text_update(editable);
   _etk_editable_cursor_update(editable);
}

/**
 * @brief Sets the alignment of the text inside the editable object
 * @param editable an editable object
 * @param align the alignment of the text, from 0.0 (left alignment) to 1.0 (right alignment)
 */
void etk_editable_align_set(Evas_Object *editable, float align)
{
   Etk_Editable_Smart_Data *sd;
   int w, h;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;

   sd->align = ETK_CLAMP(align, 0.0, 1.0);
   evas_object_geometry_get(editable, NULL, NULL, &w, &h);
   _etk_editable_text_position_update(editable, w, h);
}

/**
 * @brief Gets the alignment of the text inside the editable object
 * @param editable an editable object
 * @return Returns the alignment of the text, from 0.0 (left alignment) to 1.0 (right alignment)
 */
float etk_editable_align_get(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return 0.0;
   return sd->align;
}

/**
 * @brief Sets whether or not the editable object is in password mode. In password mode, the
 * editable object displays '*' instead of the characters
 * @param editable an editable object
 * @param password_mode ETK_TRUE to turn on the password mode, ETK_FALSE to turn it off
 */
void etk_editable_password_mode_set(Evas_Object *editable, Etk_Bool password_mode)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (sd->password_mode == password_mode)
      return;

   sd->password_mode = password_mode;
   _etk_editable_text_update(editable);
   _etk_editable_cursor_update(editable);
}

/**
 * @brief Gets whether or not the editable object is in password mode
 * @param editable an editable object
 * @return Returns ETK_TRUE if the editable object is in password mode, ETK_FALSE otherwise
 */
Etk_Bool etk_editable_password_mode_get(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return ETK_FALSE;
   return sd->password_mode;
}

/**
 * @brief Sets whether or not the editable object is disabled. This will just emit
 * the "etk,state,disabled" or "etk,state,enabled" signal to the text-object of the editable
 * @param editable an editable object
 * @param disabled ETK_TRUE to disable the editable object, ETK_FALSE to enable it
 */
void etk_editable_disabled_set(Evas_Object *editable, Etk_Bool disabled)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (sd->disabled == disabled)
      return;

   if (disabled)
      edje_object_signal_emit(sd->text_object, "etk,state,disabled", "etk");
   else
      edje_object_signal_emit(sd->text_object, "etk,state,enabled", "etk");
   sd->disabled = disabled;
}

/**
 * @brief Gets whether or not the editable object is disabled
 * @param editable an editable object
 * @return Returns ETK_TRUE if the editable object is disabled, ETK_FALSE otherwise
 */
Etk_Bool etk_editable_disabled_get(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return ETK_FALSE;
   return sd->disabled;
}

/**
 * @brief Sets the text of the editable object
 * @param editable an editable object
 * @param text the text to set
 */
void etk_editable_text_set(Evas_Object *editable, const char *text)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;

   etk_string_clear(sd->text);
   sd->unicode_length = 0;
   _etk_editable_text_insert(editable, 0, text);

   sd->cursor_pos = sd->unicode_length;
   sd->selection_pos = sd->unicode_length;
   _etk_editable_cursor_update(editable);
}

/**
 * @brief Gets the entire text of the editable object
 * @param editable an editable object
 * @return Returns the entire text of the editable object
 */
const char *etk_editable_text_get(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return NULL;
   return etk_string_get(sd->text);
}

/**
 * @brief Gets a range of the text of the editable object, from position @a start to position @a end
 * @param editable an editable object
 * @param start the start position of the text range to get
 * @param end the end position of the text range to get
 * @return Returns the range of text. The returned string will have to be freed with free()
 */
char *etk_editable_text_range_get(Evas_Object *editable, int start, int end)
{
   Etk_Editable_Smart_Data *sd;
   const char *text;
   char *range;
   int start_id, end_id;
   int i;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return NULL;
   if (!(text = etk_string_get(sd->text)))
      return NULL;

   start = ETK_CLAMP(start, 0, sd->unicode_length);
   end = ETK_CLAMP(end, 0, sd->unicode_length);
   if (end <= start)
      return NULL;

   start_id = 0;
   end_id = 0;
   for (i = 0; i < end; i++)
   {
      end_id = evas_string_char_next_get(text, end_id, NULL);
      if (i < start)
         start_id = end_id;
   }

   if (end_id <= start_id)
      return NULL;

   range = malloc(end_id - start_id + 1);
   strncpy(range, &text[start_id], end_id - start_id);
   range[end_id - start_id] = '\0';

   return range;
}

/**
 * @brief Gets the unicode length of the text of the editable object. The unicode length is not
 * always the length returned by strlen() since a UTF-8 char can take several bytes
 * @param editable an editable object
 * @return Returns the unicode length of the text of the editable object
 */
int etk_editable_text_length_get(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return 0;
   return sd->unicode_length;
}

/**
 * @brief Inserts some text at the given position in the editable object
 * @param editable the editable object in which the text should be inserted
 * @param pos the position where to insert the text
 * @param text the text to insert
 * @return Returns ETK_TRUE if the text has been modified, ETK_FALSE otherwise
 */
Etk_Bool etk_editable_insert(Evas_Object *editable, int pos, const char *text)
{
   Etk_Editable_Smart_Data *sd;
   int unicode_length;
   int w, h;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return ETK_FALSE;

   unicode_length = _etk_editable_text_insert(editable, pos, text);
   if (unicode_length <= 0)
      return ETK_FALSE;

   if (sd->cursor_pos >= pos)
      etk_editable_cursor_pos_set(editable, sd->cursor_pos + unicode_length);
   if (sd->selection_pos >= pos)
      etk_editable_selection_pos_set(editable, sd->selection_pos + unicode_length);

   evas_object_geometry_get(editable, NULL, NULL, &w, &h);
   _etk_editable_text_position_update(editable, w, h);
   return ETK_TRUE;
}

/**
 * @brief Deletes the text of the editable object, between position @a start and position @a end
 * @param editable the editable object in which the text should be deleted
 * @param start the position of the first char to delete
 * @param end the position where to stop the deletion
 * @return Returns ETK_TRUE if the text has been modified, ETK_FALSE otherwise
 */
Etk_Bool etk_editable_delete(Evas_Object *editable, int start, int end)
{
   Etk_Editable_Smart_Data *sd;
   int unicode_length;
   int w, h;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return ETK_FALSE;

   unicode_length = _etk_editable_text_delete(editable, start, end);
   if (unicode_length <= 0)
      return ETK_FALSE;

   if (sd->cursor_pos > end)
      etk_editable_cursor_pos_set(editable, sd->cursor_pos - unicode_length);
   else if (sd->cursor_pos > start)
      etk_editable_cursor_pos_set(editable, start);

   if (sd->selection_pos > end)
      etk_editable_selection_pos_set(editable, sd->selection_pos - unicode_length);
   else if (sd->selection_pos > start)
      etk_editable_selection_pos_set(editable, start);

   evas_object_geometry_get(editable, NULL, NULL, &w, &h);
   _etk_editable_text_position_update(editable, w, h);
   return ETK_TRUE;
}

/**
 * @brief Moves the cursor of the editable object to the given position
 * @param editable an editable object
 * @param pos the position where to move the cursor
 */
void etk_editable_cursor_pos_set(Evas_Object *editable, int pos)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;

   pos = ETK_CLAMP(pos, 0, sd->unicode_length);
   if (sd->cursor_pos == pos)
      return;

   sd->cursor_pos = pos;
   _etk_editable_cursor_update(editable);
}

/**
 * @brief Gets the position of the cursor of the editable object
 * @param editable an editable object
 * @return Returns the position of the cursor of the editable object
 */
int etk_editable_cursor_pos_get(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return 0;
   return sd->cursor_pos;
}

/**
 * @brief Moves the cursor to the start of the editable object
 * @param editable an editable object
 */
void etk_editable_cursor_move_to_start(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_cursor_pos_set(editable, 0);
}

/**
 * @brief Moves the cursor to the end of the editable object
 * @param editable an editable object
 */
void etk_editable_cursor_move_to_end(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_cursor_pos_set(editable, sd->unicode_length);
}

/**
 * @brief Moves the cursor backward by one character offset
 * @param editable an editable object
 */
void etk_editable_cursor_move_left(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_cursor_pos_set(editable, sd->cursor_pos - 1);
}

/**
 * @brief Moves the cursor forward by one character offset
 * @param editable an editable object
 */
void etk_editable_cursor_move_right(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_cursor_pos_set(editable, sd->cursor_pos + 1);
}

/**
 * @brief Shows the cursor of the editable object
 * @param editable the editable object whose cursor should be shown
 */
void etk_editable_cursor_show(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (sd->cursor_visible)
      return;

   sd->cursor_visible = ETK_TRUE;
   if (evas_object_visible_get(editable))
   {
      evas_object_show(sd->cursor_object);
      edje_object_signal_emit(sd->cursor_object, "etk,action,show,cursor", "etk");
   }
}

/**
 * @brief Hides the cursor of the editable object
 * @param editable the editable object whose cursor should be hidden
 */
void etk_editable_cursor_hide(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (!sd->cursor_visible)
      return;

   sd->cursor_visible = ETK_FALSE;
   evas_object_hide(sd->cursor_object);
}

/**
 * @brief Moves the selection bound of the editable object to the given position
 * @param editable an editable object
 * @param pos the position where to move the selection bound
 */
void etk_editable_selection_pos_set(Evas_Object *editable, int pos)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;

   pos = ETK_CLAMP(pos, 0, sd->unicode_length);
   if (sd->selection_pos == pos)
      return;

   sd->selection_pos = pos;
   _etk_editable_selection_update(editable);
}

/**
 * @brief Gets the position of the selection bound of the editable object
 * @param editable an editable object
 * @return Returns the position of the selection bound of the editable object
 */
int etk_editable_selection_pos_get(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return 0;
   return sd->selection_pos;
}

/**
 * @brief  Moves the selection bound to the start of the editable object
 * @param editable an editable object
 */
void etk_editable_selection_move_to_start(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_selection_pos_set(editable, 0);
}

/**
 * @brief Moves the selection bound to the end of the editable object
 * @param editable an editable object
 */
void etk_editable_selection_move_to_end(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_selection_pos_set(editable, sd->unicode_length);
}

/**
 * @brief Moves the selection bound backward by one character offset
 * @param editable an editable object
 */
void etk_editable_selection_move_left(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_selection_pos_set(editable, sd->selection_pos - 1);
}

/**
 * @brief Moves the selection bound forward by one character offset
 * @param editable an editable object
 */
void etk_editable_selection_move_right(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_selection_pos_set(editable, sd->selection_pos + 1);
}

/**
 * @brief Selects all the text of the editable object. The selection bound will be moved to the start
 * of the editable object and the cursor will be moved to the end
 * @param editable an editable object
 */
void etk_editable_select_all(Evas_Object *editable)
{
   if (!editable)
      return;
   etk_editable_selection_move_to_start(editable);
   etk_editable_cursor_move_to_end(editable);
}

/**
 * @brief Unselects all the text of the editable object. The selection bound will be moved to the cursor position
 * @param editable an editable object
 */
void etk_editable_unselect_all(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   etk_editable_selection_pos_set(editable, sd->cursor_pos);
}

/**
 * @brief Shows the selection of the editable object
 * @param editable an editable object
 */
void etk_editable_selection_show(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (sd->selection_visible)
      return;

   sd->selection_visible = ETK_TRUE;
   if (evas_object_visible_get(editable) && sd->cursor_pos != sd->selection_pos)
      evas_object_show(sd->selection_object);
}

/**
 * @brief Hides the selection of the editable object
 * @param editable an editable object
 */
void etk_editable_selection_hide(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (!sd->selection_visible)
      return;

   sd->selection_visible = ETK_FALSE;
   evas_object_hide(sd->selection_object);
}

/**
 * @brief Gets the cursor position at the coords ( @a x, @a y ). It's used to know
 * where to place the cursor or the selection bound on mouse evevents.
 * @param editable an editable object
 * @param x the x coord, relative to the editable object
 * @param y the y coord, relative to the editable object
 * @return Returns the position where to place the cursor according to the given coords, or -1 on failure
 */
int etk_editable_pos_get_from_coords(Evas_Object *editable, int x, int y)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Object *text_obj;
   int  ox, oy;
   int tx, ty, tw, th;
   int cx, cw;
   int canvas_x, canvas_y;
   int index, pos, i, j;
   const char *text;

   if ((!editable) || (!(sd = evas_object_smart_data_get(editable))))
      return 0;
   if (!(text_obj = edje_object_part_object_get(sd->text_object, "etk.text.text")))
      return 0;

   evas_object_geometry_get(editable, &ox, &oy, NULL, NULL);
   evas_object_geometry_get(text_obj, &tx, &ty, &tw, &th);
   canvas_x = ox + x;
   canvas_y = oy + y;

   if (((canvas_y + SELECTION_MARGIN) < ty) || (canvas_x < tx))
      pos = 0;
   else if (((canvas_y - SELECTION_MARGIN) > (ty + th)) || (canvas_x > (tx + tw)))
      pos = sd->unicode_length;
   else
   {
      index = evas_object_text_char_coords_get(text_obj, canvas_x - tx, (th / 2), &cx, NULL, &cw, NULL);
      text = evas_object_text_text_get(text_obj);
      if (index >= 0 && text)
      {
         if ((canvas_x - tx) > (cx + (cw / 2)))
            index++;

         i = 0;
         j = -1;
         pos = 0;
         while (i < index && j != i)
         {
            pos++;
            j = i;
            i = evas_string_char_next_get(text, i, NULL);
         }

         if (pos > sd->unicode_length)
            pos = sd->unicode_length;
      }
      else
         pos = 0;
   }

   return pos;
}

/**
 * @brief A utility function to get the average size of a character written inside the editable object
 * @param editable an editable object
 * @param w the location where to store the average width of a character
 * @param h the location where to store the average height of a character
 */
void etk_editable_char_size_get(Evas_Object *editable, int *w, int *h)
{
   int tw = 0, th = 0;
   Evas *evas;
   Evas_Object *text_obj;
   Etk_Editable_Smart_Data *sd;
   char *text = "Tout est bon dans l'abricot sauf le noyau!"
                "Wakey wakey! Eggs and Bakey!";
   const char *font, *font_source;
   Evas_Text_Style_Type style;
   int font_size;

   if (w)   *w = 0;
   if (h)   *h = 0;

   if (!editable || !(evas = evas_object_evas_get(editable)))
      return;
   if (!(sd = evas_object_smart_data_get(editable)))
      return;
   if (!(text_obj = edje_object_part_object_get(sd->text_object, "etk.text.text")))
      return;

   if (sd->average_char_w <= 0 || sd->average_char_h <= 0)
   {
      font_source = evas_object_text_font_source_get(text_obj);
      evas_object_text_font_get(text_obj, &font, &font_size);
      style = evas_object_text_style_get(text_obj);

      text_obj = evas_object_text_add(evas);
      evas_object_text_font_source_set(text_obj, font_source);
      evas_object_text_font_set(text_obj, font, font_size);
      evas_object_text_style_set(text_obj, style);
      evas_object_text_text_set(text_obj, text);
      evas_object_geometry_get(text_obj, NULL, NULL, &tw, &th);
      evas_object_del(text_obj);

      sd->average_char_w = tw / strlen(text);
      sd->average_char_h = th;
   }

   if (w)   *w = sd->average_char_w;
   if (h)   *h = sd->average_char_h;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* A utility function to insert some text inside the editable object.
 * It doesn't update the position of the cursor, nor the selection */
static int _etk_editable_text_insert(Evas_Object *editable, int pos, const char *text)
{
   Etk_Editable_Smart_Data *sd;
   const char *cur_text;
   int char_length, unicode_length;
   int index;
   int i;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return 0;
   if (!text || *text == '\0')
   {
      _etk_editable_text_update(editable);
      return 0;
   }

   pos = ETK_CLAMP(pos, 0, sd->unicode_length);

   char_length = -1;
   unicode_length = -1;
   for (i = 0; i != char_length; i = evas_string_char_next_get(text, i, NULL))
   {
      char_length = i;
      unicode_length++;
   }

   index = 0;
   if ((cur_text = etk_string_get(sd->text)))
   {
      for (i = 0; i < pos; i++)
         index = evas_string_char_next_get(cur_text, index, NULL);
   }

   etk_string_insert_sized(sd->text, index, text, char_length);
   sd->unicode_length += unicode_length;
   _etk_editable_text_update(editable);

   return unicode_length;
}

/* A utility function to delete a range of text from the editable object.
 * It doesn't update the position of the cursor, nor the selection */
static int _etk_editable_text_delete(Evas_Object *editable, int start, int end)
{
   Etk_Editable_Smart_Data *sd;
   const char *cur_text;
   int start_id, end_id;
   int i;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return 0;
   if (!(cur_text = etk_string_get(sd->text)))
      return 0;

   start = ETK_CLAMP(start, 0, sd->unicode_length);
   end = ETK_CLAMP(end, 0, sd->unicode_length);
   if (end <= start)
      return 0;

   start_id = 0;
   end_id = 0;
   for (i = 0; i < end; i++)
   {
      end_id = evas_string_char_next_get(cur_text, end_id, NULL);
      if (i < start)
         start_id = end_id;
   }

   if (end_id <= start_id)
      return 0;

   etk_string_delete(sd->text, start_id, end_id - start_id);
   sd->unicode_length -= (end - start);
   _etk_editable_text_update(editable);

   return end - start;
}

/* Updates the position of the cursor.
 * It also updates automatically the text position and the selection */
static void _etk_editable_cursor_update(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Object *text_obj;
   int tx, ty;
   int cx, cy, ch;
   int w, h;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (!(text_obj = edje_object_part_object_get(sd->text_object, "etk.text.text")))
      return;

   evas_object_geometry_get(text_obj, &tx, &ty, NULL, NULL);
   _etk_editable_char_geometry_get_from_pos(editable, sd->cursor_pos, &cx, &cy, NULL, &ch);

   evas_object_move(sd->cursor_object, tx + cx, ty + cy - 1);
   evas_object_resize(sd->cursor_object, sd->cursor_width, ch);

   if (sd->cursor_visible && evas_object_visible_get(editable))
   {
      evas_object_show(sd->cursor_object);
      edje_object_signal_emit(sd->cursor_object, "etk,action,show,cursor", "etk");
   }

   evas_object_geometry_get(editable, NULL, NULL, &w, &h);
   _etk_editable_text_position_update(editable, w, h);
   _etk_editable_selection_update(editable);
}

/* Updates the selection of the editable object */
static void _etk_editable_selection_update(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Object *text_obj;
   int tx, ty;
   int cx, cy;
   int sx, sy, sw, sh;
   int start_pos, end_pos;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   if (!(text_obj = edje_object_part_object_get(sd->text_object, "etk.text.text")))
      return;

   if (sd->cursor_pos == sd->selection_pos || !sd->selection_visible)
      evas_object_hide(sd->selection_object);
   else
   {
      evas_object_geometry_get(text_obj, &tx, &ty, NULL, NULL);

      start_pos = ETK_MIN(sd->cursor_pos, sd->selection_pos);
      end_pos = ETK_MAX(sd->cursor_pos, sd->selection_pos);

      _etk_editable_char_geometry_get_from_pos(editable, start_pos, &cx, &cy, NULL, NULL);
      sx = tx + cx;
      sy = ty + cy;

      _etk_editable_char_geometry_get_from_pos(editable, end_pos, &cx, NULL, NULL, &sh);
      sw = tx + cx - sx;

      evas_object_move(sd->selection_object, sx, sy - 1);
      evas_object_resize(sd->selection_object, sw, sh);
      evas_object_show(sd->selection_object);
   }
}

/* Updates the text of the text object of the editable object
 * (it fills it with '*' if the editable is in password mode)
 * It does not update the position of the text */
static void _etk_editable_text_update(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;
   int minw, minh;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;

   if (sd->password_mode)
   {
      char *text;

      text = malloc(sd->unicode_length + 1);
      memset(text, '*', sd->unicode_length);
      text[sd->unicode_length] = '\0';
      edje_object_part_text_set(sd->text_object, "etk.text.text", text);
      free(text);
   }
   else
      edje_object_part_text_set(sd->text_object, "etk.text.text", etk_string_get(sd->text));

   edje_object_size_min_calc(sd->text_object, &minw, &minh);
   evas_object_resize(sd->text_object, minw, minh);
}

/* Updates the position of the text object according to the position
 * of the cursor (to make sure the cursor is visible) */
static void _etk_editable_text_position_update(Evas_Object *editable, int ow, int oh)
{
   Etk_Editable_Smart_Data *sd;
   int ox, oy;
   int tx, ty, tw, th, new_ty;
   int cx, cy, cw;
   int sx, sy;
   int offset_x = 0;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;

   evas_object_geometry_get(editable, &ox, &oy, NULL, NULL);
   evas_object_geometry_get(sd->text_object, &tx, &ty, &tw, &th);
   evas_object_geometry_get(sd->cursor_object, &cx, &cy, &cw, NULL);
   evas_object_geometry_get(sd->selection_object, &sx, &sy, NULL, NULL);

   if (tw <= ow)
      offset_x = ox - tx;
   else if (cx < (ox + CURSOR_MARGIN))
      offset_x = ox + CURSOR_MARGIN - cx;
   else if ((cx + cw + CURSOR_MARGIN) > (ox + ow))
      offset_x = (ox + ow) - (cx + cw + CURSOR_MARGIN);

   if (tw > ow)
   {
      if ((tx + offset_x) > ox)
         offset_x = ox - tx;
      else if ((tx + tw + offset_x) < (ox + ow))
         offset_x = (ox + ow) - (tx + tw);
   }
   else
      offset_x += (ow - tw) * sd->align;

   new_ty = oy + ((oh - th) / 2) + 1;
   evas_object_move(sd->text_object, tx + offset_x, new_ty);
   evas_object_move(sd->cursor_object, cx + offset_x, cy + (new_ty - ty));
   evas_object_move(sd->selection_object, sx + offset_x, sy + (new_ty - ty));
}

/* Gets the geometry of the character according to its utf-8 pos */
static int _etk_editable_char_geometry_get_from_pos(Evas_Object *editable, int pos, int *cx, int *cy, int *cw, int *ch)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Object *text_obj;
   const char *text;
   int x, w;
   int index, i;
   int last_pos;
   int ret;

   if (cx)   *cx = 0;
   if (cy)   *cy = 0;
   if (cw)   *cw = 0;
   if (ch)   *ch = 0;

   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return 0;
   if (!(text_obj = edje_object_part_object_get(sd->text_object, "etk.text.text")))
      return 0;

   text = evas_object_text_text_get(text_obj);
   if (!text || sd->unicode_length <= 0 || pos <= 0)
   {
      if (cx)   *cx = 0;
      if (cy)   *cy = 0;
      etk_editable_char_size_get(editable, cw, ch);
      return 1;
   }
   else
   {
      if (pos >= sd->unicode_length)
      {
         pos = sd->unicode_length - 1;
         last_pos = 1;
      }
      else
         last_pos = 0;

      index = 0;
      for (i = 0; i < pos; i++)
         index = evas_string_char_next_get(text, index, NULL);

      ret = evas_object_text_char_pos_get(text_obj, index, &x, cy, &w, ch);
      if (cx)   *cx = x - 1 + (last_pos ? w : 0);
      if (cw)   *cw = last_pos ? 1 : w;
      return ret;
   }
}


/**************************
 *
 * Editable object's smart methods
 *
 **************************/

/* Creates a new editable object */
static void _etk_editable_smart_add(Evas_Object *object)
{
   Evas *evas;
   Etk_Editable_Smart_Data *sd;
   int ox, oy;

   if (!object || !(evas = evas_object_evas_get(object)))
      return;

   sd = malloc(sizeof(Etk_Editable_Smart_Data));
   if (!sd)
      return;

   _etk_editable_smart_use++;
   evas_object_smart_data_set(object, sd);
   evas_object_geometry_get(object, &ox, &oy, NULL, NULL);

   sd->text = etk_string_new(NULL);
   sd->unicode_length = 0;

   sd->cursor_width = 1;
   sd->selection_on_fg = ETK_FALSE;
   sd->average_char_w = -1;
   sd->average_char_h = -1;

   sd->align = 0.0;
   sd->cursor_pos = 0;
   sd->cursor_visible = ETK_TRUE;
   sd->selection_pos = 0;
   sd->selection_visible = ETK_TRUE;
   sd->password_mode = ETK_FALSE;
   sd->disabled = ETK_FALSE;

   sd->clip_object = evas_object_rectangle_add(evas);
   evas_object_move(sd->clip_object, ox, oy);
   evas_object_smart_member_add(sd->clip_object, object);

   sd->event_object = evas_object_rectangle_add(evas);
   evas_object_color_set(sd->event_object, 255, 255, 255, 0);
   evas_object_clip_set(sd->event_object, sd->clip_object);
   evas_object_move(sd->event_object, ox, oy);
   evas_object_smart_member_add(sd->event_object, object);

   sd->text_object = edje_object_add(evas);
   evas_object_pass_events_set(sd->text_object, 1);
   evas_object_clip_set(sd->text_object, sd->clip_object);
   evas_object_move(sd->text_object, ox, oy);
   evas_object_smart_member_add(sd->text_object, object);

   sd->selection_object = edje_object_add(evas);
   evas_object_pass_events_set(sd->selection_object, 1);
   evas_object_clip_set(sd->selection_object, sd->clip_object);
   evas_object_move(sd->selection_object, ox, oy);
   evas_object_smart_member_add(sd->selection_object, object);

   sd->cursor_object = edje_object_add(evas);
   evas_object_pass_events_set(sd->cursor_object, 1);
   evas_object_clip_set(sd->cursor_object, sd->clip_object);
   evas_object_move(sd->cursor_object, ox, oy);
   evas_object_smart_member_add(sd->cursor_object, object);

   _etk_editable_cursor_update(object);
}

/* Deletes the editable object */
static void _etk_editable_smart_del(Evas_Object *object)
{
   Etk_Editable_Smart_Data *sd;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;

   evas_object_del(sd->clip_object);
   evas_object_del(sd->event_object);
   evas_object_del(sd->text_object);
   evas_object_del(sd->cursor_object);
   evas_object_del(sd->selection_object);

   etk_object_destroy(ETK_OBJECT(sd->text));
   free(sd);

   _etk_editable_smart_use--;
   if (_etk_editable_smart_use <= 0)
   {
      evas_smart_free(_etk_editable_smart);
      _etk_editable_smart = NULL;
   }
}

/* Moves the editable object */
static void _etk_editable_smart_move(Evas_Object *object, int x, int y)
{
   Etk_Editable_Smart_Data *sd;
   int prev_x, prev_y;
   int ox, oy;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;

   evas_object_geometry_get(object, &prev_x, &prev_y, NULL, NULL);

   evas_object_move(sd->clip_object, x, y);
   evas_object_move(sd->event_object, x, y);

   evas_object_geometry_get(sd->text_object, &ox, &oy, NULL, NULL);
   evas_object_move(sd->text_object, ox + (x - prev_x), oy + (y - prev_y));

   evas_object_geometry_get(sd->cursor_object, &ox, &oy, NULL, NULL);
   evas_object_move(sd->cursor_object, ox + (x - prev_x), oy + (y - prev_y));

   evas_object_geometry_get(sd->selection_object, &ox, &oy, NULL, NULL);
   evas_object_move(sd->selection_object, ox + (x - prev_x), oy + (y - prev_y));
}

/* Resizes the editable object */
static void _etk_editable_smart_resize(Evas_Object *object, int w, int h)
{
   Etk_Editable_Smart_Data *sd;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;

   evas_object_resize(sd->clip_object, w, h);
   evas_object_resize(sd->event_object, w, h);
   _etk_editable_text_position_update(object, w, h);
}

/* Shows the editable object */
static void _etk_editable_smart_show(Evas_Object *object)
{
   Etk_Editable_Smart_Data *sd;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;

   evas_object_show(sd->clip_object);
   evas_object_show(sd->event_object);
   evas_object_show(sd->text_object);

   if (sd->cursor_visible)
   {
      evas_object_show(sd->cursor_object);
      edje_object_signal_emit(sd->cursor_object, "etk,action,show,cursor", "etk");
   }

   if (sd->selection_visible && sd->cursor_pos != sd->selection_pos)
      evas_object_show(sd->selection_object);
}

/* Hides the editable object */
static void _etk_editable_smart_hide(Evas_Object *object)
{
   Etk_Editable_Smart_Data *sd;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;

   evas_object_hide(sd->clip_object);
   evas_object_hide(sd->event_object);
   evas_object_hide(sd->text_object);
   evas_object_hide(sd->cursor_object);
   evas_object_hide(sd->selection_object);
}

/* Changes the color of the editable object */
static void _etk_editable_color_set(Evas_Object *object, int r, int g, int b, int a)
{
   Etk_Editable_Smart_Data *sd;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;
   evas_object_color_set(sd->clip_object, r, g, b, a);
}

/* Clips the editable object against "clip" */
static void _etk_editable_clip_set(Evas_Object *object, Evas_Object *clip)
{
   Etk_Editable_Smart_Data *sd;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;
   evas_object_clip_set(sd->clip_object, clip);
}

/* Unclips the editable object */
static void _etk_editable_clip_unset(Evas_Object *object)
{
   Etk_Editable_Smart_Data *sd;

   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;
   evas_object_clip_unset(sd->clip_object);
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Editable
 *
 * You will almost never have to use directly an editable object, except if you are implementing a new widget that
 * needs an editable single-line.
 */
