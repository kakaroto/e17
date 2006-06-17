/** @file etk_textblock.h */
#ifndef _ETK_TEXTBLOCK_H_
#define _ETK_TEXTBLOCK_H_

#include "etk_object.h"
#include <Evas.h>
#include "etk_types.h"

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

/** Gets the type of a textblock iterator */
#define ETK_TEXTBLOCK_ITER_TYPE       (etk_textblock_iter_type_get())
/** Casts the object to an Etk_Textblock_Iter */
#define ETK_TEXTBLOCK_ITER(obj)       (ETK_OBJECT_CAST((obj), ETK_TEXTBLOCK_ITER_TYPE, Etk_Textblock_Iter))
/** Check if the object is an Etk_Textblock_Iter */
#define ETK_IS_TEXTBLOCK_ITER(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TEXTBLOCK_ITER_TYPE))

/** TODOC */
typedef enum Etk_Textblock_Wrap
{
   ETK_TEXTBLOCK_WRAP_NONE,     /**< TODOC */        
   ETK_TEXTBLOCK_WRAP_WORD,     /**< TODOC */
   ETK_TEXTBLOCK_WRAP_CHAR      /**< TODOC */
} Etk_Textblock_Wrap;

/** TODOC */
typedef enum Etk_Textblock_Tag_Type
{
   ETK_TEXTBLOCK_TAG_ROOT,
   ETK_TEXTBLOCK_TAG_DEFAULT,
   ETK_TEXTBLOCK_TAG_BOLD,
   ETK_TEXTBLOCK_TAG_ITALIC,
   ETK_TEXTBLOCK_TAG_UNDERLINE,
   ETK_TEXTBLOCK_TAG_P,
   ETK_TEXTBLOCK_TAG_STYLE,
   ETK_TEXTBLOCK_TAG_FONT
   /* ... */
} Etk_Textblock_Tag_Type;

/** TODOC */
typedef enum Etk_Textblock_Style_Type
{
   ETK_TEXTBLOCK_STYLE_NONE,
   ETK_TEXTBLOCK_STYLE_OUTLINE,
   ETK_TEXTBLOCK_STYLE_SHADOW
   /* ... */
} Etk_Textblock_Style_Type;

/** TODOC */
typedef enum Etk_Textblock_Underline_Type
{
   ETK_TEXTBLOCK_UNDERLINE_NONE,
   ETK_TEXTBLOCK_UNDERLINE_SINGLE,
   ETK_TEXTBLOCK_UNDERLINE_DOUBLE
} Etk_Textblock_Underline_Type;

/** TODOC */
typedef enum Etk_Textblock_Gravity
{
   ETK_TEXTBLOCK_GRAVITY_LEFT,
   ETK_TEXTBLOCK_GRAVITY_RIGHT
} Etk_Textblock_Gravity;

/** TODOC */
struct Etk_Textblock_Format
{
   Etk_Textblock_Style_Type style;
   Etk_Color style_color1;
   Etk_Color style_color2;
   
   Etk_Textblock_Underline_Type underline;
   Etk_Color underline_color1;
   Etk_Color underline_color2;
   
   const char *font_face;
   int font_size;
   Etk_Color font_color;
   
   unsigned char bold : 1;
   unsigned char italic : 1;
   /* ... */
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
         
         /* Params for the <p> tag */
         struct
         {
            float align;
            int left_margin;
            int right_margin;
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
         
         /* ... */
      } params;
      
      Etk_Textblock_Tag_Type type;
   } tag;
   
   Etk_String *text;
   int unicode_length;
   
   Etk_Textblock_Node *parent;
   Etk_Textblock_Node *prev;
   Etk_Textblock_Node *next;
   Etk_Textblock_Node *children;
};

/**
 * @brief @object The structure of a textblock iterator
 * @structinfo
 */
struct Etk_Textblock_Iter
{
   /* private: */
   /* Inherit from Etk_Object */
   Etk_Object object;
   
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
   Evas_List *iters;
   Evas_List *line_iters;
   
   Evas_List *evas_objects;
};

/* Textblock's funcs */
Etk_Type *etk_textblock_type_get();
Etk_Type *etk_textblock_iter_type_get();

Etk_Textblock *etk_textblock_new();
Etk_Textblock_Iter *etk_textblock_iter_new(Etk_Textblock *tb);

void etk_textblock_text_set(Etk_Textblock *tb, const char *text, Etk_Bool markup);
Etk_String *etk_textblock_text_get(Etk_Textblock *tb, Etk_Bool markup);
void etk_textblock_clear(Etk_Textblock *tb);

void etk_textblock_text_insert(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *text, int length);
void etk_textblock_text_insert_markup(Etk_Textblock *tb, Etk_Textblock_Iter *iter, const char *markup_text, int length);

void etk_textblock_printf(Etk_Textblock *tb);

/* Textblock iter's funcs */
void etk_textblock_iter_backward_start(Etk_Textblock_Iter *iter);
void etk_textblock_iter_forward_end(Etk_Textblock_Iter *iter);

void etk_textblock_iter_copy(Etk_Textblock_Iter *dest, const Etk_Textblock_Iter *src);
int etk_textblock_iter_compare(Etk_Textblock_Iter *iter1, Etk_Textblock_Iter *iter2);

/* Textblock object's funcs */
Evas_Object *etk_textblock_object_add(Etk_Textblock *tb, Evas *evas);

void etk_textblock_object_wrap_set(Evas_Object *tbo, Etk_Textblock_Wrap wrap);
Etk_Textblock_Wrap etk_textblock_object_wrap_get(Evas_Object *tbo);

/* Misc funcs */
int etk_textblock_unicode_length_get(const char *unicode_string);
Etk_Bool etk_textblock_is_white_char(int c);

/** @} */
 
#endif
