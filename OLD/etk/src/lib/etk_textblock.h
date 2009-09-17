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

/** @file etk_textblock.h */
#ifndef _ETK_TEXTBLOCK_H_
#define _ETK_TEXTBLOCK_H_

#include <Evas.h>

#include "etk_object.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Textblock Etk_Textblock
 * @brief TODO: Etk_Textblock: brief description
 * @{
 */

/** Gets the type of a textblock */
#define ETK_TEXTBLOCK_TYPE       (etk_textblock_type_get())
/** Casts the object to an Etk_Textblock */
#define ETK_TEXTBLOCK(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXTBLOCK_TYPE, Etk_Textblock))
/** Check if the object is an Etk_Textblock */
#define ETK_IS_TEXTBLOCK(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXTBLOCK_TYPE))

/** @brief The different types of wrapping to apply on a textblock object */
typedef enum
{
   ETK_TEXTBLOCK_WRAP_NONE,     /**< The text is not wrapped */
   ETK_TEXTBLOCK_WRAP_DEFAULT,  /**< TODOC */
   ETK_TEXTBLOCK_WRAP_WORD,     /**< The text is wrapped between the words (or between the chars if it's not sufficient) */
   ETK_TEXTBLOCK_WRAP_CHAR      /**< The text is wrapped between the chars */
} Etk_Textblock_Wrap;

/** @brief The different types of node */
typedef enum
{
   ETK_TEXTBLOCK_NODE_ROOT,        /**< The node is the root node */
   ETK_TEXTBLOCK_NODE_PARAGRAPH,   /**< The node is a paragraph node */
   ETK_TEXTBLOCK_NODE_LINE,        /**< The node is a line node */
   ETK_TEXTBLOCK_NODE_NORMAL       /**< The node is a normal node (containing some text or a format) */
} Etk_Textblock_Node_Type;

/** @brief The different types of tag for a node */
typedef enum
{
   ETK_TEXTBLOCK_TAG_DEFAULT,      /**< The default tag: no formatting */
   ETK_TEXTBLOCK_TAG_BOLD,         /**< The text is bold */
   ETK_TEXTBLOCK_TAG_ITALIC,       /**< The text is italic */
   ETK_TEXTBLOCK_TAG_UNDERLINE,    /**< The text is underlined */
   ETK_TEXTBLOCK_TAG_STRIKETHROUGH,/**< The text is strikethrough */
   ETK_TEXTBLOCK_TAG_P,            /**< The tag describes a paragraph */
   ETK_TEXTBLOCK_TAG_STYLE,        /**< The tag describes the style of the text (normal, glow, ...) */
   ETK_TEXTBLOCK_TAG_FONT          /**< The tag describes the font used by the text (face, size, ...) */
} Etk_Textblock_Tag_Type;

/** @brief The different types of style that can be applied on a text */
typedef enum
{
   ETK_TEXTBLOCK_STYLE_NONE,                    /**< No style is applied */
   ETK_TEXTBLOCK_STYLE_OUTLINE,                 /**< The text is outlined */
   ETK_TEXTBLOCK_STYLE_SHADOW,                  /**< The text has a sharp shadow */
   ETK_TEXTBLOCK_STYLE_SOFT_OUTLINE,            /**< The text has a soft outline */
   ETK_TEXTBLOCK_STYLE_GLOW,                    /**< The text has a glow */
   ETK_TEXTBLOCK_STYLE_OUTLINE_SHADOW,          /**< The text is outlined and has a sharp shadow */
   ETK_TEXTBLOCK_STYLE_FAR_SHADOW,              /**< The text has a sharp far shadow */
   ETK_TEXTBLOCK_STYLE_OUTLINE_SOFT_SHADOW,     /**< The text is outlined and has a soft shadow */
   ETK_TEXTBLOCK_STYLE_SOFT_SHADOW,             /**< The text has a soft shadow */
   ETK_TEXTBLOCK_STYLE_FAR_SOFT_SHADOW          /**< The text has a far soft shadow */
} Etk_Textblock_Style_Type;

/** @brief The different type of underlining for a text */
typedef enum
{
   ETK_TEXTBLOCK_UNDERLINE_NONE,    /**< The text is not underlined */
   ETK_TEXTBLOCK_UNDERLINE_SINGLE,  /**< The text is underlined by a single line */
   ETK_TEXTBLOCK_UNDERLINE_DOUBLE   /**< The text is underlined by two lines */
} Etk_Textblock_Underline_Type;

/** TODOC */
typedef enum
{
   ETK_TEXTBLOCK_GRAVITY_LEFT,
   ETK_TEXTBLOCK_GRAVITY_RIGHT
} Etk_Textblock_Gravity;

/** TODOC */
struct Etk_Textblock_Format
{
   Etk_Textblock_Wrap wrap;

   Etk_Textblock_Style_Type style;
   Etk_Color style_color1;
   Etk_Color style_color2;

   Etk_Textblock_Underline_Type underline;
   Etk_Color underline_color1;
   Etk_Color underline_color2;

   Etk_Color strikethrough_color;

   const char *font_face;
   Etk_Color font_color;

   float align;
   int left_margin;
   int right_margin;
   int font_size;

   Etk_Bool strikethrough:1;
   Etk_Bool bold:1;
   Etk_Bool italic:1;
};

/** TODOC */
struct Etk_Textblock_Node
{
   /* private: */
   struct
   {
      union
      {
         /* Params for the <u> tag */
         struct
         {
            Etk_Textblock_Underline_Type type;
            Etk_Color color1;
            Etk_Color color2;
         } u;

         /* Params for the <s> tag */
         struct
         {
            Etk_Color color;
         } s;

         /* Params for the <p> tag */
         struct
         {
            float align;
            int left_margin;
            int right_margin;
            Etk_Textblock_Wrap wrap;
         } p;

         /* Params for the <style> tag */
         struct
         {
            Etk_Textblock_Style_Type type;
            Etk_Color color1;
            Etk_Color color2;
         } style;

         /* Params for the <font> tag */
         struct
         {
            char *face;
            int size;
            Etk_Color color;
         } font;

      } params;

      Etk_Textblock_Tag_Type type;
   } tag;

   Etk_Textblock_Node_Type type;
   Etk_String *text;
   int unicode_length;

   Etk_Textblock_Node *parent;
   Etk_Textblock_Node *prev;
   Etk_Textblock_Node *next;
   Etk_Textblock_Node *children;
   Etk_Textblock_Node *last_child;
};

/**
 * @brief The structure of a textblock iterator
 * @structinfo
 */
struct Etk_Textblock_Iter
{
   /* private: */
   Etk_Textblock *tb;
   Etk_Textblock_Node *node;

   Etk_Textblock_Gravity gravity;
   int pos;
   int index;
};

/**
 * @brief @object The structure of a textblock
 * @structinfo
 */
struct Etk_Textblock
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;

   Etk_Textblock_Node root;
   Eina_List *iters;

   Eina_List *evas_objects;
};

