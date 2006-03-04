/** @file etk_editable_text_object.c */
#include "etk_editable_text_object.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include "etk_types.h"
#include "etk_widget.h"

typedef struct _Etk_Editable_Text_Smart_Data
{
   Evas_Object *clip;
   Evas_Object *text_object;
   Evas_Object *cursor_object;
   Ecore_Timer *cursor_timer;
   
   Etk_Bool cursor_at_the_end;
   Etk_Bool show_cursor;
} Etk_Editable_Text_Smart_Data;

static Etk_Bool _etk_editable_text_is_empty(Evas_Object *object);
static void _etk_editable_text_cursor_position_update(Evas_Object *object, Etk_Geometry *object_geometry);
static void _etk_editable_text_cursor_visibility_update(Evas_Object *object);
static int _etk_editable_text_cursor_timer_cb(void *data);
static void _etk_editable_text_size_update(Evas_Object *object);

static void _etk_editable_text_smart_add(Evas_Object *object);
static void _etk_editable_text_smart_del(Evas_Object *object);
static void _etk_editable_text_smart_raise(Evas_Object *object);
static void _etk_editable_text_smart_lower(Evas_Object *object);
static void _etk_editable_text_smart_stack_above(Evas_Object *object, Evas_Object *above);
static void _etk_editable_text_smart_stack_below(Evas_Object *object, Evas_Object *below);
static void _etk_editable_text_smart_move(Evas_Object *object, Evas_Coord x, Evas_Coord y);
static void _etk_editable_text_smart_resize(Evas_Object *object, Evas_Coord w, Evas_Coord h);
static void _etk_editable_text_smart_show(Evas_Object *object);
static void _etk_editable_text_smart_hide(Evas_Object *object);
static void _etk_editable_clip_set(Evas_Object *object, Evas_Object *clip);
static void _etk_editable_clip_unset(Evas_Object *object);

static Evas_Smart *etk_editable_text_smart = NULL;
static Evas_Textblock_Style *_etk_editable_text_style = NULL;
static int _etk_editable_text_style_use_count = 0;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Creates a new editable text object
 * @param evas the evas where to create the object
 * @return Returns the new editable text object
 */
Evas_Object *etk_editable_text_object_add(Evas *evas)
{
   if (!etk_editable_text_smart)
   {
      etk_editable_text_smart = evas_smart_new("etk_editable_text_object",
         _etk_editable_text_smart_add, /* add */
         _etk_editable_text_smart_del, /* del */
         NULL, /* layer_set */
         _etk_editable_text_smart_raise, /* raise */
         _etk_editable_text_smart_lower, /* lower */
         _etk_editable_text_smart_stack_above, /* stack_above */
         _etk_editable_text_smart_stack_below, /* stack_below */
         _etk_editable_text_smart_move, /* move */
         _etk_editable_text_smart_resize, /* resize */
         _etk_editable_text_smart_show, /* show */
         _etk_editable_text_smart_hide, /* hide */
         NULL, /* color_set */
         _etk_editable_clip_set, /* clip_set */
         _etk_editable_clip_unset, /* clip_unset */
         NULL); /* data*/
   }
   return evas_object_smart_add(evas, etk_editable_text_smart);
}

/**
 * @brief Sets the text of the object
 * @param object an editable text object
 * @param text the text to set
 */
void etk_editable_text_object_text_set(Evas_Object *object, const char *text)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)) || !text)
      return;
   
   evas_object_textblock_text_markup_set(editable_text_sd->text_object, text);
   _etk_editable_text_size_update(object);
   etk_editable_text_object_cursor_move_at_end(object);
}

/**
 * @brief Inserts text at the cursor position of the object
 * @param object an editable text object
 * @param text the text to insert
 * @return Returns ETK_TRUE if the text has been changed
 */
Etk_Bool etk_editable_text_object_insert(Evas_Object *object, const char *text)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return ETK_FALSE;
   if (!text || (strlen(text) <= 1 && *text < 0x20))
      return ETK_FALSE;
   
   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);

   if (editable_text_sd->cursor_at_the_end)
      evas_textblock_cursor_text_append(cursor, text);
   else
      evas_textblock_cursor_text_prepend(cursor, text);
   _etk_editable_text_size_update(object);
   _etk_editable_text_cursor_position_update(object, NULL);

   return ETK_TRUE;
}

/**
 * @brief Gets the text of the editable text object
 * @param object an editable text object
 * @return Returns the text of the editable text object
 */
