/** @file etk_textblock2.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_textblock2.h"

#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Job.h>

#include "etk_string.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Textblock2
 * @{
 */

typedef struct Etk_TB2_Object_SD Etk_TB2_Object_SD;
typedef struct Etk_TB2_Object_Line Etk_TB2_Object_Line;

/* The data associated to a textblock-object */
struct Etk_TB2_Object_SD
{
   Etk_Textblock2 *tb;
   Etk_Textblock2_Iter *cursor;
   Etk_Textblock2_Iter *selection;
   Etk_TB2_Object_Line *lines;
   
   Evas_Textblock_Style *style;
   Evas_Object *clip;
   Ecore_Job *update_job;
   
   int xoffset;
   int yoffset;
};

/* A line of a textblock-object */
struct Etk_TB2_Object_Line
{
   Etk_TB2_Object_Line *prev;
   Etk_TB2_Object_Line *next;
   Etk_TB2_Object_Line *fellow_prev;
   Etk_TB2_Object_Line *fellow_next;
   Etk_Textblock2_Line *line;
   
   Evas_Object *tbo;
   Evas_Object *object;
   
   Etk_Size size;
   Etk_Bool need_content_update : 1;
   Etk_Bool need_geometry_update : 1;
   Etk_Bool wrapped : 1;
};

static void                      _etk_tb2_constructor(Etk_Textblock2 *tb);
static void                      _etk_tb2_destructor(Etk_Textblock2 *tb);

static Etk_Textblock2_Paragraph *_etk_tb2_paragraph_create(Etk_Textblock2 *tb, Etk_Textblock2_Paragraph *prev, Etk_Bool create_line);
static Etk_Textblock2_Line      *_etk_tb2_line_create(Etk_Textblock2_Paragraph *paragraph, Etk_Textblock2_Line *prev, Etk_Bool create_node);
static Etk_Textblock2_Node      *_etk_tb2_node_create(Etk_Textblock2_Line *line, Etk_Textblock2_Node *prev);
static void                      _etk_tb2_paragraph_free(Etk_Textblock2_Paragraph *paragraph);
static void                      _etk_tb2_line_free(Etk_Textblock2_Line *line);
static void                      _etk_tb2_node_free(Etk_Textblock2_Node *node);
static Etk_Textblock2_Paragraph *_etk_tb2_paragraph_split(Etk_Textblock2_Paragraph *paragraph, Etk_Textblock2_Iter *iter);
static Etk_Textblock2_Line      *_etk_tb2_line_split(Etk_Textblock2_Line *line, Etk_Textblock2_Iter *iter);
static Etk_Textblock2_Node      *_etk_tb2_node_split(Etk_Textblock2_Node *node, Etk_Textblock2_Iter *iter);

static Etk_Bool                  _etk_tb2_iter_is_valid(Etk_Textblock2_Iter *iter);
static void                      _etk_tb2_iter_reorder(Etk_Textblock2_Iter *iter, Etk_Textblock2_Iter *prev);

static Etk_TB2_Object_Line      *_etk_tb2_object_line_get(Evas_Object *tbo, Etk_Textblock2_Line *line);
static void                      _etk_tb2_object_line_add(Evas_Object *tbo, Etk_Textblock2_Line *line);
static void                      _etk_tb2_object_line_remove(Evas_Object *tbo, Etk_Textblock2_Line *line);
static void                      _etk_tb2_object_line_update_queue(Evas_Object *tbo, Etk_Textblock2_Line *line, Etk_Bool update_content, Etk_Bool update_geometry);
static void                      _etk_tb2_object_update_queue(Evas_Object *tbo);
static void                      _etk_tb2_object_update(void *obj);
static Evas_Object              *_etk_tb2_object_line_object_add(Evas_Object *tbo);
static void                      _etk_tb2_object_line_object_build(Evas_Object *lo, Etk_Textblock2_Line *line);

static void                      _etk_tb2_object_smart_add(Evas_Object *tbo);
static void                      _etk_tb2_object_smart_del(Evas_Object *tbo);
static void                      _etk_tb2_object_smart_move(Evas_Object *tbo, Evas_Coord x, Evas_Coord y);
static void                      _etk_tb2_object_smart_resize(Evas_Object *tbo, Evas_Coord w, Evas_Coord h);
static void                      _etk_tb2_object_smart_show(Evas_Object *tbo);
static void                      _etk_tb2_object_smart_hide(Evas_Object *tbo);
static void                      _etk_tb2_object_smart_color_set(Evas_Object *tbo, int r, int g, int b, int a);
static void                      _etk_tb2_object_smart_clip_set(Evas_Object *tbo, Evas_Object *clip);
static void                      _etk_tb2_object_smart_clip_unset(Evas_Object *tbo);