/* Textblock's funcs */
Etk_Type      *etk_textblock_type_get();
Etk_Textblock *etk_textblock_new();

void        etk_textblock_text_set(Etk_Textblock *tb, const char *text, Etk_Bool markup);
Etk_String *etk_textblock_text_get(Etk_Textblock *tb, Etk_Bool markup);
Etk_String *etk_textblock_range_text_get(Etk_Textblock *tb, Etk_Textblock_Iter *iter1, Etk_Textblock_Iter *iter2, Etk_Bool markup);

void etk_textblock_insert(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *text, int length);
void etk_textblock_insert_markup(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *markup_text, int length);

void etk_textblock_clear(Etk_Textblock *tb);
void etk_textblock_delete_before(Etk_Textblock *tb, Etk_Textblock_Iter *iter);
void etk_textblock_delete_after(Etk_Textblock *tb, Etk_Textblock_Iter *iter);
void etk_textblock_delete_range(Etk_Textblock *tb, Etk_Textblock_Iter *iter1, Etk_Textblock_Iter *iter2);

/* Textblock iter's funcs */
Etk_Textblock_Iter   *etk_textblock_iter_new(Etk_Textblock *tb);
void                  etk_textblock_iter_free(Etk_Textblock_Iter *iter);
void                  etk_textblock_iter_gravity_set(Etk_Textblock_Iter *iter, Etk_Textblock_Gravity gravity);
Etk_Textblock_Gravity etk_textblock_iter_gravity_get(Etk_Textblock_Iter *iter);

void     etk_textblock_iter_backward_start(Etk_Textblock_Iter *iter);
void     etk_textblock_iter_forward_end(Etk_Textblock_Iter *iter);
Etk_Bool etk_textblock_iter_backward_char(Etk_Textblock_Iter *iter);
Etk_Bool etk_textblock_iter_forward_char(Etk_Textblock_Iter *iter);

void etk_textblock_iter_copy(Etk_Textblock_Iter *dest, const Etk_Textblock_Iter *src);
int  etk_textblock_iter_compare(Etk_Textblock_Iter *iter1, Etk_Textblock_Iter *iter2);

/* Textblock object's funcs */
Evas_Object       *etk_textblock_object_add(Etk_Textblock *tb, Evas *evas);
void               etk_textblock_object_wrap_set(Evas_Object *tbo, Etk_Textblock_Wrap wrap);
Etk_Textblock_Wrap etk_textblock_object_wrap_get(Evas_Object *tbo);

Etk_Textblock_Iter *etk_textblock_object_cursor_get(Evas_Object *tbo);
Etk_Textblock_Iter *etk_textblock_object_selection_bound_get(Evas_Object *tbo);
void                etk_textblock_object_cursor_visible_set(Evas_Object *tbo, Etk_Bool visible);
Etk_Bool            etk_textblock_object_cursor_visible_get(Evas_Object *tbo);

void etk_textblock_object_yoffset_set( Evas_Object *tbo, int yoffset );
int etk_textblock_object_yoffset_get( Evas_Object *tbo );

void etk_textblock_object_xoffset_set( Evas_Object *tbo, int xoffset );
int etk_textblock_object_xoffset_get( Evas_Object *tbo );

void etk_textblock_object_full_geometry_get( Evas_Object *tbo, int *x, int *y, int *w, int *h );

/* Misc funcs */
int      etk_textblock_unicode_length_get(const char *unicode_string);
Etk_Bool etk_textblock_is_white_char(int c);
void     etk_textblock_char_size_get(Evas *evas, const char *font_face, int font_size, int *w, int *h);
void     etk_textblock_printf(Etk_Textblock *tb);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