const char *etk_editable_text_object_text_get(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return NULL;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   return evas_textblock_cursor_node_text_get(cursor);
}

/**
 * @brief Deletes the char placed before the cursor
 * @param object an editable text object
 * @return Returns ETK_TRUE if the text has been changed
 */
Etk_Bool etk_editable_text_object_delete_char_before(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;
   Etk_Bool text_changed = ETK_FALSE;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)) || _etk_editable_text_is_empty(object))
      return ETK_FALSE;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);

   if (editable_text_sd->cursor_at_the_end || evas_textblock_cursor_char_prev(cursor))
   {
      evas_textblock_cursor_char_delete(cursor);
      text_changed = ETK_TRUE;
   }
   
   _etk_editable_text_size_update(object);
   _etk_editable_text_cursor_position_update(object, NULL);
   return text_changed;
}

/**
 * @brief Deletes the char placed after the cursor
 * @param object an editable text object
 * @return Returns ETK_TRUE if the text has been changed
 */
Etk_Bool etk_editable_text_object_delete_char_after(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;
   Etk_Bool text_changed = ETK_FALSE;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)) || _etk_editable_text_is_empty(object))
      return ETK_FALSE;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   
   if (!editable_text_sd->cursor_at_the_end)
   {
      if (!evas_textblock_cursor_char_next(cursor))
         editable_text_sd->cursor_at_the_end = ETK_TRUE;
      else
         evas_textblock_cursor_char_prev(cursor);
      evas_textblock_cursor_char_delete(cursor);
      text_changed = ETK_TRUE;
   }

   _etk_editable_text_size_update(object);
   _etk_editable_text_cursor_position_update(object, NULL);
   return text_changed;
}

/**
 * @brief Moves the cursor of the editable text object at the start of the text
 * @param object an editable text object
 */
void etk_editable_text_object_cursor_move_at_start(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)) || _etk_editable_text_is_empty(object))
      return;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   editable_text_sd->cursor_at_the_end = ETK_FALSE;
   evas_textblock_cursor_char_first(cursor);

   _etk_editable_text_cursor_position_update(object, NULL);
}

/**
 * @brief Moves the cursor of the editable text object at the end of the text
 * @param object an editable text object
 */
void etk_editable_text_object_cursor_move_at_end(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)) || _etk_editable_text_is_empty(object))
      return;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   editable_text_sd->cursor_at_the_end = ETK_TRUE;
   evas_textblock_cursor_char_last(cursor);

   _etk_editable_text_cursor_position_update(object, NULL);
}

/**
 * @brief Moves the cursor of the editable text object to the left
 * @param object an editable text object
 */
void etk_editable_text_object_cursor_move_left(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   if (editable_text_sd->cursor_at_the_end)
      editable_text_sd->cursor_at_the_end = ETK_FALSE;
   else
      evas_textblock_cursor_char_prev(cursor);
   
   _etk_editable_text_size_update(object);
   _etk_editable_text_cursor_position_update(object, NULL);
}

/**
 * @brief Moves the cursor of the editable text object to the right
 * @param object an editable text object
 */
void etk_editable_text_object_cursor_move_right(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   if (!evas_textblock_cursor_char_next(cursor))
      editable_text_sd->cursor_at_the_end = ETK_TRUE;

   _etk_editable_text_size_update(object);
   _etk_editable_text_cursor_position_update(object, NULL);
}

/**
 * @brief Shows the cursor of the editable text object
 * @param object the editable text object
 */
void etk_editable_text_object_cursor_show(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   editable_text_sd->show_cursor = ETK_TRUE;
   _etk_editable_text_cursor_visibility_update(object);
}

/**
 * @brief Hides the cursor of the editable text object
 * @param object the editable text object
 */
