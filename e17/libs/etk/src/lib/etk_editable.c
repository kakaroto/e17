/** @file etk_editable.c */
#include "etk_editable.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include "etk_string.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Editable
 * @{
 */
 
#define ETK_EDITABLE_CURSOR_SHOW_DELAY 1.25
#define ETK_EDITABLE_CURSOR_HIDE_DELAY 0.25
#define ETK_EDITABLE_CURSOR_MARGIN 5

typedef struct Etk_Editable_Smart_Data
{
   Evas_Object *clip_object;
   Evas_Object *event_object;
   Evas_Object *text_object;
   Evas_Object *cursor_object;
   Evas_Object *selection_object;

   Ecore_Timer *cursor_timer;
   int cursor_pos;
   Etk_Bool cursor_visible;
   int selection_pos;
   Etk_Bool selection_visible;
   Etk_Bool password_mode;
   
   Etk_String *text;
   int unicode_length;
   
   char *font;
   int font_size;
   Evas_Text_Style_Type font_style;
   int cursor_width;
   Etk_Bool selection_on_fg;
   
   int average_char_w;
   int average_char_h;
} Etk_Editable_Smart_Data;


static int _etk_editable_text_insert(Evas_Object *editable, int pos, const char *text);
static int _etk_editable_text_delete(Evas_Object *editable, int start, int end);
static void _etk_editable_cursor_update(Evas_Object *editable);
static void _etk_editable_selection_update(Evas_Object *editable);
static void _etk_editable_text_update(Evas_Object *editable);
static void _etk_editable_text_position_update(Evas_Object *editable, Evas_Coord real_w);
static int _etk_editable_cursor_timer_cb(void *data);

static void _etk_editable_smart_add(Evas_Object *object);
static void _etk_editable_smart_del(Evas_Object *object);
static void _etk_editable_smart_move(Evas_Object *object, Evas_Coord x, Evas_Coord y);
static void _etk_editable_smart_resize(Evas_Object *object, Evas_Coord w, Evas_Coord h);
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
      _etk_editable_smart = evas_smart_new("Editable_Object",
         _etk_editable_smart_add,
         _etk_editable_smart_del,
         NULL, NULL, NULL, NULL, NULL,
         _etk_editable_smart_move,
         _etk_editable_smart_resize,
         _etk_editable_smart_show,
         _etk_editable_smart_hide,
         _etk_editable_color_set,
         _etk_editable_clip_set,
         _etk_editable_clip_unset,
         NULL);
        _etk_editable_smart_use = 0;
   }
   
   return evas_object_smart_add(evas, _etk_editable_smart);
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
   
   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return ETK_FALSE;
   
   unicode_length = _etk_editable_text_insert(editable, pos, text);
   if (unicode_length <= 0)
      return ETK_FALSE;
   
   if (sd->cursor_pos >= pos)
      etk_editable_cursor_pos_set(editable, sd->cursor_pos + unicode_length);
   if (sd->selection_pos >= pos)
      etk_editable_selection_pos_set(editable, sd->selection_pos + unicode_length);
   
   _etk_editable_text_position_update(editable, -1);
   return ETK_TRUE;
}

/**
 * @brief Deletes the text of the editable object, between position @a start and position @a end
 * @param editable the editable object in which the text should be deleted
 * @param start the position of the first char to delete
 * @param end the position of the last char to delete
 * @return Returns ETK_TRUE if the text has been modified, ETK_FALSE otherwise
 */