static const char *_escape_sequences[] =
{
   "&lt;", "<",
   "&gt;", ">",
   "&amp;", "&",
   NULL
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
 * @brief Gets the type of an Etk_Textblock2
 * @return Returns the type of an Etk_Textblock2
 */
Etk_Type *etk_textblock2_type_get(void)
{
   static Etk_Type *textblock2_type = NULL;

   if (!textblock2_type)
   {
      textblock2_type = etk_type_new("Etk_Textblock2", ETK_OBJECT_TYPE, sizeof(Etk_Textblock2),
            ETK_CONSTRUCTOR(_etk_tb2_constructor), ETK_DESTRUCTOR(_etk_tb2_destructor), NULL);
   }

   return textblock2_type;
}

/**
 * @brief Creates a new textblock
 * @return Returns the new textblock
 */
Etk_Textblock2 *etk_textblock2_new(void)
{
   return ETK_TEXTBLOCK2(etk_object_new(ETK_TEXTBLOCK2_TYPE, NULL));
}

/**
 * @brief Sets the text of the textblock. The current text will be replaced by @a text
 * @param tb a textblock
 * @param text the text to set
 * @param markup whether or not you want to apply markup and styling on the text.
 * If @a markup is ETK_TRUE, you can use tags to format the text.
 * @note After this operation, all the iterators will be at the start of the textblock
 */
void etk_textblock2_text_set(Etk_Textblock2 *tb, const char *text, Etk_Bool markup)
{
   Etk_Textblock2_Iter *iter;
   
   if (!tb)
      return;
   
   etk_textblock2_clear(tb);
   iter = etk_textblock2_iter_new(tb);
   if (markup)
      etk_textblock2_insert_markup(iter, text, -1);
   else
      etk_textblock2_insert(iter, text, -1);
   etk_textblock2_iter_free(iter);
   
   /* Move the iterators at the start of the textblock */
   for (iter = tb->iters; iter; iter = iter->next)
   {
      iter->node = tb->paragraphs->lines->nodes;
      iter->pos = 0;
      iter->index = 0;
   }
}

/**
 * @brief Gets the text of the textblock
 * @param tb a textblock
 * @param markup whether or not you want to get formatting-tags in the returned text
 * @return Returns a string containing the text of the textblock
 * @note Once you no longer need the returned string, you should destroy it with etk_object_destroy()
 */
Etk_String *etk_textblock2_text_get(Etk_Textblock2 *tb, Etk_Bool markup)
{
   Etk_Textblock2_Iter *iter1;
   Etk_Textblock2_Iter *iter2;
   Etk_String *text;
   
   if (!tb)
      return NULL;
   
   iter1 = etk_textblock2_iter_new(tb);
   iter2 = etk_textblock2_iter_new(tb);
   etk_textblock2_iter_forward_end(iter2);
   text = etk_textblock2_range_text_get(iter1, iter2, markup);
   etk_textblock2_iter_free(iter1);
   etk_textblock2_iter_free(iter2);
   
   return text;
}

/**
 * @brief Gets the text of the textblock, between @a iter1 and @a iter2
 * @param tb a textblock
 * @param iter1 the first iterator
 * @param iter2 the second_iterator
 * @param markup whether or not you want to get formatting-tags in the returned text
 * @return Returns a string containing the text of the textblock, between @a iter1 and @a iter2
 * @note Once you no longer need the returned string, you should destroy it with etk_object_destroy()
 * @note @a iter1 has not necessarily to be before @a iter2
 */
Etk_String *etk_textblock2_range_text_get(Etk_Textblock2_Iter *iter1, Etk_Textblock2_Iter *iter2, Etk_Bool markup)
{
   /* TODO: implement etk_textblock2_range_text_get() */
   return NULL;
}

/**
 * @brief Inserts @a length bytes of @a text at @a iter in the textblock.
 * If you want to use tags to format the inserted text, you should use etk_textblock2_insert_markup() instead
 * @param iter the iterator where to insert the text. The iterator will be placed after the inserted text
 * @param text the unicode-encoded text to insert. You can use '\n' to break the text into several lines
 * @param length the maximum number of bytes to insert. If @a length is negative, the text will be entirely inserted
 * @see etk_textblock_insert_markup()
 */
void etk_textblock2_insert(Etk_Textblock2_Iter *iter, const char *text, int length)
{
   Etk_Textblock2_Iter *it;
   Etk_Textblock2_Node *node;
   Etk_Bool new_line;
   Etk_Bool done;
   Evas_List *l;
   int len, ulen;
   int decoded;
   int i, first;
   int pos;

   if (!iter || !text || !_etk_tb2_iter_is_valid(iter))
      return;
   
   first = 0;
   done = ETK_FALSE;
   while (!done)
   {
      len = 0;
      ulen = 0;
      decoded = 1;
      new_line = ETK_FALSE;
      done = ETK_TRUE;
      
      /* Calculate the length/unicode-length of the text to insert */
      for (i = first; decoded != 0 && text[i] != '\0' && (length < 0 || i < length);
            i = evas_string_char_next_get(text, i, &decoded))
      {
         if (text[i] == '\n' || text[i] == '\r')
         {
            done = ETK_FALSE;
            new_line = ETK_TRUE;
            i++;
            
            if (text[i - 1] == '\r' && text[i] == '\n')
               i++;
            break;
         }
         
         len = i - first + 1;
         ulen++;
      }
      
      /* Insert the text into the corresponding node */
      iter->node->text = etk_string_insert_sized(iter->node->text, iter->index, &text[first], len);
      iter->node->unicode_length += ulen;

      /* Update the iterators */
      node = iter->node;
      pos = iter->pos;
      for (it = iter->tb->iters; it; it = it->next)
      {
         if (it->node == node && it->pos >= pos)
         {
            it->pos += ulen;
            it->index += len;
         }
      }
      
      /* Update the textblock-objects' affected line */
      for (l = iter->tb->tbos; l; l = l->next)
         _etk_tb2_object_line_update_queue(l->data, iter->node->line, ETK_TRUE, ETK_TRUE);
      
      /* Insert a line if necessary */
      if (new_line)
         etk_textblock2_line_add(iter);
      
      first = i;
   }
}

/**
 * @brief Inserts @a length bytes of @a text at @a iter in the textblock. Tags can be used to format the text
 * @param iter the iterator where to insert the text. The iterator will be placed after the inserted text
 * @param markup_text the unicode-encoded text to insert. You can use tags to format the text, as well as escape sequences
 * @param length the maximum number of bytes to insert. If @a length is negative, the text will be entirely inserted
 * @see etk_textblock_insert()
 */
void etk_textblock2_insert_markup(Etk_Textblock2_Iter *iter, const char *markup_text, int length)
{
   /* TODO: implement etk_textblock2_insert_markup() */
}

/**
 * @brief Clears the text of the given textblock. After this operation, the textblock
 * will only have one paragraph, with one line made up of one empty node. All the iterators
 * will still be valid and will be placed at the start of the empty textblock
 * @param tb the textblock to clear
 */
void etk_textblock2_clear(Etk_Textblock2 *tb)
{
   if (!tb)
      return;
   while (etk_textblock2_paragraph_delete(tb->paragraphs));
}

/**
 * @brief Deletes the character before the iterator
 * @param iter the iterator where to delete the character
 * @return Returns ETK_TRUE if there was a character before the iterator, ETK_FALSE otherwise
 */
Etk_Bool etk_textblock2_delete_before(Etk_Textblock2_Iter *iter)
{
   Etk_Textblock2_Iter *iter2;
   Etk_Bool res;

   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return ETK_FALSE;

   iter2 = etk_textblock2_iter_new(iter->tb);
   etk_textblock2_iter_copy(iter2, iter);
   if ((res = etk_textblock2_iter_backward_char(iter)))
      etk_textblock2_delete_range(iter, iter2);
   etk_textblock2_iter_free(iter2);
   return res;
}

/**
 * @brief Deletes the character after the iterator
 * @param iter the iterator where to delete the character
 * @return Returns ETK_TRUE if there was a character after the iterator, ETK_FALSE otherwise
 */
Etk_Bool etk_textblock2_delete_after(Etk_Textblock2_Iter *iter)
{
   Etk_Textblock2_Iter *iter2;
   Etk_Bool res;

   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return ETK_FALSE;

   iter2 = etk_textblock2_iter_new(iter->tb);
   etk_textblock2_iter_copy(iter2, iter);
   if ((res = etk_textblock2_iter_forward_char(iter2)))
      etk_textblock2_delete_range(iter, iter2);
   etk_textblock2_iter_free(iter2);
   return res;
}

/**
 * @brief Deletes the text between @a iter1 and @a iter2
 * @param iter1 the first iterator
 * @param iter2 the second iterator
 * @note @a iter1 has not necessarily to be before @a iter2
 */
void etk_textblock2_delete_range(Etk_Textblock2_Iter *iter1, Etk_Textblock2_Iter *iter2)
{
   int cmp;
   Etk_Textblock2_Iter *it;
   
   if (!iter1 || !iter2 || !_etk_tb2_iter_is_valid(iter1) || !_etk_tb2_iter_is_valid(iter2))
      return;
   
   cmp = etk_textblock2_iter_compare(iter1, iter2);
   if (cmp == 0)
      return;
   else if (cmp > 0)
   {
      it = iter1;
      iter1 = iter2;
      iter2 = it;
   }
   
   /* TODO: implement etk_textblock2_delete_range() */
}

/**
 * @brief Prints the textblock's structure to stdout.
 * This function is for debug-purpose only and should be deleted soon
 * @param tb the textblock to print
 */
/* TODO: remove this function */
void etk_textblock2_printf(Etk_Textblock2 *tb)
{
   Etk_Textblock2_Paragraph *p;
   Etk_Textblock2_Line *l;
   Etk_Textblock2_Node *n;
   const char *text;
   int pc, lc, nc;
   
   if (!tb)
      return;
   
   for (p = tb->paragraphs, pc = 0; p; p = p->next, pc++)
   {
      printf("P%d:\n", pc);
      for (l = p->lines, lc = 0; l; l = l->next, lc++)
      {
         printf("\tL%d:\n", lc);
         for (n = l->nodes, nc = 0; n; n = n->next, nc++)
         {
            printf("\t\tN%d:", nc);
            if ((text = etk_string_get(n->text)))
               printf(" %s", text);
            printf("\n");
         }
      }
   }
}

/**************************
 * Textblock-Paragraph's funcs
 **************************/

/**
 * @brief Adds a new paragraph to the textblock at @iter. The new paragraph will contain the content
 * of the current paragraph located after the iterator. The iterator will be placed at the start of
 * the new paragraph. The new paragraph will have the same properties as the current one
 * @param iter the iter where to insert the new paragraph
 * @return Returns the new paragraph
 */
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_add(Etk_Textblock2_Iter *iter)
{
   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return NULL;
   return _etk_tb2_paragraph_split(iter->node->line->paragraph, iter);
}

/**
 * @brief Deletes an existing paragraph of a textblock. If the paragraph was the last remaining
 * paragraph of the textblock, a new empty paragraph will be created (a textblock always has to
 * have at least one paragraph in order to remain valid). The iterators will be moved to the start
 * of the next paragraph if there is one, or to the end of the textblock otherwise
 * @param paragraph the paragraph to delete
 * @return Returns ETK_FALSE if the paragraph was the last paragraph of the textblock, ETK_TRUE otherwise
 */
Etk_Bool etk_textblock2_paragraph_delete(Etk_Textblock2_Paragraph *paragraph)
{
   Etk_Textblock2 *tb;
   Etk_Textblock2_Iter *it;
   Etk_Bool res;
   
   if (!paragraph || !(tb = paragraph->tb))
      return ETK_FALSE;
   
   /* Make sure at least one paragraph is remaining */
   if (!paragraph->prev && !paragraph->next)
   {
      _etk_tb2_paragraph_create(tb, NULL, ETK_TRUE);
      res = ETK_FALSE;
   }
   else
      res = ETK_TRUE;
   
   /* Update the iterators */
   for (it = tb->iters; it; it = it->next)
   {
      if (it->node->line->paragraph == paragraph)
      {
         if (paragraph->next)
         {
            it->node = paragraph->next->lines->nodes;
            it->pos = 0;
            it->index = 0;
         }
         else
         {
            it->node = tb->last_paragraph->last_line->last_node;
            it->pos = it->node->unicode_length;
            it->index = etk_string_length_get(it->node->text);
         }
      }
   }
   
   /* Destroys the paragraph */
   _etk_tb2_paragraph_free(paragraph);
   
   return res;
}

/**
 * @brief Gets the first paragraph of the textblock
 * @param tb a textblock
 * @return Returns the first paragraph of the textblock
 */
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_first_get(Etk_Textblock2 *tb)
{
   if (!tb)
      return NULL;
   return tb->paragraphs;
}

/**
 * @brief Gets the last paragraph of the textblock
 * @param tb a textblock
 * @return Returns the last paragraph of the textblock
 */
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_last_get(Etk_Textblock2 *tb)
{
   if (!tb)
      return NULL;
   return tb->last_paragraph;
}

/**
 * @brief Gets the paragraph before @a paragraph
 * @param paragraph a paragraph
 * @return Returns the paragraph before @a paragraph or NULL if @a paragraph is the first one
 */
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_prev_get(Etk_Textblock2_Paragraph *paragraph)
{
   if (!paragraph)
      return NULL;
   return paragraph->prev;
}

/**
 * @brief Gets the paragraph after @a paragraph
 * @param paragraph a paragraph
 * @return Returns the paragraph after @a paragraph or NULL if @a paragraph is the last one
 */
Etk_Textblock2_Paragraph *etk_textblock2_paragraph_next_get(Etk_Textblock2_Paragraph *paragraph)
{
   if (!paragraph)
      return NULL;
   return paragraph->next;
}

/**************************
 * Textblock-Line's funcs
 **************************/

/**
 * @brief Adds a new line to the textblock at @iter. The new line will contain the content of the current
 * line located after the iterator. The iterator will be placed at the start of the new line
 * @param iter the iter where to insert the new line
 * @return Returns the new line
 */
Etk_Textblock2_Line *etk_textblock2_line_add(Etk_Textblock2_Iter *iter)
{
   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return NULL;
   return _etk_tb2_line_split(iter->node->line, iter);
}

/**
 * @brief Deletes an existing line of a paragraph. If the line was the last remaining line
 * of the paragraph, a new empty line will be created (a paragraph always has to have at least
 * one line in order to remain valid). The iterators will be moved to the start of the next line
 * if there is one, or to the end of the textblock otherwise
 * @param line the line to delete
 * @return Returns ETK_FALSE if the line was the last line of the paragraph, ETK_TRUE otherwise
 */
Etk_Bool etk_textblock2_line_delete(Etk_Textblock2_Line *line)
{
   Etk_Textblock2 *tb;
   Etk_Textblock2_Paragraph *paragraph;
   Etk_Textblock2_Line *next;
   Etk_Textblock2_Iter *it;
   Etk_Bool res;
   
   if (!line || !(paragraph = line->paragraph) || !(tb = paragraph->tb))
      return ETK_FALSE;
   
   /* Make sure at least one line is remaining */
   if (!line->prev && !line->next)
   {
      _etk_tb2_line_create(paragraph, NULL, ETK_TRUE);
      res = ETK_FALSE;
   }
   else
      res = ETK_TRUE;
   
   /* Update the iterators */
   /* TODO: is there a good reason to have this here and not in line_free() ? */
   next = etk_textblock2_line_walk_next(line);
   for (it = tb->iters; it; it = it->next)
   {
      if (it->node->line == line)
      {
         if (next)
         {
            it->node = next->nodes;
            it->pos = 0;
            it->index = 0;
         }
         else
         {
            it->node = tb->last_paragraph->last_line->last_node;
            it->pos = it->node->unicode_length;
            it->index = etk_string_length_get(it->node->text);
         }
      }
   }
   
   /* Destroys the line */
   _etk_tb2_line_free(line);
   
   return res;
}

/**
 * @brief Gets the first line of the paragraph
 * @param paragraph a paragraph
 * @return Returns the first line of the paragraph
 */
Etk_Textblock2_Line *etk_textblock2_line_first_get(Etk_Textblock2_Paragraph *paragraph)
{
   if (!paragraph)
      return NULL;
   return paragraph->lines;
}

/**
 * @brief Gets the last line of the paragraph
 * @param tb a paragraph
 * @return Returns the last line of the paragraph
 */
Etk_Textblock2_Line *etk_textblock2_line_last_get(Etk_Textblock2_Paragraph *paragraph)
{
   if (!paragraph)
      return NULL;
   return paragraph->last_line;
}

/**
 * @brief Gets the line before @a node in the current paragraph
 * @param node a node
 * @return Returns the line before @a line in the current paragraph,
 * or NULL if @a line is the first one of the paragraph
 */
Etk_Textblock2_Line *etk_textblock2_line_prev_get(Etk_Textblock2_Line *line)
{
   if (!line)
      return NULL;
   return line->prev;
}

/**
 * @brief Gets the line after @a node in the current paragraph
 * @param node a node
 * @return Returns the line after @a line in the current paragraph,
 * or NULL if @a line is the last one of the paragraph
 */
Etk_Textblock2_Line *etk_textblock2_line_next_get(Etk_Textblock2_Line *line)
{
   if (!line)
      return NULL;
   return line->next;
}

/**
 * @brief Walks to the previous line of the textblock. Unlike etk_textblock2_line_prev_get(),
 * this function will not return NULL if the line is the first line of the paragraph. In this case, it
 * will return the last line of the previous paragraph of the textblock
 * @param node a node
 * @return Returns the line before @a line in the textblock, or NULL if @a line is the first line of the textblock
 */
Etk_Textblock2_Line *etk_textblock2_line_walk_prev(Etk_Textblock2_Line *line)
{
   if (!line)
      return NULL;
   
   if (line->prev)
      return line->prev;
   else if (line->paragraph->prev)
      return line->paragraph->prev->last_line;
   else
      return NULL;
}

/**
 * @brief Walks to the next line of the textblock. Unlike etk_textblock2_line_next_get(),
 * this function will not return NULL if the line is the last line of the paragraph. In this case, it
 * will return the first line of the next paragraph of the textblock
 * @param node a node
 * @return Returns the line after @a line in the textblock, or NULL if @a line is the last line of the textblock
 */
Etk_Textblock2_Line *etk_textblock2_line_walk_next(Etk_Textblock2_Line *line)
{
   if (!line)
      return NULL;
   
   if (line->next)
      return line->next;
   else if (line->paragraph->next)
      return line->paragraph->next->lines;
   else
      return NULL;
}

/**************************
 * Textblock-Node's funcs
 **************************/

/**
 * @brief Adds an empty node to the textblock at @iter.
 * @param iter the iter where to insert the new node
 * @return Returns the new node
 */
Etk_Textblock2_Node *etk_textblock2_node_add(Etk_Textblock2_Iter *iter)
{
   Etk_Textblock2_Node *node;
   Etk_Textblock2_Line *line;
   
   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return NULL;
   
   node = iter->node;
   line = iter->node->line;
   
   /* No need to split the current node if the iter is at the start of the node */
   if (iter->pos == 0)
      return _etk_tb2_node_create(line, node->prev);
   /* No need to split the current node if the iter is at the end of the node */
   else if (iter->pos == node->unicode_length)
      return _etk_tb2_node_create(line, node);
   /* Otherwise, we split the current node and insert a new node between the two chunks */
   else
   {
      _etk_tb2_node_split(node, iter);
      return _etk_tb2_node_create(line, node);
   }
}

/**
 * @brief Deletes an existing node of a line. If the node was the last remaining node of the
 * line, a new empty node will be created (a line always has to have at least one node in
 * order to remain valid). The iterators will be moved to the start of the next node
 * if there is one, or to the end of the textblock otherwise
 * @param node the node to delete
 * @return Returns ETK_FALSE if the node was the last node of the line, ETK_TRUE otherwise
 */
Etk_Bool etk_textblock2_node_delete(Etk_Textblock2_Node *node)
{
   Etk_Textblock2 *tb;
   Etk_Textblock2_Line *line;
   Etk_Textblock2_Node *next;
   Etk_Textblock2_Iter *it;
   Etk_Bool res;
   
   if (!node || !(line = node->line) || !(tb = line->paragraph->tb))
      return ETK_FALSE;
   
   /* Make sure at least one node is remaining */
   if (!node->prev && !node->next)
   {
      _etk_tb2_node_create(line, NULL);
      res = ETK_FALSE;
   }
   else
      res = ETK_TRUE;
   
   /* Update the iterators */
   next = etk_textblock2_node_walk_next(node);
   for (it = tb->iters; it; it = it->next)
   {
      if (it->node == node)
      {
         if (next)
         {
            it->node = next;
            it->pos = 0;
            it->index = 0;
         }
         else
         {
            it->node = tb->last_paragraph->last_line->last_node;
            it->pos = it->node->unicode_length;
            it->index = etk_string_length_get(it->node->text);
         }
      }
   }
   
   /* Destroys the line */
   _etk_tb2_node_free(node);
   
   return res;
}

/**
 * @brief Gets the first node of the line
 * @param line a line
 * @return Returns the first node of the line
 */
Etk_Textblock2_Node *etk_textblock2_node_first_get(Etk_Textblock2_Line *line)
{
   if (!line)
      return NULL;
   return line->nodes;
}

/**
 * @brief Gets the last node of the line
 * @param line a line
 * @return Returns the last node of the line
 */
Etk_Textblock2_Node *etk_textblock2_node_last_get(Etk_Textblock2_Line *line)
{
   if (!line)
      return NULL;
   return line->last_node;
}

/**
 * @brief Gets the node before @a node in the current line
 * @param node a node
 * @return Returns the node before @a node in the current line,
 * or NULL if @a node is the first node of the line
 */
Etk_Textblock2_Node *etk_textblock2_node_prev_get(Etk_Textblock2_Node *node)
{
   if (!node)
      return NULL;
   return node->prev;
}

/**
 * @brief Gets the node after @a node in the current line
 * @param node a node
 * @return Returns the node after @a node in the current line,
 * or NULL if @a node is the last one of the line
 */
Etk_Textblock2_Node *etk_textblock2_node_next_get(Etk_Textblock2_Node *node)
{
   if (!node)
      return NULL;
   return node->next;
}

/**
 * @brief Walks to the previous node of the textblock. Unlike etk_textblock2_node_prev_get(),
 * this function will not return NULL if the node is the first node of the line. In this case, it
 * will return the last node of the previous line of the textblock
 * @param node a node
 * @return Returns the node before @a node in the textblock, or NULL if @a node is the first node of the textblock
 */
Etk_Textblock2_Node *etk_textblock2_node_walk_prev(Etk_Textblock2_Node *node)
{
   if (!node)
      return NULL;
   
   if (node->prev)
      return node->prev;
   else if (node->line->prev)
      return node->line->prev->last_node;
   else if (node->line->paragraph->prev)
      return node->line->paragraph->prev->last_line->last_node;
   else
      return NULL;
}


/**
 * @brief Walks to the next node of the textblock. Unlike etk_textblock2_node_next_get(),
 * this function will not return NULL if the node is the last node of the line. In this case, it
 * will return the first node of the next line of the textblock
 * @param node a node
 * @return Returns the node after @a node in the textblock, or NULL if @a node is the last node of the textblock
 */
Etk_Textblock2_Node *etk_textblock2_node_walk_next(Etk_Textblock2_Node *node)
{
   if (!node)
      return NULL;
   
   if (node->next)
      return node->next;
   else if (node->line->next)
      return node->line->next->nodes;
   else if (node->line->paragraph->next)
      return node->line->paragraph->next->lines->nodes;
   else
      return NULL;
}

/**************************
 * Textblock-Iter's funcs
 **************************/

/**
 * @brief Creates a new iterator for the textblock. The new iterator will be placed at the start
 * of the textblock. Iterators Are used to insert or delete text section at specific places
 * @param tb a textblock
 * @return Returns the new iterator
 */
Etk_Textblock2_Iter *etk_textblock2_iter_new(Etk_Textblock2 *tb)
{
   Etk_Textblock2_Iter *iter;
   
   if (!tb)
      return NULL;
   
   iter = malloc(sizeof(Etk_Textblock2_Iter));
   iter->tb = tb;
   iter->prev = NULL;
   iter->next = NULL;
   iter->node = tb->paragraphs->lines->nodes;
   iter->pos = 0;
   iter->index = 0;
   
   _etk_tb2_iter_reorder(iter, NULL);
   
   return iter;
}

/**
 * @brief Destroys the given iterator. It will no longer be usable
 * @param iter the iterator to free
 * @note Iterators are automatically freed when a textblock is destroyed
 */
void etk_textblock2_iter_free(Etk_Textblock2_Iter *iter)
{
   if (!iter)
      return;
   
   if (iter->prev)
      iter->prev->next = iter->next;
   if (iter->next)
      iter->next->prev = iter->prev;
   if (iter->tb->iters == iter)
      iter->tb->iters = iter->next;
   if (iter->tb->last_iter == iter)
      iter->tb->last_iter = iter->prev;
   
   free(iter);
}

/**
 * @brief Copies the iterator @a src to the iterator @a dest
 * @param dest the destination iterator
 * @param src the source iterator
 */
void etk_textblock2_iter_copy(Etk_Textblock2_Iter *dest, Etk_Textblock2_Iter *src)
{
   if (!dest || !src)
      return;
   if (src->tb != dest->tb)
   {
      ETK_WARNING("Unable to copy an iterator to another iterator belonging to a different textblock");
      return;
   }

   dest->node = src->node;
   dest->pos = src->pos;
   dest->index = src->index;
   _etk_tb2_iter_reorder(dest, src);
}

/**
 * @brief Compare the position of the two iterators
 * @param iter1 the first iterator to compare
 * @param iter2 the second iterator to compare
 * @return Returns 0 if the two iterators are at the same position, -1 if @a iter1
 * is before @a iter2 and 1 if @a iter1 is after @a iter2
 */
int etk_textblock2_iter_compare(Etk_Textblock2_Iter *iter1, Etk_Textblock2_Iter *iter2)
{
   Etk_Textblock2_Iter *it;
   
   if (!iter1 || !iter2 || !_etk_tb2_iter_is_valid(iter1) || !_etk_tb2_iter_is_valid(iter2))
      return 0;
   if (iter1->tb != iter2->tb)
   {
      ETK_WARNING("Unable to compare two iterators belonging to different textblocks");
      return 0;
   }
   
   if (iter1->node == iter2->node)
   {
      if (iter1->pos == iter2->pos)
         return 0;
      else if (iter1->pos < iter2->pos)
         return -1;
      else
         return 1;
   }
   
   for (it = iter1->tb->iters; it; it = it->next)
   {
      if (it == iter1)
         return -1;
      else if (it == iter2)
         return 1;
   }
   
   ETK_WARNING("Comparaison has failed: the iterators have not been found in the textblock. "
         "This is a bug of Etk. Please report!");
   return 0;
}

/**
 * @brief Moves the iterator at the start of the textblock
 * @param iter the iterator to move
 */
void etk_textblock2_iter_backward_start(Etk_Textblock2_Iter *iter)
{
   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return;
   
   iter->node = iter->tb->paragraphs->lines->nodes;
   iter->pos = 0;
   iter->index = 0;
   _etk_tb2_iter_reorder(iter, NULL);
}

/**
 * @brief Moves the iterator at the end of the textblock
 * @param iter the iterator to move
 */
void etk_textblock2_iter_forward_end(Etk_Textblock2_Iter *iter)
{
   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return;
   
   iter->node = iter->tb->last_paragraph->last_line->last_node;
   iter->pos = iter->node->unicode_length;
   iter->index = etk_string_length_get(iter->node->text);
   _etk_tb2_iter_reorder(iter, iter->tb->last_iter);
}

/**
 * @brief Moves the iterator backward by one character offset
 * @param iter an iterator
 * @return Returns ETK_TRUE if the character has been moved, ETK_FALSE otherwise
 * (meaning that the iterator was already at the start of the textblock)
 */
Etk_Bool etk_textblock2_iter_backward_char(Etk_Textblock2_Iter *iter)
{
   /*Etk_Textblock2_Node *n;
   
   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return ETK_FALSE;
   
   n = iter->tb->paragraphs->lines->nodes;
   if (iter->node == n && iter->pos == 0)
      return ETK_FALSE;
      
   if (iter->pos <= 1)
   {
      n = etk_textblock2_node_walk_prev(iter->node);
      while (n && n->unicode_length <= 0)
         n = etk_textblock2_node_walk_prev(n);
      
      if (n)
      {
         iter->node = n;
         iter->pos = n->unicode_length;
         iter->index = etk_string_length_get(n->text);
      }
      else
         etk_textblock2_iter_backward_start(iter);
   }
   else
   {
      iter->pos--;
      iter->index = evas_string_char_prev_get(etk_string_get(iter->node->text), iter->index, NULL);
   }
   
   return ETK_TRUE;*/
   
   /* TODO: make sure it's OK, and reorder the iter */
   return ETK_FALSE;
}

/**
 * @brief Moves the iterator fortward by one character offset
 * @param iter an iterator
 * @return Returns ETK_TRUE if the character has been moved, ETK_FALSE otherwise
 * (meaning that the iterator was already at the end of the textblock)
 */
Etk_Bool etk_textblock2_iter_forward_char(Etk_Textblock2_Iter *iter)
{
   /*Etk_Textblock2_Node *n;
   
   if (!iter || !_etk_tb2_iter_is_valid(iter))
      return ETK_FALSE;
   
   n = iter->tb->last_paragraph->last_line->last_node;
   if (iter->node == n && iter->pos == n->unicode_length)
      return ETK_FALSE;
      
   if (iter->pos >= iter->node->unicode_length)
   {
      n = etk_textblock2_node_walk_next(iter->node);
      while (n && n->unicode_length <= 0)
         n = etk_textblock2_node_walk_next(n);
      
      if (n)
      {
         iter->node = n;
         iter->pos = 1;
         iter->index = evas_string_char_prev_get(etk_string_get(n->text), 0, NULL);
      }
      else
         etk_textblock2_iter_forward_end(iter);
   }
   else
   {
      iter->pos++;
      iter->index = evas_string_char_next_get(etk_string_get(iter->node->text), iter->index, NULL);
   }
   
   return ETK_TRUE;*/
   
   /* TODO: make sure it's OK, and reorder the iter */
   return ETK_FALSE;
}

/**************************
 * Textblock-Object's funcs
 **************************/

/**
 * @brief Creates a new textblock-object that will be used to render the textblock's content.
 * A textblock can have several textblock-objects
 * @param tb a textblock
 * @param evas the Evas where to add the textblock-object
 * @return Returns the new textblock-object
 */
Evas_Object *etk_textblock2_object_add(Etk_Textblock2 *tb, Evas *evas)
{
   Evas_Object *tbo;
   Etk_TB2_Object_SD *sd;
   Etk_Textblock2_Paragraph *p;
   Etk_Textblock2_Line *l;
   static Evas_Smart *tbo_smart = NULL;
   static const Evas_Smart_Class tbo_sc =
   {
      "Textblock2_Object",
      EVAS_SMART_CLASS_VERSION,
      _etk_tb2_object_smart_add,
      _etk_tb2_object_smart_del,
      _etk_tb2_object_smart_move,
      _etk_tb2_object_smart_resize,
      _etk_tb2_object_smart_show,
      _etk_tb2_object_smart_hide,
      _etk_tb2_object_smart_color_set,
      _etk_tb2_object_smart_clip_set,
      _etk_tb2_object_smart_clip_unset,
      NULL
   };
   
   if (!tb || !evas)
      return NULL;
   
   if (!tbo_smart)
      tbo_smart = evas_smart_class_new(&tbo_sc);
   tbo = evas_object_smart_add(evas, tbo_smart);
   
   sd = evas_object_smart_data_get(tbo);
   sd->tb = tb;
   sd->cursor = etk_textblock2_iter_new(tb);
   sd->selection = etk_textblock2_iter_new(tb);
   
   /* Adds the object-lines for this textblock-object */
   for (p = tb->paragraphs; p; p = p->next)
   {
      for (l = p->lines; l; l = l->next)
         _etk_tb2_object_line_add(tbo, l);
   }
   
   tb->tbos = evas_list_append(tb->tbos, tbo);
   _etk_tb2_object_update_queue(tbo);

   return tbo;
}

/**
 * @brief Gets the cursor's iterator of the given textblock-object
 * @param tbo a textblock-object
 * @return Returns the cursor's iterator of the textblock-object
 */
Etk_Textblock2_Iter *etk_textblock2_object_cursor_get(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return NULL;
   return sd->cursor;
}

/**
 * @brief Gets the selection-bound's iterator of the given textblock-object
 * @param tbo a textblock-object
 * @return Returns the selection-bound's iterator of the textblock-object
 */
Etk_Textblock2_Iter *etk_textblock2_object_selection_bound_get(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return NULL;
   return sd->selection;
}

/** TODOC */
void etk_textblock2_object_offset_set(Evas_Object *tbo, int xoffset, int yoffset)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   sd->xoffset = xoffset;
   sd->yoffset = yoffset;
   _etk_tb2_object_update_queue(tbo);
}