void etk_editable_text_object_cursor_hide(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   editable_text_sd->show_cursor = ETK_FALSE;
   _etk_editable_text_cursor_visibility_update(object);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Returns ETK_TRUE if the text of the editable object is empty */
static Etk_Bool _etk_editable_text_is_empty(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return ETK_TRUE;

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   return (evas_textblock_cursor_node_text_length_get(cursor) <= 0);
}

/* Updates the size of the text object: to be called when the text of the object is changed */
static void _etk_editable_text_size_update(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   int w, h;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_textblock_size_native_get(editable_text_sd->text_object, &w, &h);
   evas_object_resize(editable_text_sd->text_object, w, h);
}

/* Updates the cursor position: to be called when the cursor or the object are moved */
static void _etk_editable_text_cursor_position_update(Evas_Object *object, Etk_Geometry *object_geometry)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;
   Evas_Coord tx, ty, tw, th, cx, cy, cw, ch, ox, oy, ow, oh;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_geometry_get(editable_text_sd->text_object, &tx, &ty, &tw, &th);
   if (object_geometry)
   {
      ox = object_geometry->x;
      oy = object_geometry->y;
      ow = object_geometry->w;
      oh = object_geometry->h;
   }
   else
      evas_object_geometry_get(object, &ox, &oy, &ow, &oh);
   
   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
   
   if (_etk_editable_text_is_empty(object))
   {
      evas_object_move(editable_text_sd->cursor_object, ox, oy);
      evas_object_resize(editable_text_sd->cursor_object, 1, oh);
      return;
   }
   else if (editable_text_sd->cursor_at_the_end)
   {
      evas_textblock_cursor_char_last(cursor);
      evas_textblock_cursor_char_geometry_get(cursor, &cx, &cy, &cw, &ch);
      cx += tx + cw;
   }
   else
   {
      evas_textblock_cursor_char_geometry_get(cursor, &cx, &cy, &cw, &ch);
      cx += tx;
   }

   /* TODO: fix the cursor placement */
   if ((cx < ox + 20) && (cx - tx > 20))
   {
      evas_object_move(editable_text_sd->text_object, tx + ox + 20 - cx, ty);
      cx = ox + 20;
   }
   else if (cx < ox)
   {
      evas_object_move(editable_text_sd->text_object, tx + ox - cx, ty);
      cx = ox;
   }
   else if ((cx > ox + ow - 20) && (tw - (cx - tx) > 20))
   {
      evas_object_move(editable_text_sd->text_object, tx - cx + ox + ow - 20, ty);
      cx = ox + ow - 20;
   }
   else if (cx > ox + ow)
   {
      evas_object_move(editable_text_sd->text_object, tx - cx + ox + ow, ty);
      cx = ox + ow;
   }
   
   evas_object_move(editable_text_sd->cursor_object, cx, ty + cy);
   evas_object_resize(editable_text_sd->cursor_object, 1, ch);
}

/* Updates the visibility state of the cursor */
static void _etk_editable_text_cursor_visibility_update(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   
   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   if (!editable_text_sd->show_cursor)
   {
      if (editable_text_sd->cursor_timer)
      {
         ecore_timer_del(editable_text_sd->cursor_timer);
         editable_text_sd->cursor_timer = NULL;
      }
      evas_object_hide(editable_text_sd->cursor_object);
   }
   else
   {
      if (!editable_text_sd->cursor_timer)
      {
         editable_text_sd->cursor_timer = ecore_timer_add(0.75, _etk_editable_text_cursor_timer_cb, object);
         evas_object_show(editable_text_sd->cursor_object);
      }
   }
}

/* Make the cursor blink */
static int _etk_editable_text_cursor_timer_cb(void *data)
{
   Evas_Object *object;
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   
   if (!(object = data) || !(editable_text_sd = evas_object_smart_data_get(object)))
      return 1;

   if (evas_object_visible_get(editable_text_sd->cursor_object))
   {
      evas_object_hide(editable_text_sd->cursor_object);
      ecore_timer_interval_set(editable_text_sd->cursor_timer, 0.25);
   }
   else
   {
      evas_object_show(editable_text_sd->cursor_object);
      ecore_timer_interval_set(editable_text_sd->cursor_timer, 0.75);
   }

   return 1;
}

/* Called when the object is created */
static void _etk_editable_text_smart_add(Evas_Object *object)
{
   Evas *evas;
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Evas_Textblock_Cursor *cursor;

   if (!object || !(evas = evas_object_evas_get(object)))
      return;

   if (!_etk_editable_text_style)
   {
      _etk_editable_text_style = evas_textblock_style_new();
      evas_textblock_style_set(_etk_editable_text_style, "DEFAULT='font=Vera font_size=10 align=left color=#000000 wrap=word'");
      _etk_editable_text_style_use_count = 0;
   }

   editable_text_sd = malloc(sizeof(Etk_Editable_Text_Smart_Data));
   editable_text_sd->show_cursor = ETK_FALSE;
   editable_text_sd->cursor_at_the_end = ETK_TRUE;
   editable_text_sd->cursor_timer = NULL;

   editable_text_sd->text_object = evas_object_textblock_add(evas);
   evas_object_textblock_style_set(editable_text_sd->text_object, _etk_editable_text_style);
   _etk_editable_text_style_use_count++;
   evas_object_smart_member_add(editable_text_sd->text_object, object);

   editable_text_sd->clip = evas_object_rectangle_add(evas);
   evas_object_clip_set(editable_text_sd->text_object, editable_text_sd->clip);
   evas_object_smart_member_add(editable_text_sd->clip, object);

   editable_text_sd->cursor_object = evas_object_rectangle_add(evas);
   evas_object_color_set(editable_text_sd->cursor_object, 0, 0, 0, 255);
   evas_object_smart_member_add(editable_text_sd->cursor_object, object);

   evas_object_smart_data_set(object, editable_text_sd);

   cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(editable_text_sd->text_object);
	evas_textblock_cursor_node_first(cursor);
}