Etk_Bool etk_editable_delete(Evas_Object *editable, int start, int end)
{
   Etk_Editable_Smart_Data *sd;
   int unicode_length;
   
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
   
   _etk_editable_text_position_update(editable, -1);
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
      sd->cursor_timer = ecore_timer_add(ETK_EDITABLE_CURSOR_SHOW_DELAY, _etk_editable_cursor_timer_cb, editable);
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
   if (sd->cursor_timer)
   {
      ecore_timer_del(sd->cursor_timer);
      sd->cursor_timer = NULL;
   }
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
 * @brief Gets the cursor position at the coords ( @a x, @a y ). It's used to know
 * where to place the cursor or the selection bound on mouse evevents.
 * @param editable an editable object
 * @param x the x coord, relative to the editable object
 * @param y the y coord, relative to the editable object
 * @return Returns the position where to place the cursor according to the given coords, or -1 on failure
 */
int etk_editable_pos_get_from_coords(Evas_Object *editable, Evas_Coord x, Evas_Coord y)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Coord ox, oy;
   Evas_Coord tx, ty, tw, th;
   Evas_Coord cx, cw;
   Evas_Coord canvas_x, canvas_y;
   int pos;
   
   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return -1;
   
   evas_object_geometry_get(editable, &ox, &oy, NULL, NULL);
   evas_object_geometry_get(sd->text_object, &tx, &ty, &tw, &th);
   canvas_x = ox + x;
   canvas_y = oy + y;
   
   if (canvas_y < ty || canvas_x < tx)
      pos = 0;
   else if (canvas_y > (ty + th) || canvas_x > (tx + tw))
      pos = sd->unicode_length;
   else
   {
      pos = evas_object_text_char_coords_get(sd->text_object, canvas_x - tx, canvas_y - ty, &cx, NULL, &cw, NULL);
      if (pos >= 0)
      {
         if ((canvas_x - tx) > (cx + (cw / 2)))
            pos++;
         if (pos > sd->unicode_length)
            pos = sd->unicode_length;
      }
      else
         pos = -1;
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
   Etk_Editable_Smart_Data *sd;
   Evas_Object *text_object;
   char *text = "Tout est bon dans l'abricot sauf le noyau!"
                "Wakey wakey! Eggs and Bakey!";

   if (w)   *w = 0;
   if (h)   *h = 0;
   
   if (!editable || !(evas = evas_object_evas_get(editable)))
      return;
   if (!(sd = evas_object_smart_data_get(editable)) || !sd->font)
      return;
   
   if (sd->average_char_w <= 0 || sd->average_char_h <= 0)
   {
      text_object = evas_object_text_add(evas);
      evas_object_text_font_set(text_object, sd->font, sd->font_size);
      evas_object_text_style_set(text_object, sd->font_style);
      evas_object_text_text_set(text_object, text);
      evas_object_geometry_get(text_object, NULL, NULL, &tw, &th);
      evas_object_del(text_object);
      
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
      return 0;
   
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
   
   if (unicode_length <= 0 || char_length <= 0)
      return 0;
   
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
   
   etk_string_delete(sd->text, start, end - start);
   sd->unicode_length -= (end - start);
   _etk_editable_text_update(editable);
   
   return end - start;
}

/* Updates the position of the cursor.
 * It also updates automatically the text position and the selection */
static void _etk_editable_cursor_update(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Coord tx, ty;
   Evas_Coord cx, cy, cw, ch;
   
   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   
   evas_object_geometry_get(sd->text_object, &tx, &ty, NULL, NULL);
   
   if (sd->unicode_length <= 0 || sd->cursor_pos <= 0)
   {
      etk_editable_char_size_get(editable, &cw, &ch);
      evas_object_move(sd->cursor_object, tx, ty);
      evas_object_resize(sd->cursor_object, 1, ch);
   }
   else
   {
      if (sd->cursor_pos >= sd->unicode_length)
      {
         evas_object_text_char_pos_get(sd->text_object, sd->unicode_length - 1, &cx, &cy, &cw, &ch);
         evas_object_move(sd->cursor_object, tx + cx + cw - 1, ty + cy);
         evas_object_resize(sd->cursor_object, 1, ch);
      }
      else
      {
         evas_object_text_char_pos_get(sd->text_object, sd->cursor_pos, &cx, &cy, &cw, &ch);
         evas_object_move(sd->cursor_object, tx + cx - 1, ty + cy);
         evas_object_resize(sd->cursor_object, 1, ch);
      }
   }
   
   if (sd->cursor_timer)
   {
      evas_object_show(sd->cursor_object);
      ecore_timer_interval_set(sd->cursor_timer, ETK_EDITABLE_CURSOR_SHOW_DELAY);
   }
   
   _etk_editable_selection_update(editable);
   _etk_editable_text_position_update(editable, -1);
}

/* Updates the selection of the editable object */
static void _etk_editable_selection_update(Evas_Object *editable)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Coord tx, ty;
   Evas_Coord cx, cy, cw, ch;
   Evas_Coord sx, sy, sw, sh;
   int start_pos, end_pos;
   
   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   
   if (sd->cursor_pos == sd->selection_pos || !sd->selection_visible)
      evas_object_hide(sd->selection_object);
   else
   {
      evas_object_geometry_get(sd->text_object, &tx, &ty, NULL, NULL);
      
      start_pos = ETK_MIN(sd->cursor_pos, sd->selection_pos);
      end_pos = ETK_MAX(sd->cursor_pos, sd->selection_pos);
      
      /* Position of the start cursor (note, the start cursor can not be at
       * the end of the editable object, and the editable object can not be
       * empty, or it would have returned before)*/
      evas_object_text_char_pos_get(sd->text_object, start_pos, &cx, &cy, &cw, &ch);
      sx = tx + cx - 1;
      sy = ty + cy;
      
      /* Position of the end cursor (note, the editable object can not be
       * empty, or it would have returned before)*/
      if (end_pos >= sd->unicode_length)
      {
         evas_object_text_char_pos_get(sd->text_object, sd->unicode_length - 1, &cx, &cy, &cw, &ch);
         sw = (tx + cx + cw - 1) - sx;
         sh = ch;
      }
      else
      {
         evas_object_text_char_pos_get(sd->text_object, end_pos, &cx, &cy, &cw, &ch);
         sw = (tx + cx - 1) - sx;
         sh = ch;
      }
      
      evas_object_move(sd->selection_object, sx, sy);
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
   
   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   
   if (sd->password_mode)
   {
      char *text;
      
      text = malloc(sd->unicode_length + 1);
      memset(text, '*', sd->unicode_length);
      text[sd->unicode_length] = '\0';
      evas_object_text_text_set(sd->text_object, text);
      free(text);
   }
   else
      evas_object_text_text_set(sd->text_object, etk_string_get(sd->text));
   
}

/* Updates the position of the text object according to the position
 * of the cursor (to make sure the cursor is visible) */
static void _etk_editable_text_position_update(Evas_Object *editable, Evas_Coord real_w)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Coord ox, oy, ow;
   Evas_Coord tx, ty, tw;
   Evas_Coord cx, cy, cw;
   Evas_Coord sx, sy;
   Evas_Coord offset_x = 0;
   
   if (!editable || !(sd = evas_object_smart_data_get(editable)))
      return;
   
   evas_object_geometry_get(editable, &ox, &oy, &ow, NULL);
   if (real_w >= 0)
      ow = real_w;
   evas_object_geometry_get(sd->text_object, &tx, &ty, &tw, NULL);
   evas_object_geometry_get(sd->cursor_object, &cx, &cy, &cw, NULL);
   evas_object_geometry_get(sd->selection_object, &sx, &sy, NULL, NULL);
   
   if (tw <= ow)
      offset_x = ox - tx;
   else if (cx < (ox + ETK_EDITABLE_CURSOR_MARGIN))
      offset_x = ox + ETK_EDITABLE_CURSOR_MARGIN - cx;
   else if ((cx + cw + ETK_EDITABLE_CURSOR_MARGIN) > (ox + ow))
      offset_x = (ox + ow) - (cx + cw + ETK_EDITABLE_CURSOR_MARGIN);
     
   if (tw > ow)
   {
      if ((tx + offset_x) > ox)
         offset_x = ox - tx;
      else if ((tx + tw + offset_x) < (ox + ow))
         offset_x = (ox + ow) - (tx + tw);
   }
   
   if (offset_x != 0)
   {
      evas_object_move(sd->text_object, tx + offset_x, ty);
      evas_object_move(sd->cursor_object, cx + offset_x, cy);
      evas_object_move(sd->selection_object, sx + offset_x, sy);
   }
}

/* Shows/hides the cursor on regular interval */
static int _etk_editable_cursor_timer_cb(void *data)
{
   Evas_Object *editable;
   Etk_Editable_Smart_Data *sd;
   
   if (!(editable = data) || !(sd = evas_object_smart_data_get(editable)))
      return 1;
   
   if (evas_object_visible_get(sd->cursor_object))
   {
      evas_object_hide(sd->cursor_object);
      ecore_timer_interval_set(sd->cursor_timer, ETK_EDITABLE_CURSOR_HIDE_DELAY);
   }
   else
   {
      evas_object_show(sd->cursor_object);
      ecore_timer_interval_set(sd->cursor_timer, ETK_EDITABLE_CURSOR_SHOW_DELAY);
   }
   
   return 1;
}

/* Editable object's smart methods */

/* Creates a new editable object */
static void _etk_editable_smart_add(Evas_Object *object)
{
   Evas *evas;
   Etk_Editable_Smart_Data *sd;
   
   if (!object || !(evas = evas_object_evas_get(object)))
      return;

   sd = malloc(sizeof(Etk_Editable_Smart_Data));
   evas_object_smart_data_set(object, sd);
   
   sd->text = etk_string_new(NULL);
   sd->unicode_length = 0;
   
   /* TODO: themability! */
   sd->font = strdup("Vera");
   sd->font_size = 10;
   sd->font_style = EVAS_TEXT_STYLE_PLAIN;
   sd->cursor_width = 1;
   sd->selection_on_fg = ETK_FALSE;
   sd->average_char_w = -1;
   sd->average_char_h = -1;
   
   sd->cursor_timer = NULL;
   sd->cursor_pos = 0;
   sd->cursor_visible = ETK_TRUE;
   sd->selection_pos = 0;
   sd->selection_visible = ETK_TRUE;
   sd->password_mode = ETK_FALSE;

   sd->clip_object = evas_object_rectangle_add(evas);
   evas_object_smart_member_add(sd->clip_object, object);
   
   sd->event_object = evas_object_rectangle_add(evas);
   evas_object_color_set(sd->event_object, 255, 255, 255, 0);
   evas_object_clip_set(sd->event_object, sd->clip_object);
   evas_object_smart_member_add(sd->event_object, object);
   
   sd->text_object = evas_object_text_add(evas);
   evas_object_text_font_set(sd->text_object, sd->font, sd->font_size);
   evas_object_text_style_set(sd->text_object, sd->font_style);
   evas_object_color_set(sd->text_object, 0, 0, 0, 255);
   evas_object_clip_set(sd->text_object, sd->clip_object);
   evas_object_smart_member_add(sd->text_object, object);
   
   /* TODO: */
   //sd->selection_object = edje_object_add(evas);
   sd->selection_object = evas_object_rectangle_add(evas);
   evas_object_color_set(sd->selection_object, 245, 205, 109, 102);
   evas_object_clip_set(sd->selection_object, sd->clip_object);
   evas_object_smart_member_add(sd->selection_object, object);
   
   /* TODO: */
   //sd->cursor_object = edje_object_add(evas);
   sd->cursor_object = evas_object_rectangle_add(evas);
   evas_object_color_set(sd->cursor_object, 0, 0, 0, 255);
   evas_object_clip_set(sd->cursor_object, sd->clip_object);
   evas_object_smart_member_add(sd->cursor_object, object);
   
   _etk_editable_cursor_update(object);
   _etk_editable_smart_use++;
}

/* Deletes the editable object */
static void _etk_editable_smart_del(Evas_Object *object)
{
   Etk_Editable_Smart_Data *sd;
   
   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;
   
   free(sd->font);
   
   evas_object_del(sd->clip_object);
   evas_object_del(sd->event_object);
   evas_object_del(sd->text_object);
   evas_object_del(sd->cursor_object);
   evas_object_del(sd->selection_object);
   if (sd->cursor_timer)
      ecore_timer_del(sd->cursor_timer);
   
   free(sd);
   
   _etk_editable_smart_use--;
   if (_etk_editable_smart_use <= 0)
   {
      evas_smart_free(_etk_editable_smart);
      _etk_editable_smart = NULL;
   }
}

/* Moves the editable object */
static void _etk_editable_smart_move(Evas_Object *object, Evas_Coord x, Evas_Coord y)
{
   Etk_Editable_Smart_Data *sd;
   Evas_Coord prev_x, prev_y;
   Evas_Coord ox, oy;
   
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
static void _etk_editable_smart_resize(Evas_Object *object, Evas_Coord w, Evas_Coord h)
{
   Etk_Editable_Smart_Data *sd;
   
   if (!object || !(sd = evas_object_smart_data_get(object)))
      return;
   
   evas_object_resize(sd->clip_object, w, h);
   evas_object_resize(sd->event_object, w, h);
   _etk_editable_text_position_update(object, w);
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
      if (sd->cursor_timer)
         ecore_timer_interval_set(sd->cursor_timer, ETK_EDITABLE_CURSOR_SHOW_DELAY);
      else
         sd->cursor_timer = ecore_timer_add(ETK_EDITABLE_CURSOR_SHOW_DELAY, _etk_editable_cursor_timer_cb, object);
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
   
   if (sd->cursor_timer)
   {
      ecore_timer_del(sd->cursor_timer);
      sd->cursor_timer = NULL;
   }
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