/** TODOC */
void etk_textblock2_object_offset_get(Evas_Object *tbo, int *xoffset, int *yoffset)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
   {
      if (xoffset)   *xoffset = 0;
      if (yoffset)   *yoffset = 0;
   }
   else
   {
      if (xoffset)   *xoffset = sd->xoffset;
      if (yoffset)   *yoffset = sd->yoffset;
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the textblock */
static void _etk_tb2_constructor(Etk_Textblock2 *tb)
{
   if (!tb)
      return;
   
   tb->paragraphs = NULL;
   tb->last_paragraph = NULL;
   tb->iters = NULL;
   tb->last_iter = NULL;
   tb->tbos = NULL;
   _etk_tb2_paragraph_create(tb, NULL, ETK_TRUE);
}

/* Destroys the textblock */
static void _etk_tb2_destructor(Etk_Textblock2 *tb)
{
   while (tb->tbos)
      evas_object_del(tb->tbos->data);
   while (tb->iters)
      etk_textblock2_iter_free(tb->iters);
   while (tb->paragraphs)
      _etk_tb2_paragraph_free(tb->paragraphs);
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Creates a new paragraph and insert it after "prev". The paragraph will contain an empty line */
static Etk_Textblock2_Paragraph *_etk_tb2_paragraph_create(Etk_Textblock2 *tb, Etk_Textblock2_Paragraph *prev, Etk_Bool create_line)
{
   Etk_Textblock2_Paragraph *paragraph;
   
   if (!tb)
      return NULL;
   
   paragraph = malloc(sizeof(Etk_Textblock2_Paragraph));
   paragraph->lines = NULL;
   paragraph->last_line = NULL;
   paragraph->tb = tb;
   paragraph->align = 0.0;
   paragraph->left_margin = 0;
   paragraph->right_margin = 0;
   paragraph->wrap = ETK_TEXTBLOCK2_WRAP_WORD;
   
   paragraph->prev = prev;
   if (!prev)
   {
      paragraph->next = tb->paragraphs;
      if (tb->paragraphs)
         tb->paragraphs->prev = paragraph;
   }
   else
   {
      paragraph->next = prev->next;
      if (prev->next)
         prev->next->prev = paragraph;
      prev->next = paragraph;
   }
   if (!paragraph->prev)
      tb->paragraphs = paragraph;
   if (!paragraph->next)
      tb->last_paragraph = paragraph;
   
   if (create_line)
      _etk_tb2_line_create(paragraph, NULL, ETK_TRUE);
   
   return paragraph;
}

/* Creates a new line and insert it after "prev". The line will contain an empty node */
static Etk_Textblock2_Line *_etk_tb2_line_create(Etk_Textblock2_Paragraph *paragraph, Etk_Textblock2_Line *prev, Etk_Bool create_node)
{
   Etk_Textblock2_Line *line;
   Evas_List *l;
   
   if (!paragraph)
      return NULL;
   
   line = malloc(sizeof(Etk_Textblock2_Line));
   line->nodes = NULL;
   line->last_node = NULL;
   line->paragraph = paragraph;
   line->object_lines = NULL;
   
   line->prev = prev;
   if (!prev)
   {
      line->next = paragraph->lines;
      if (paragraph->lines)
         paragraph->lines->prev = line;
   }
   else
   {
      line->next = prev->next;
      if (prev->next)
         prev->next->prev = line;
      prev->next = line;
   }
   if (!line->prev)
      paragraph->lines = line;
   if (!line->next)
      paragraph->last_line = line;
   
   if (create_node)
      _etk_tb2_node_create(line, NULL);
   
   /* Add the corresponding object-line to each textblock-object */
   for (l = paragraph->tb->tbos; l; l = l->next)
      _etk_tb2_object_line_add(l->data, line);
   
   return line;
}

/* Creates a new empty node and insert it after "prev" */
static Etk_Textblock2_Node *_etk_tb2_node_create(Etk_Textblock2_Line *line, Etk_Textblock2_Node *prev)
{
   Etk_Textblock2_Node *node;
   
   if (!line)
      return NULL;
   
   node = malloc(sizeof(Etk_Textblock2_Node));
   node->line = line;
   node->type = ETK_TEXTBLOCK2_TAG_DEFAULT;
   node->text = NULL;
   node->unicode_length = 0;
   
   node->prev = prev;
   if (!prev)
   {
      node->next = line->nodes;
      if (line->nodes)
         line->nodes->prev = node;
   }
   else
   {
      node->next = prev->next;
      if (prev->next)
         prev->next->prev = node;
      prev->next = node;
   }
   if (!node->prev)
      line->nodes = node;
   if (!node->next)
      line->last_node = node;
   
   /* TODO: set the default properties */
   
   return node;
}

/* Destroys the given paragraph */
static void _etk_tb2_paragraph_free(Etk_Textblock2_Paragraph *paragraph)
{
   if (!paragraph)
      return;
   
   while (paragraph->lines)
      _etk_tb2_line_free(paragraph->lines);
   
   if (paragraph->prev)
      paragraph->prev->next = paragraph->next;
   if (paragraph->next)
      paragraph->next->prev = paragraph->prev;
   if (paragraph->tb->paragraphs == paragraph)
      paragraph->tb->paragraphs = paragraph->next;
   if (paragraph->tb->last_paragraph == paragraph)
      paragraph->tb->last_paragraph = paragraph->prev;
   
   free(paragraph);
}

/* Destroys the given line */
static void _etk_tb2_line_free(Etk_Textblock2_Line *line)
{
   Evas_List *l;
   
   if (!line)
      return;
   
   /* Remove the corresponding object-line from each textblock-object */
   for (l = line->paragraph->tb->tbos; l; l = l->next)
      _etk_tb2_object_line_remove(l->data, line);
   
   while (line->nodes)
      _etk_tb2_node_free(line->nodes);
   
   if (line->prev)
      line->prev->next = line->next;
   if (line->next)
      line->next->prev = line->prev;
   if (line->paragraph->lines == line)
      line->paragraph->lines = line->next;
   if (line->paragraph->last_line == line)
      line->paragraph->last_line = line->prev;
   
   free(line);
}

/* Destroys the given node */
static void _etk_tb2_node_free(Etk_Textblock2_Node *node)
{
   Evas_List *l;
   
   if (!node)
      return;
   
   /* Update the corresponding object-line of each textblock-object */
   for (l = node->line->paragraph->tb->tbos; l; l = l->next)
      _etk_tb2_object_line_update_queue(l->data, node->line, ETK_TRUE, ETK_TRUE);
   
   etk_object_destroy(ETK_OBJECT(node->text));
   //TODO: free(node->font.face);
   
   if (node->prev)
      node->prev->next = node->next;
   if (node->next)
      node->next->prev = node->prev;
   if (node->line->nodes == node)
      node->line->nodes = node->next;
   if (node->line->last_node == node)
      node->line->last_node = node->prev;
   
   free(node);
}

/* Splits the paragraph into two paragraphs at the iter's position. The new created paragraph, which is
 * placed after the split, is returned. The iterator will be moved to start of the new paragraph */
/* TODO: update object-line ? */
static Etk_Textblock2_Paragraph *_etk_tb2_paragraph_split(Etk_Textblock2_Paragraph *paragraph, Etk_Textblock2_Iter *iter)
{
   Etk_Textblock2_Paragraph *next_paragraph;
   Etk_Textblock2_Line *line, *next_line;
   
   if (!paragraph || !iter || !_etk_tb2_iter_is_valid(iter))
      return NULL;
   
   line = iter->node->line;
   next_paragraph = _etk_tb2_paragraph_create(iter->tb, paragraph, ETK_FALSE);
   next_line = _etk_tb2_line_split(line, iter);
   
   next_line->prev = NULL;
   next_paragraph->lines = next_line;
   next_paragraph->last_line = paragraph->last_line;
   
   line->next = NULL;
   paragraph->last_line = line;
   
   for (line = next_line; line; line = line->next)
      line->paragraph = next_paragraph;
   
   /* Copy the properties */
   next_paragraph->align = paragraph->align;
   next_paragraph->left_margin = paragraph->left_margin;
   next_paragraph->right_margin = paragraph->right_margin;
   next_paragraph->wrap = paragraph->wrap;
   
   return next_paragraph;
}

/* Splits the line into two lines at the iter's position. The new created line, which is
 * placed after the split, is returned. The iterator will be moved to start of the new line */
/* TODO: update object-line ? */
static Etk_Textblock2_Line *_etk_tb2_line_split(Etk_Textblock2_Line *line, Etk_Textblock2_Iter *iter)
{
   Etk_Textblock2_Line *next_line;
   Etk_Textblock2_Node *node, *next_node;
   
   if (!line || !iter || !_etk_tb2_iter_is_valid(iter))
      return NULL;
   
   node = iter->node;
   next_line = _etk_tb2_line_create(iter->node->line->paragraph, line, ETK_FALSE);
   next_node = _etk_tb2_node_split(node, iter);
   
   next_node->prev = NULL;
   next_line->nodes = next_node;
   next_line->last_node = line->last_node;
   
   node->next = NULL;
   line->last_node = node;
   
   for (node = next_node; node; node = node->next)
      node->line = next_line;
   
   return next_line;
}

/* Splits the node into two nodes at the iter's position. The new created node, which is
 * placed after the split, is returned. The iterator will be moved to start of the new node */
/* TODO: update object-line ? */
static Etk_Textblock2_Node *_etk_tb2_node_split(Etk_Textblock2_Node *node, Etk_Textblock2_Iter *iter)
{
   Etk_Textblock2_Node *next_node;
   Etk_Textblock2_Iter *it;
   int pos, index;
   
   if (!node || !iter || !_etk_tb2_iter_is_valid(iter))
      return NULL;
   if (iter->node != node)
   {
      ETK_WARNING("Error: Trying to split a node with an iterator that does not belong to this node. "
            "This is a bug of Etk. Please report!");
      return NULL;
   }
   
   next_node = _etk_tb2_node_create(node->line, node);
   
   /* Split the text between the two nodes */
   next_node->text = etk_string_set(next_node->text, &(etk_string_get(node->text))[iter->index]);
   next_node->unicode_length = node->unicode_length - iter->pos;
   node->text = etk_string_truncate(node->text, iter->index);
   node->unicode_length = iter->pos;
   
   /* Move the concerned iterators to the next node */
   pos = iter->pos;
   index = iter->index;
   for (it = iter->tb->iters; it; it = it->next)
   {
      if (it->node == node && it->pos >= pos)
      {
         it->node = next_node;
         it->pos -= pos;
         it->index -= index;
      }
   }
   
   /* Copy the properties */
   next_node->type = node->type;
   next_node->params = node->params;
   
   return next_node;
}

/* TODOC */
static Etk_Bool _etk_tb2_iter_is_valid(Etk_Textblock2_Iter *iter)
{
   /* TODO */
   return ETK_TRUE;
}

/* TODOC */
static void _etk_tb2_iter_reorder(Etk_Textblock2_Iter *iter, Etk_Textblock2_Iter *prev)
{
   Etk_Textblock2 *tb;
   
   if (!iter || !_etk_tb2_iter_is_valid(iter) || (prev && !_etk_tb2_iter_is_valid(prev)))
      return;
   if (prev && iter->tb != prev->tb)
   {
      ETK_WARNING("Unable to reorder an iterator after another iterator belonging to a different textblock. "
            "This is a bug of Etk. Please report!");
      return;
   }
   if (iter == prev)
      return;
   
   tb = iter->tb;
   
   /* Remove the iter from the list */
   if (iter->prev)
      iter->prev->next = iter->next;
   if (iter->next)
      iter->next->prev = iter->prev;
   if (tb->iters == iter)
      tb->iters = iter->next;
   if (tb->last_iter == iter)
      tb->last_iter = iter->prev;
   
   /* Reinsert the iter at the right place */
   iter->prev = prev;
   if (!prev)
   {
      iter->next = tb->iters;
      if (tb->iters)
         tb->iters->prev = iter;
   }
   else
   {
      iter->next = prev->next;
      if (prev->next)
         prev->next->prev = iter;
      prev->next = iter;
   }
   if (!iter->prev)
      tb->iters = iter;
   if (!iter->next)
      tb->last_iter = iter;
}

/**************************
 *
 * Textblock-Object's private functions
 *
 **************************/

/* Gets the object-line associated to the given textblock-object and textblock-line */
static Etk_TB2_Object_Line *_etk_tb2_object_line_get(Evas_Object *tbo, Etk_Textblock2_Line *line)
{
   Etk_TB2_Object_Line *object_line;
   
   if (!tbo || !line)
      return NULL;
   
   for (object_line = line->object_lines; object_line; object_line = object_line->fellow_next)
   {
      if (object_line->tbo == tbo)
         return object_line;
   }
   return NULL;
}

/* Adds a line to the given textblock-object */
static void _etk_tb2_object_line_add(Evas_Object *tbo, Etk_Textblock2_Line *line)
{
   Etk_TB2_Object_SD *sd;
   Etk_TB2_Object_Line *object_line;
   Etk_TB2_Object_Line *prev;
   
   if (!tbo || !line || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   object_line = malloc(sizeof(Etk_TB2_Object_Line));
   object_line->tbo = tbo;
   object_line->line = line;
   object_line->object = NULL;
   object_line->size.w = 0;
   object_line->size.h = 0;
   object_line->need_content_update = ETK_TRUE;
   object_line->need_geometry_update = ETK_TRUE;
   object_line->wrapped = ETK_FALSE;
   
   prev = _etk_tb2_object_line_get(tbo, line->prev);
   object_line->prev = prev;
   if (!prev)
   {
      object_line->next = sd->lines;
      if (sd->lines)
         sd->lines->prev = object_line;
   }
   else
   {
      object_line->next = prev->next;
      if (prev->next)
         prev->next->prev = object_line;
      prev->next = object_line;
   }
   if (!object_line->prev)
      sd->lines = object_line;
   
   object_line->fellow_prev = NULL;
   object_line->fellow_next = line->object_lines;
   if (object_line->fellow_next)
      object_line->fellow_next->fellow_prev = object_line;
   line->object_lines = object_line;
   
   _etk_tb2_object_update_queue(tbo);
}

/* Removes a line from the given textblock-object */
static void _etk_tb2_object_line_remove(Evas_Object *tbo, Etk_Textblock2_Line *line)
{
   Etk_TB2_Object_SD *sd;
   Etk_TB2_Object_Line *object_line;
   
   if (!tbo || !line || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   if ((object_line = _etk_tb2_object_line_get(tbo, line)))
   {
      if (object_line->prev)
         object_line->prev->next = object_line->next;
      if (object_line->next)
         object_line->next->prev = object_line->prev;
      if (sd->lines == object_line)
         sd->lines = object_line->next;
      
      if (object_line->fellow_prev)
         object_line->fellow_prev->fellow_next = object_line->fellow_next;
      if (object_line->fellow_next)
         object_line->fellow_next->fellow_prev = object_line->fellow_prev;
      if (line->object_lines == object_line)
         line->object_lines = object_line->fellow_next;
      
      if (object_line->object)
         evas_object_del(object_line->object);
      free(object_line);
      
      _etk_tb2_object_update_queue(tbo);
   }
}

/* Queues an update of the line "line" in the given textblock-object */
static void _etk_tb2_object_line_update_queue(Evas_Object *tbo, Etk_Textblock2_Line *line, Etk_Bool update_content, Etk_Bool update_geometry)
{
   Etk_TB2_Object_SD *sd;
   Etk_TB2_Object_Line *object_line;
   
   if (!tbo || !line || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   if ((object_line = _etk_tb2_object_line_get(tbo, line)))
   {
      object_line->need_content_update |= update_content;
      object_line->need_geometry_update |= (update_geometry || update_content);
      _etk_tb2_object_update_queue(tbo);
   }
}

/* Queues an update for the given textblock-object */
static void _etk_tb2_object_update_queue(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   if (!sd->update_job)
      sd->update_job = ecore_job_add(_etk_tb2_object_update, tbo);
}

/* Update the given textblock-object */
static void _etk_tb2_object_update(void *obj)
{
   Evas_Object *tbo;
   Etk_TB2_Object_SD *sd;
   Etk_TB2_Object_Line *line;
   Evas_Object *line_obj;
   int x, y, w, h;
   int tb_w, tb_h, native_w, native_h;
   Etk_Bool visible;
   int num_wrapped, num_lines, num_updates, num_visible;
   
   if (!(tbo = obj) || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   printf("Update Job\n");
   sd->update_job = NULL;
   evas_object_geometry_get(tbo, &x, &y, &w, &h);
   if (w == 0 || h == 0)
      return;
   w = ETK_MAX(w, 100);
   
   tb_w = 0;
   tb_h = 0;
   line_obj = NULL;
   
   num_wrapped = 0;
   num_lines = 0;
   num_updates = 0;
   num_visible = 0;
   for (line = sd->lines; line; line = line->next)
   {
      /* Update the geometry of the object-line if needed */
      if (line->need_geometry_update)
      {
         if (!line->object)
         {
            if (line_obj)
            {
               line->object = line_obj;
               line_obj = NULL;
            }
            else
               line->object = _etk_tb2_object_line_object_add(tbo);
            _etk_tb2_object_line_object_build(line->object, line->line);
         }
         else if (line->need_content_update)
            _etk_tb2_object_line_object_build(line->object, line->line);
         
         num_updates++;
         evas_object_resize(line->object, w, 300);
         evas_object_textblock_size_native_get(line->object, &native_w, &native_h);
         evas_object_textblock_size_formatted_get(line->object, &line->size.w, &line->size.h);
         line->wrapped = (native_w != line->size.w || native_h != line->size.h);
      }
      
      /* Now, render the line-object if the line is visible, or delete it otherwise */
      visible = ((tb_h <= (sd->yoffset + h)) && ((tb_h + line->size.h) >= sd->yoffset));
      if (visible)
      {
         if (!line->object)
         {
            line->object = _etk_tb2_object_line_object_add(tbo);
            _etk_tb2_object_line_object_build(line->object, line->line);
         }
         
         evas_object_move(line->object, x - sd->xoffset, y + tb_h - sd->yoffset);
         evas_object_resize(line->object, w, 300);
         evas_object_show(line->object);
         num_visible++;
      }
      else if (line->object)
      {
         if (!line_obj)
            line_obj = line->object;
         else
            evas_object_del(line->object);
         line->object = NULL;
      }
      
      line->need_geometry_update = ETK_FALSE;
      line->need_content_update = ETK_FALSE;
      
      tb_w = ETK_MAX(tb_w, line->size.w);
      tb_h += line->size.h;
      
      if (line->wrapped)
         num_wrapped++;
      num_lines++;
   }
   
   if (line_obj)
      evas_object_del(line_obj);
   
   printf("Nb lines: %d | Updated: %d | Wrapped: %d | Visible: %d\n", num_lines, num_updates, num_wrapped, num_visible);
}

/* Creates a new line-object (Evas-Textblock) for the given textblock-object */
static Evas_Object *_etk_tb2_object_line_object_add(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   Evas_Object *line_obj;
   Evas *evas;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)) || !(evas = evas_object_evas_get(tbo)))
      return NULL;
   
   line_obj = evas_object_textblock_add(evas);
   evas_object_textblock_style_set(line_obj, sd->style);
   evas_object_clip_set(line_obj, sd->clip);
   evas_object_smart_member_add(line_obj, tbo);
   
   return line_obj;
}

/* Builds the line-object (Evas-Textblock) from the given line of the textblock */
static void _etk_tb2_object_line_object_build(Evas_Object *lo, Etk_Textblock2_Line *line)
{
   Etk_Textblock2_Node *n;
   Evas_Textblock_Cursor *cursor;
   
   if (!lo || !line)
      return;
   
   evas_object_textblock_clear(lo);
   cursor = evas_object_textblock_cursor_new(lo);
   for (n = line->nodes; n; n = n->next)
   {
      /* TODO: apply format */
      evas_textblock_cursor_text_prepend(cursor, etk_string_get(n->text));
   }
   evas_textblock_cursor_free(cursor);
}

/**************************
 *
 * Textblock-Object's smart methods
 *
 **************************/

/* Called when the textblock-object is created */
static void _etk_tb2_object_smart_add(Evas_Object *tbo)
{
   Evas *evas;
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(evas = evas_object_evas_get(tbo)))
      return;
   
   sd = malloc(sizeof(Etk_TB2_Object_SD));
   sd->tb = NULL;
   sd->cursor = NULL;
   sd->selection = NULL;
   sd->lines = NULL;
   sd->style = NULL;
   sd->update_job = NULL;
   sd->xoffset = 0;
   sd->yoffset = 0;
   
   sd->clip = evas_object_rectangle_add(evas);
   evas_object_smart_member_add(sd->clip, tbo);
   
   /* TODO: themable style */
   sd->style = evas_textblock_style_new();
   evas_textblock_style_set(sd->style, "DEFAULT='font=Vera font_size=10 align=left color=#000000 wrap=word'");
   
   evas_object_smart_data_set(tbo, sd);
}

/* Called when the textblock-object is deleted */
static void _etk_tb2_object_smart_del(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   while (sd->lines)
      _etk_tb2_object_line_remove(tbo, sd->lines->line);
   
   etk_textblock2_iter_free(sd->cursor);
   etk_textblock2_iter_free(sd->selection);
   evas_textblock_style_free(sd->style);
   
   if (sd->update_job)
      ecore_job_del(sd->update_job);
   
   sd->tb->tbos = evas_list_remove(sd->tb->tbos, tbo);
   free(sd);
}

/* Called when the textblock-object is moved */
static void _etk_tb2_object_smart_move(Evas_Object *tbo, Evas_Coord x, Evas_Coord y)
{
   Evas_List *members, *l;
   Evas_Object *m;
   Evas_Coord ox, oy, mx, my;
   
   if (!tbo)
      return;
   
   evas_object_geometry_get(tbo, &ox, &oy, NULL, NULL);
   
   members = evas_object_smart_members_get(tbo);
   for (l = members; l; l = l->next)
   {
      m = l->data;
      evas_object_geometry_get(m, &mx, &my, NULL, NULL);
      evas_object_move(m, mx + (x - ox), my + (y - oy));
   }
   evas_list_free(members);
}

/* Called when the textblock-object is resized */
static void _etk_tb2_object_smart_resize(Evas_Object *tbo, Evas_Coord w, Evas_Coord h)
{
   Etk_TB2_Object_SD *sd;
   Etk_TB2_Object_Line *line;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   
   evas_object_resize(sd->clip, w, h);
   
   for (line = sd->lines; line; line = line->next)
   {
      if (line->wrapped || line->size.w > w)
         line->need_geometry_update = ETK_TRUE;
   }
   _etk_tb2_object_update_queue(tbo);
}

/* Called when the textblock-object is shown */
static void _etk_tb2_object_smart_show(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   evas_object_show(sd->clip);
}

/* Called when the textblock-object is hidden */
static void _etk_tb2_object_smart_hide(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   evas_object_hide(sd->clip);
}

/* Called when the textblock-object's color is changed */
static void _etk_tb2_object_smart_color_set(Evas_Object *tbo, int r, int g, int b, int a)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   evas_object_color_set(sd->clip, r, g, b, a);
}

/* Called when the textblock-object is clipped */
static void _etk_tb2_object_smart_clip_set(Evas_Object *tbo, Evas_Object *clip)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   evas_object_clip_set(sd->clip, clip);
}

/* Called when the textblock-object is unclipped */
static void _etk_tb2_object_smart_clip_unset(Evas_Object *tbo)
{
   Etk_TB2_Object_SD *sd;
   
   if (!tbo || !(sd = evas_object_smart_data_get(tbo)))
      return;
   evas_object_clip_unset(sd->clip);
}

/** @} */
