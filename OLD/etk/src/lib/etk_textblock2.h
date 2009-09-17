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

/** @file etk_textblock2.h */
#ifndef _ETK_TEXTBLOCK2_H_
#define _ETK_TEXTBLOCK2_H_

#include <Evas.h>

#include "etk_object.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Textblock2 Etk_Textblock2
 * @brief TODO: Etk_Textblock2: brief description
 * @{
 */

/** Gets the type of a textblock */
#define ETK_TEXTBLOCK2_TYPE       (etk_textblock2_type_get())
/** Casts the object to an Etk_Textblock2 */
#define ETK_TEXTBLOCK2(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXTBLOCK2_TYPE, Etk_Textblock2))
/** Check if the object is an Etk_Textblock */
#define ETK_IS_TEXTBLOCK2(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXTBLOCK2_TYPE))


/** @brief The different types of wrapping to apply on a textblock's paragraph */
typedef enum
{
   ETK_TEXTBLOCK2_WRAP_NONE,                  /**< The text is not wrapped */
   ETK_TEXTBLOCK2_WRAP_WORD,                  /**< The text is wrapped between the words (or between the chars if it's not sufficient) */
   ETK_TEXTBLOCK2_WRAP_CHAR                   /**< The text is wrapped between the chars */
} Etk_Textblock2_Wrap;

/** @brief The different types of tag for a node */
typedef enum
{
   ETK_TEXTBLOCK2_TAG_DEFAULT = 0,            /**< The default tag: no formatting */
   ETK_TEXTBLOCK2_TAG_BOLD = 1 << 0,          /**< The text is bold */
   ETK_TEXTBLOCK2_TAG_ITALIC = 1 << 1,        /**< The text is italic */
   ETK_TEXTBLOCK2_TAG_UNDERLINE = 1 << 2,     /**< The text is underlined */
   ETK_TEXTBLOCK2_TAG_STRIKETHROUGH = 1 << 3, /**< The text is strikethrough */
   ETK_TEXTBLOCK2_TAG_STYLE = 1 << 4,         /**< The tag describes the style of the text (normal, glow, ...) */
   ETK_TEXTBLOCK2_TAG_FONT = 1 << 5           /**< The tag describes the font used by the text (face, size, ...) */
} Etk_Textblock2_Tag_Type;

/** @brief The different types of style that can be applied on a text */
typedef enum
{
   ETK_TEXTBLOCK2_STYLE_NONE,                 /**< No style is applied */
   ETK_TEXTBLOCK2_STYLE_OUTLINE,              /**< The text is outlined */
   ETK_TEXTBLOCK2_STYLE_SHADOW,               /**< The text has a sharp shadow */
   ETK_TEXTBLOCK2_STYLE_SOFT_OUTLINE,         /**< The text has a soft outline */
   ETK_TEXTBLOCK2_STYLE_GLOW,                 /**< The text has a glow */
   ETK_TEXTBLOCK2_STYLE_OUTLINE_SHADOW,       /**< The text is outlined and has a sharp shadow */
   ETK_TEXTBLOCK2_STYLE_FAR_SHADOW,           /**< The text has a sharp far shadow */
   ETK_TEXTBLOCK2_STYLE_OUTLINE_SOFT_SHADOW,  /**< The text is outlined and has a soft shadow */
   ETK_TEXTBLOCK2_STYLE_SOFT_SHADOW,          /**< The text has a soft shadow */
   ETK_TEXTBLOCK2_STYLE_FAR_SOFT_SHADOW       /**< The text has a far soft shadow */
} Etk_Textblock2_Style_Type;

/** @brief The different type of underlining for a text */
typedef enum
{
   ETK_TEXTBLOCK2_UNDERLINE_NONE,             /**< The text is not underlined */
   ETK_TEXTBLOCK2_UNDERLINE_SINGLE,           /**< The text is underlined by a single line */
   ETK_TEXTBLOCK2_UNDERLINE_DOUBLE            /**< The text is underlined by two lines */
} Etk_Textblock2_Underline_Type;


/** TODOC */
struct Etk_Textblock2_Format
{
   /* Params for the <u> tag */
   struct
   {
      Etk_Textblock2_Underline_Type type;
      Etk_Color color1;
      Etk_Color color2;
   } u;

   /* Params for the <s> tag */
   struct
   {
      Etk_Color color;
   } s;

   /* Params for the <style> tag */
   struct
   {
      Etk_Textblock2_Style_Type type;
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
   
   Etk_Textblock2_Tag_Type type;
};

/** TODOC */
struct Etk_Textblock2_Node
{
   /* private: */
   Etk_Textblock2_Line *line;
   Etk_Textblock2_Node *prev;
   Etk_Textblock2_Node *next;

   Etk_Textblock2_Format *format;

   Etk_String *text;
   int unicode_length;
};

/** TODOC */
struct Etk_Textblock2_Line
{
   /* private: */
   Etk_Textblock2_Node *nodes;
   Etk_Textblock2_Node *last_node;

   Etk_Textblock2_Paragraph *paragraph;
   Etk_Textblock2_Line *prev;
   Etk_Textblock2_Line *next;
   
   void *object_lines;
};

/** TODOC */
struct Etk_Textblock2_Paragraph
{
   /* private: */
   Etk_Textblock2_Line *lines;
   Etk_Textblock2_Line *last_line;

   Etk_Textblock2 *tb;
   Etk_Textblock2_Paragraph *prev;
   Etk_Textblock2_Paragraph *next;

   float align;
   int left_margin;
   int right_margin;
   Etk_Textblock2_Wrap wrap;
};

/**
 * @brief TODOC
 * @structinfo
 */
struct Etk_Textblock2_Iter
{
   /* private: */
   Etk_Textblock2 *tb;
   Etk_Textblock2_Iter *prev;
   Etk_Textblock2_Iter *next;
   
   Etk_Textblock2_Node *node;
   int pos;
   int index;
};

/**
 * @brief @object TODOC
 * @structinfo
 */
struct Etk_Textblock2
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;

   Etk_Textblock2_Paragraph *paragraphs;
   Etk_Textblock2_Paragraph *last_paragraph;
   Etk_Textblock2_Iter *iters;
   Etk_Textblock2_Iter *last_iter;
   Eina_List *tbos;
};


/* Textblock's funcs */
Etk_Type                 *etk_textblock2_type_get(void);
Etk_Textblock2           *etk_textblock2_new(void);
void                      etk_textblock2_text_set(Etk_Textblock2 *tb, const char *text, Etk_Bool markup);
Etk_String               *etk_textblock2_text_get(Etk_Textblock2 *tb, Etk_Bool markup);
Etk_String               *etk_textblock2_range_text_get(Etk_Textblock2_Iter *iter1, Etk_Textblock2_Iter *iter2, Etk_Bool markup);
void                      etk_textblock2_insert(Etk_Textblock2_Iter *iter, const char *text, int length);
void                      etk_textblock2_insert_markup(Etk_Textblock2_Iter *iter, const char *markup_text, int length);
void                      etk_textblock2_clear(Etk_Textblock2 *tb);
Etk_Bool                  etk_textblock2_delete_before(Etk_Textblock2_Iter *iter);
Etk_Bool                  etk_textblock2_delete_after(Etk_Textblock2_Iter *iter);
void                      etk_textblock2_delete_range(Etk_Textblock2_Iter *iter1, Etk_Textblock2_Iter *iter2);
void                      etk_textblock2_printf(Etk_Textblock2 *tb);

/* Textblock-paragraph's funcs */
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_add(Etk_Textblock2_Iter *iter);
Etk_Bool                  etk_textblock2_paragraph_delete(Etk_Textblock2_Paragraph *paragraph);
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_first_get(Etk_Textblock2 *tb);
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_last_get(Etk_Textblock2 *tb);
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_prev_get(Etk_Textblock2_Paragraph *paragraph);
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_next_get(Etk_Textblock2_Paragraph *paragraph);

/* Textblock-line's funcs */
Etk_Textblock2_Line      *etk_textblock2_line_add(Etk_Textblock2_Iter *iter);
Etk_Bool                  etk_textblock2_line_delete(Etk_Textblock2_Line *line);
Etk_Textblock2_Line      *etk_textblock2_line_first_get(Etk_Textblock2_Paragraph *paragraph);
Etk_Textblock2_Line      *etk_textblock2_line_last_get(Etk_Textblock2_Paragraph *paragraph);
Etk_Textblock2_Line      *etk_textblock2_line_prev_get(Etk_Textblock2_Line *line);
Etk_Textblock2_Line      *etk_textblock2_line_next_get(Etk_Textblock2_Line *line);
Etk_Textblock2_Line      *etk_textblock2_line_walk_prev(Etk_Textblock2_Line *line);
Etk_Textblock2_Line      *etk_textblock2_line_walk_next(Etk_Textblock2_Line *line);

/* Textblock-node's funcs */
Etk_Textblock2_Node      *etk_textblock2_node_add(Etk_Textblock2_Iter *iter);
Etk_Bool                  etk_textblock2_node_delete(Etk_Textblock2_Node *node);
Etk_Textblock2_Node      *etk_textblock2_node_first_get(Etk_Textblock2_Line *line);
Etk_Textblock2_Node      *etk_textblock2_node_last_get(Etk_Textblock2_Line *line);
Etk_Textblock2_Node      *etk_textblock2_node_prev_get(Etk_Textblock2_Node *node);
Etk_Textblock2_Node      *etk_textblock2_node_next_get(Etk_Textblock2_Node *node);
Etk_Textblock2_Node      *etk_textblock2_node_walk_prev(Etk_Textblock2_Node *node);
Etk_Textblock2_Node      *etk_textblock2_node_walk_next(Etk_Textblock2_Node *node);
void                      etk_textblock2_node_format_set(Etk_Textblock2_Node *node, const Etk_Textblock2_Format *format);
const Etk_Textblock2_Format *etk_textblock2_node_format_get(Etk_Textblock2_Node *node);
void                      etk_textblock2_node_format_apply(Etk_Textblock2_Node *node, const char *format);

/* Textblock-format's funcs */
Etk_Textblock2_Format    *etk_textblock2_format_new(void);
void                      etk_textblock2_format_free(Etk_Textblock2_Format *format);
void                      etk_textblock2_format_reset(Etk_Textblock2_Format *format);
void                      etk_textblock2_format_copy(Etk_Textblock2_Format *format1, const Etk_Textblock2_Format *format2);

/* Textblock-iter's funcs */
Etk_Textblock2_Iter      *etk_textblock2_iter_new(Etk_Textblock2 *tb);
void                      etk_textblock2_iter_free(Etk_Textblock2_Iter *iter);
void                      etk_textblock2_iter_copy(Etk_Textblock2_Iter *dest, Etk_Textblock2_Iter *src);
int                       etk_textblock2_iter_compare(Etk_Textblock2_Iter *iter1, Etk_Textblock2_Iter *iter2);

void                      etk_textblock2_iter_backward_start(Etk_Textblock2_Iter *iter);
void                      etk_textblock2_iter_forward_end(Etk_Textblock2_Iter *iter);
Etk_Bool                  etk_textblock2_iter_backward_char(Etk_Textblock2_Iter *iter);
Etk_Bool                  etk_textblock2_iter_forward_char(Etk_Textblock2_Iter *iter);
void                      etk_textblock2_iter_goto_paragraph(Etk_Textblock2_Iter *iter, Etk_Textblock2_Paragraph *paragraph);
void                      etk_textblock2_iter_goto_line(Etk_Textblock2_Iter *iter, Etk_Textblock2_Line *line);
void                      etk_textblock2_iter_goto_node(Etk_Textblock2_Iter *iter, Etk_Textblock2_Node *node);

/* Textblock-object's funcs */
Evas_Object              *etk_textblock2_object_add(Etk_Textblock2 *tb, Evas *evas);
Etk_Textblock2_Iter      *etk_textblock2_object_cursor_get(Evas_Object *tbo);
Etk_Textblock2_Iter      *etk_textblock2_object_selection_bound_get(Evas_Object *tbo);
void                      etk_textblock2_object_offset_set(Evas_Object *tbo, int xoffset, int yoffset);
void                      etk_textblock2_object_offset_get(Evas_Object *tbo, int *xoffset, int *yoffset);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
