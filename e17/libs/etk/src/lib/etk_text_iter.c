#include "etk_text_iter.h"
#include <stdlib.h>
#include <Evas.h>
#include "etk_text_buffer.h"

static void _etk_text_iter_constructor(Etk_Text_Iter *text_iter);
static void _etk_text_iter_destructor(Etk_Text_Iter *text_iter);

/**
 * @brief Gets the type of an Etk_Text_Iter
 * @return Returns the type on an Etk_Text_Iter
 */
Etk_Type *etk_text_iter_type_get()
{
   static Etk_Type *text_iter_type = NULL;

   if (!text_iter_type)
   {
      text_iter_type = etk_type_new("Etk_Text_Iter", ETK_OBJECT_TYPE, sizeof(Etk_Text_Iter),
         ETK_CONSTRUCTOR(_etk_text_iter_constructor), ETK_DESTRUCTOR(_etk_text_iter_destructor));
   }

   return text_iter_type;
}

/**
 * @brief Creates a new text iterator
 * @param text_buffer the text buffer which the iterator will be linked to
 * @return Returns the new text iterator
 */
Etk_Text_Iter *etk_text_iter_new(Etk_Text_Buffer *text_buffer)
{
   Etk_Text_Iter *new_iter;
   
   if (!text_buffer)
      return NULL;
   
   new_iter = ETK_TEXT_ITER(etk_object_new(ETK_TEXT_ITER_TYPE, NULL));
   new_iter->text_buffer = text_buffer;
   text_buffer->iterators = evas_list_append(text_buffer->iterators, new_iter);
   
   return new_iter;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the text iter */
static void _etk_text_iter_constructor(Etk_Text_Iter *text_iter)
{
   if (!text_iter)
      return;
   
   text_iter->text_buffer = NULL;
   text_iter->formatted_pos = 0;
   text_iter->unformatted_pos = 0;
}

/* Destroys the text iter */
static void _etk_text_iter_destructor(Etk_Text_Iter *text_iter)
{
   if (text_iter && text_iter->text_buffer)
      text_iter->text_buffer->iterators = evas_list_remove(text_iter->text_buffer->iterators, text_iter);
}