/* Called when the object is deleted */
static void _etk_editable_text_smart_del(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   if (editable_text_sd->cursor_timer)
      ecore_timer_del(editable_text_sd->cursor_timer);
   evas_object_del(editable_text_sd->text_object);
   evas_object_del(editable_text_sd->clip);
   evas_object_del(editable_text_sd->cursor_object);
   free(editable_text_sd);

   _etk_editable_text_style_use_count--;
   if (_etk_editable_text_style_use_count <= 0 && _etk_editable_text_style)
   {
      evas_textblock_style_free(_etk_editable_text_style);
      _etk_editable_text_style = NULL;
   }
}

/* Called when the object is stacked above all the other objects */
static void _etk_editable_text_smart_raise(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_raise(editable_text_sd->clip);
   evas_object_raise(editable_text_sd->text_object);
   evas_object_raise(editable_text_sd->cursor_object);
}

/* Called when the object is stacked below all the other objects */
static void _etk_editable_text_smart_lower(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_lower(editable_text_sd->cursor_object);
   evas_object_lower(editable_text_sd->text_object);
   evas_object_lower(editable_text_sd->clip);
}

/* Called when the object is stacked above another object */
static void _etk_editable_text_smart_stack_above(Evas_Object *object, Evas_Object *above)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !above || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_stack_above(editable_text_sd->clip, above);
   evas_object_stack_above(editable_text_sd->text_object, editable_text_sd->clip);
   evas_object_stack_above(editable_text_sd->cursor_object, editable_text_sd->text_object);
}

/* Called when the object is stacked below another object */
static void _etk_editable_text_smart_stack_below(Evas_Object *object, Evas_Object *below)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !below || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_stack_below(editable_text_sd->cursor_object, below);
   evas_object_stack_below(editable_text_sd->text_object, editable_text_sd->cursor_object);
   evas_object_stack_below(editable_text_sd->clip, editable_text_sd->text_object);
}

/* Called when the object is moved */
static void _etk_editable_text_smart_move(Evas_Object *object, Evas_Coord x, Evas_Coord y)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;
   Etk_Geometry object_geometry;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_move(editable_text_sd->clip, x, y);
   evas_object_move(editable_text_sd->text_object, x, y);
   object_geometry.x = x;
   object_geometry.y = y;
   evas_object_geometry_get(object, NULL, NULL, &object_geometry.w, &object_geometry.h);
   _etk_editable_text_cursor_position_update(object, &object_geometry);
}

/* Called when the object is resized */
static void _etk_editable_text_smart_resize(Evas_Object *object, Evas_Coord w, Evas_Coord h)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_resize(editable_text_sd->clip, w, h);
}

/* Called when the object is shown */
static void _etk_editable_text_smart_show(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_show(editable_text_sd->text_object);
   evas_object_show(editable_text_sd->clip);
   _etk_editable_text_cursor_visibility_update(object);
}

/* Called when the object is hidden */
static void _etk_editable_text_smart_hide(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;

   evas_object_hide(editable_text_sd->cursor_object);
   evas_object_hide(editable_text_sd->text_object);
   evas_object_hide(editable_text_sd->clip);
}

/* Called when the object is clipped */
static void _etk_editable_clip_set(Evas_Object *object, Evas_Object *clip)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;
   evas_object_clip_set(editable_text_sd->clip, clip);
}

/* Called when the object is unclipped */
static void _etk_editable_clip_unset(Evas_Object *object)
{
   Etk_Editable_Text_Smart_Data *editable_text_sd;

   if (!object || !(editable_text_sd = evas_object_smart_data_get(object)))
      return;
   evas_object_clip_unset(editable_text_sd->clip);
}
